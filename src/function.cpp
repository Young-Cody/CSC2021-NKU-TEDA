#include "function.h"
#include "unit.h"

Function::Function(Unit *u, symbol_entry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this, symbol_table::gen_label(1));
    exit = new BasicBlock(this, symbol_table::gen_label(1));
    sym_ptr = s;
    new_label = 0;
    critical = false;
    insert_point = nullptr;
    front = rear = new DummyInstruction();
    front->setNext(rear);
    rear->setPrev(front);
    front->setParent(exit);
}

Function::~Function()
{
    for (auto &i : block_list)
        delete i;

    auto p = front;
    Instruction *t;
    while (p != rear->getNext())
    {
        t = p;
        p = p->getNext();
        delete t;
    }
}

void Function::insertInst(Instruction *inst)
{
    inst->setNext(rear->getNext());
    inst->setPrev(rear);
    rear->getNext()->setPrev(inst);
    rear->setNext(inst);
    rear = inst;
}

void Function::insertBefore(Instruction *dst, Instruction *src)
{
    dst->setPrev(src->getPrev());
    dst->setNext(src);
    src->getPrev()->setNext(dst);
    src->setPrev(dst);
}

void Function::insertAfter(Instruction *dst, Instruction *src)
{
    dst->setPrev(src);
    dst->setNext(src->getNext());
    src->getNext()->setPrev(dst);
    src->setNext(dst);
}

// erase instructions in the range of [start, end] in the instruction list.
void Function::erase(Instruction *start, Instruction *end)
{
    if(end == rear)
        rear = start->getPrev();
    remove(start, end);
    Instruction *inst = start;
    while (inst != end->getNext())
    {
        Instruction *t = inst;
        Operand *def = t->getDef();
        if(def != nullptr)
            def->removeDef(t);
        for(auto use: t->getUse())
            use->removeUse(t);
        inst = inst->getNext();
        delete t;
    }
}

// erase instruction in the instruction list.
void Function::erase(Instruction *inst)
{
    if(inst == rear)
        rear = inst->getPrev();
    remove(inst);
    delete inst;
}

// erase the basicblock bb from its block_list.
void Function::erase(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
    delete bb;
}

// remove instructions in the range of [start, end] from the instruction list but not delete them.
void Function::remove(Instruction *start, Instruction *end)
{
    if(end == rear)
        rear = rear->getPrev();
    start->getPrev()->setNext(end->getNext());
    end->getNext()->setPrev(start->getPrev());
}

// remove instruction from the instruction list but not delete it.
void Function::remove(Instruction *inst)
{
    if(inst == rear)
        rear = rear->getPrev();
    inst->getPrev()->setNext(inst->getNext());
    inst->getNext()->setPrev(inst->getPrev());
}

void Function::constructCFG()
{
    RetInstruction *ret;
    if (isvoid(sym_ptr->tp->operand))
    {
        ret = new RetInstruction(nullptr, exit);
        ret->setRetVoid();
    }
    for (auto &i : block_list)
    {
        if (i == exit || i == entry)
            continue;
        if (i->empty())
        {
            i->addSucc(exit);
            exit->addPred(i);
            i->insert(new BranchInstruction(exit));
            continue;
        }
        Instruction *last = i->end();
        Instruction *next = last->getNext();
        BasicBlock *next_bb;
        next_bb = next->getParent();
        if (last->isBranch())
        {
            auto to = dynamic_cast<BranchInstruction *>(last)->getBranch();
            i->addSucc(to);
            to->addPred(i);
        }
        else if (!last->isRet())
        {
            BranchInstruction *inst = new BranchInstruction(next_bb);
            i->insertAfter(inst, last);
            i->addSucc(next_bb);
            next_bb->addPred(i);
        }
        else if (last->isRet())
        {
            i->addSucc(exit);
            exit->addPred(i);
        }
        if (last->isCondBr())
        {
            dynamic_cast<IfInstruction *>(last)->setFalseBranch(next_bb);
            i->addSucc(next_bb);
            next_bb->addPred(i);
        }
    }
}

void Function::reLabel()
{
    if (sym_ptr->tp->func_params != nullptr)
    {
        int i = 0;
        while (true)
        {
            symbol_entry *param = sym_ptr->tp->func_params[i];
            if (param == nullptr)
                break;
            param->label = getLabel(param->label);
            i++;
        }
    }
    std::set<BasicBlock *> v;
    std::vector<BasicBlock *> q;
    q.push_back(*entry->succ_begin());
    v.insert({*entry->succ_begin()});
    while (!q.empty())
    {
        auto p = q.front();
        q.erase(q.begin());
        p->reLabel();
        for (auto i = p->succ_begin(); i != p->succ_end(); i++)
        {
            if (v.find(*i) == v.end())
            {
                v.insert(*i);
                q.push_back(*i);
            }
        }
    }
}

int Function::getLabel(int label)
{
    if (label_map.find(label) == label_map.end())
        label_map[label] = new_label++;
    return label_map[label];
}

void Function::compute_dom()
{
    for (auto &item : block_list)
    {
        item->getDom().clear();
        if (item == entry)
            item->getDom().insert(entry);
        else
            item->getDom().insert(block_list.begin(), block_list.end());
    }
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto &item : block_list)
        {
            if (item == entry)
                continue;
            auto &domi = item->getDom();
            std::set<BasicBlock *> all = std::set<BasicBlock *>(block_list.begin(), block_list.end());
            for (auto i = item->pred_begin(); i != item->pred_end(); i++)
            {
                std::set<BasicBlock *> intersect;
                set_intersection(all.begin(), all.end(), (*i)->getDom().begin(), (*i)->getDom().end(), inserter(intersect, intersect.begin()));
                all = intersect;
            }
            all.insert(item);
            if (all != domi)
            {
                domi = all;
                flag = true;
            }
        }
    }
}

void Function::compute_idom()
{
    std::map<BasicBlock *, std::set<BasicBlock *>> tmp;
    for (auto &node : block_list)
    {
        tmp[node] = node->getDom();
        tmp[node].erase(node);
        node->getDomTree().clear();
    }
    for (auto &node : block_list)
    {
        if (node == entry)
            continue;
        std::vector<BasicBlock *> bbs(tmp[node].begin(), tmp[node].end());
        for (auto &s : bbs)
            for (auto &t : bbs)
            {
                if (s == t)
                    continue;
                if (tmp[s].find(t) != tmp[s].end())
                    tmp[node].erase(t);
            }
    }
    for (auto &node : block_list)
    {
        BasicBlock *t;
        if (node == entry)
            t = nullptr;
        else
        {
            t = *tmp[node].begin();
            t->getDomTree().push_back(node);
        }
        node->getIdom() = t;
    }
}

void Function::compute_dom_frontier()
{
    compute_dom();
    compute_idom();
    for (auto &u : block_list)
        u->getDf().clear();
    for (auto &u : block_list)
    {
        for (auto v = u->succ_begin(); v != u->succ_end(); v++)
        {
            if(*v == exit)
                continue;
            BasicBlock *p = u;
            while (!(p != *v && (*v)->isDominatedBy(p)))
            {
                p->getDf().insert(*v);
                p = p->getIdom();
            }
        }
    }
}

void Function::compute_reverse_dom() 
{
    for (auto &item : block_list)
    {
        item->getDom().clear();
        if (item == exit)
            item->getDom().insert(exit);
        else
            item->getDom().insert(block_list.begin(), block_list.end());
    }
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto &item : block_list)
        {
            if (item == exit)
                continue;
            auto &domi = item->getDom();
            std::set<BasicBlock *> all = std::set<BasicBlock *>(block_list.begin(), block_list.end());
            for (auto i = item->succ_begin(); i != item->succ_end(); i++)
            {
                std::set<BasicBlock *> intersect;
                set_intersection(all.begin(), all.end(), (*i)->getDom().begin(), (*i)->getDom().end(), inserter(intersect, intersect.begin()));
                all = intersect;
            }
            all.insert(item);
            if (all != domi)
            {
                domi = all;
                flag = true;
            }
        }
    }
}

void Function::compute_reverse_idom() 
{
    std::map<BasicBlock *, std::set<BasicBlock *>> tmp;
    for (auto &node : block_list)
    {
        tmp[node] = node->getDom();
        tmp[node].erase(node);
        node->getDomTree().clear();
    }
    for (auto &node : block_list)
    {
        if (node == exit)
            continue;
        std::vector<BasicBlock *> bbs(tmp[node].begin(), tmp[node].end());
        for (auto &s : bbs)
            for (auto &t : bbs)
            {
                if (s == t)
                    continue;
                if (tmp[s].find(t) != tmp[s].end())
                    tmp[node].erase(t);
            }
    }
    for (auto &node : block_list)
    {
        BasicBlock *t;
        if (node == exit)
            t = nullptr;
        else
        {
            t = *tmp[node].begin();
            t->getDomTree().push_back(node);
        }
        node->getIdom() = t;
    }
}

void Function::compute_reverse_dom_frontier(){
    compute_reverse_dom();
    compute_reverse_idom();
    for (auto &u : block_list)
        u->getRDF().clear();
    for (auto &u : block_list)
    {
        for (auto v = u->pred_begin(); v != u->pred_end(); v++)
        {
            BasicBlock *p = u;
            while (!(p != *v && (*v)->isDominatedBy(p)))
            {
                p->getRDF().insert(*v);
                p = p->getIdom();
            }
        }
    }
}

bool mycompare(BasicBlock* &bb1, BasicBlock* &bb2){
    return bb1->isDominatedBy(bb2);
}

BasicBlock* Function::compute_nearest_marked_postdominator(Instruction* ins){
    BasicBlock* bb = ins->getParent();
    for(auto it = bb->getRIdom(); it; it = it->getRIdom()){
        if(it->isMarked()){
            return it;
        }
    }
    return bb->getRIdom();
}

std::set<BasicBlock *> Function::compute_natural_loop(BasicBlock *u, BasicBlock *v)
{
    std::set<BasicBlock *> loop;
    std::vector<BasicBlock *> s;
    if (u == v)
        loop.insert(u);
    else
    {
        loop.insert(u);
        loop.insert(v);
        s.push_back(u);
    }

    while (!s.empty())
    {
        auto t = s.back();
        s.pop_back();
        for (auto item = t->pred_begin(); item != t->pred_end(); item++)
        {
            if (loop.find(*item) == loop.end())
            {
                s.push_back(*item);
                loop.insert(*item);
            }
        }
    }
    return loop;
}

void Function::print()
{
    reLabel();
    printf("define dso_local ");
    printf("%s ", type_system::getName(sym_ptr->tp->operand).c_str());
    if (sym_ptr->tp->func_params == nullptr)
        printf("@%s() #0 {\n", sym_ptr->name.c_str());
    else
    {
        symbol_entry *se = sym_ptr->tp->func_params[0];
        printf("@%s(%s %%%d", sym_ptr->name.c_str(), type_system::getName(se->tp).c_str(), se->label);
        int i = 1;
        while (sym_ptr->tp->func_params[i] != nullptr)
        {
            printf(", %s %%%d", type_system::getName(sym_ptr->tp->func_params[i]->tp).c_str(),
                   sym_ptr->tp->func_params[i]->label);
            i++;
        }
        printf(") #0 {\n");
    }

    std::set<BasicBlock *> v;
    std::vector<BasicBlock *> q;
    q.push_back(*entry->succ_begin());
    v.insert(*entry->succ_begin());
    while (!q.empty())
    {
        auto p = q.front();
        q.erase(q.begin());
        p->print();
        for (auto i = p->succ_begin(); i != p->succ_end(); i++)
        {
            if (v.find(*i) == v.end())
            {
                v.insert(*i);
                q.push_back(*i);
            }
        }
    }
    printf("}\n");
}

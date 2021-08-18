#include "ast.h"
#include <iostream>
#include <vector>
using namespace std;

Ast::Ast(ostream &o) : out(o)
{
    root = nullptr;
    func = nullptr;
    unit = nullptr;
    node_seq = 0;
}

Node *Ast::NewRoot(int kind, int kind_kind, symbol_entry *s,
                   Node *child1, Node *child2, Node *child3)
{
    Node *t = new Node;
    t->child[0] = child1;
    t->child[1] = child2;
    t->child[2] = child3;
    t->sym_ptr = s;
    t->kind = kind;
    t->kind_kind = kind_kind;
    t->sibling = nullptr;
    t->seq = node_seq++;
    t->label.truelist = t->label.falselist = t->label.nextlist = nullptr;
    t->label.begininst = nullptr;
    root = t;
    return t;
}

void Ast::printAst()
{
    root->output(out);
}

Unit *Ast::gen_code()
{
    unit = new Unit();
    unit->setGlobal(globals);
    unit->setFuncDecl(functions);
    recursive_gen_code(root->child[0]);
    return unit;
}

void Node::addSibling(Node *t)
{
    Node *p = this;
    while (p->sibling)
        p = p->sibling;
    p->sibling = t;
}

void Node::output(ostream &out)
{
    out << "#";
    printNodeInfo(out);
    out << "\n";
    for (int i = 0; i < 3; i++)
        if (child[i])
            child[i]->output(out);
    if (sibling)
        sibling->output(out);
}

void Node::printNodeInfo(ostream &out)
{
    out << "@" << seq << '\t';
    switch (kind)
    {
    case NODE_PROG:
        out << "NODE_PROG";
        break;
    case NODE_DECL:
    {

        out << "NODE_DECL\t";
        out << sym_ptr->name << '\t';
        if (isarray(sym_ptr->tp))
        {
            if (sym_ptr->array_value)
            {
                int length = sym_ptr->tp->size / sym_ptr->tp->align;
                for (int i = 0; i < length; i++)
                    out << sym_ptr->array_value[i] << ' ';
            }
        }
        else if (sym_ptr->scope == GLOBAL || isconst(sym_ptr->tp))
            out << sym_ptr->constant;
        type *p = sym_ptr->tp;
        while (isarray(p))
            p = p->operand;

        if (sym_ptr->scope > GLOBAL && !isconst(p))
        {
            out << "offset: ";
            out << sym_ptr->offset << '\t';
        }
    }
    break;
    case NODE_EXPR:
        out << "NODE_EXPR";
        switch (kind_kind)
        {
        case OP_E:
            out << "\tOP_E";
            break;
        case OP_NE:
            out << "\tOP_NE";
            break;
        case OP_L:
            out << "\tOP_L";
            break;
        case OP_G:
            out << "\tOP_G";
            break;
        case OP_LE:
            out << "\tOP_LE";
            break;
        case OP_GE:
            out << "\tOP_G";
            break;
        case OP_AND:
            out << "\tOP_AND";
            break;
        case OP_OR:
            out << "\tOP_OR";
            break;
        case OP_NOT:
            out << "\tOP_NOT";
            break;
        case OP_ADD:
            out << "\tOP_ADD";
            break;
        case OP_MINUS:
            out << "\tOP_MINUS";
            break;
        case OP_MUL:
            out << "\tOP_MUL";
            break;
        case OP_DIV:
            out << "\tOP_DIV";
            break;
        case OP_MOD:
            out << "\tOP_MOD";
            break;
        case OP_UMINUS:
            out << "\tOP_UMINUS";
            break;
        case OP_FUNC:
            out << "\tOP_FUNC";
            break;
        case OP_ARRAY:
            out << "\tOP_ARRAY";
            break;
        default:
            out << "\t" << sym_ptr->name;
            break;
        }
        break;
    case NODE_STMT:
        out << "NODE_STMT\t";
        switch (kind_kind)
        {
        case STMT_WHILE:
            out << "STMT_WHILE";
            break;
        case STMT_IF:
            out << "STMT_IF";
            break;
        case STMT_BLANK:
            out << "STMT_BLANK";
            break;
        case STMT_CONTINUE:
            out << "STMT_CONTINUE";
            break;
        case STMT_BREAK:
            out << "STMT_BREAK";
            break;
        case STMT_RETURN:
            out << "STMT_RETURN";
            break;
        case STMT_COMPOUND:
            out << "STMT_COMPOUND";
            break;
        case STMT_EXPR:
            out << "STMT_EXPR";
            break;
        case STMT_FUNC:
            out << "STMT_FUNC";
            break;
        case STMT_ASSIGN:
            out << "STMT_ASSIGN";
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    if (child[0] != nullptr || child[1] != nullptr || child[2] != nullptr)
    {
        out << "\tchild: ";
        for (int i = 0; i < 3; i++)
        {
            if (child[i])
                out << '@' << child[i]->seq << ' ';
        }
    }
    if (sibling)
        out << "\tsibling: @" << sibling->seq << ' ';
}

void Ast::recursive_gen_code(Node *t)
{
    if (t->kind == NODE_STMT)
        stmt_gen_code(t);
    else if (t->kind == NODE_DECL)
        decl_gen_code(t);
    else if (t->kind == NODE_EXPR)
        expr_gen_code(t);

    if (t->sibling)
    {
        recursive_gen_code(t->sibling);
        if(t->sibling->label.begininst)
        {
            backPatch(t->label.nextlist, t->sibling->label.begininst);
            t->label.nextlist = t->sibling->label.nextlist;
        }
        else
            t->label.nextlist = merge(t->label.nextlist, t->sibling->label.nextlist);
        
        t->label.begininst = get_begininst(t->label.begininst, t->sibling->label.begininst, nullptr);
    }
}

void Ast::decl_gen_code(Node *t)
{
    if (isconst(t->sym_ptr->tp))
        return;
    if (t->sym_ptr->scope >= LOCAL)
    {
        t->sym_ptr->label = symbol_table::gen_label(1);
        t->label.begininst = new AllocaInstruction(create_operand(t), func->getInsertPoint());
    }
    if (t->child[0] == nullptr)
        return;

    if (isarray(t->sym_ptr->tp))
    {
        int offset = 0;
        recursive_gen_decl(t->child[0], offset, create_operand(t));
        t->label.begininst = get_begininst(t->label.begininst, t->child[0]->label.begininst, nullptr);
    }
    else
    {
        Instruction *inst;
        recursive_gen_code(t->child[0]);
        inst = new StoreInstruction(create_operand(t), create_operand(t->child[0]), func->getInsertPoint());
        t->label.begininst = get_begininst(t->label.begininst, t->child[0]->label.begininst, inst);
    }
}

void Ast::gen_logic(Node *t)
{
    Operand *c = unit->getConstant(0);
    Instruction *instcmp, *instif, *instbr;
    Operand *res = unit->getTemp(symbol_table::gen_temp(typesys.int_type));
    instcmp = new CmpInstruction(CmpInstruction::NE, res, create_operand(t), c, func->getInsertPoint());
    instif = new IfInstruction(nullptr, res, func->getInsertPoint());
    instbr = new BranchInstruction(nullptr, func->getInsertPoint());
    t->label.falselist = new std::vector<Instruction *>({instbr});
    t->label.truelist = new std::vector<Instruction *>({instif});
    t->label.begininst = get_begininst(t->label.begininst, instcmp, nullptr);
}

void Ast::recursive_gen_decl(Node *t, int &offset, Operand *base)
{
    Node *p = t;
    while (p)
    {
        if (p->kind_kind == OP_INITLIST)
        {
            int offset_t;
            offset_t = offset;
            if (p->child[0])
            {
                if (p->child[0]->sym_ptr == nullptr)
                    p->child[0]->sym_ptr = new symbol_entry;
                p->child[0]->sym_ptr->tp = t->sym_ptr->tp->operand;
                recursive_gen_decl(p->child[0], offset_t, base);
                t->label.begininst = get_begininst(t->label.begininst, p->child[0]->label.begininst, nullptr);
            }
            offset += (t->sym_ptr->tp->size / t->sym_ptr->tp->align);
            int zero_init = offset - offset_t;
            Operand *zero = unit->getConstant(0);
            for (int i = 0; i < zero_init; i++)
            {
                Operand *index = unit->getConstant(offset_t + i);
                Operand *dst = unit->getTemp(symbol_table::gen_temp(typesys.ptr(typesys.int_type)));
                Instruction *addrinst;
                addrinst = new AddrInstruction(dst, base, index, func->getInsertPoint());
                new StoreInstruction(dst, zero, func->getInsertPoint());
                t->label.begininst = get_begininst(t->label.begininst, addrinst, nullptr);
            }
        }
        else
        {
            expr_gen_code(p->child[0]);
            Operand *src = create_operand(p->child[0]);
            Operand *index = unit->getConstant(offset);
            Operand *dst = unit->getTemp(symbol_table::gen_temp(typesys.ptr(typesys.int_type)));
            Instruction *addrinst;
            addrinst = new AddrInstruction(dst, base, index, func->getInsertPoint());
            new StoreInstruction(dst, src, func->getInsertPoint());
            t->label.begininst = get_begininst(t->label.begininst, p->label.begininst, addrinst);
            offset += 1;
        }
        p = p->sibling;
    }
}

Instruction *Ast::get_begininst(Instruction *inst1, Instruction *inst2, Instruction *inst3)
{
    if (inst1 != nullptr)
        return inst1;
    if (inst2 != nullptr)
        return inst2;
    return inst3;
}

void Ast::expr_gen_code(Node *t, bool cond)
{
    Node *e1 = t->child[0];
    Node *e2 = t->child[1];
    int op = t->kind_kind;
    if (op == OP_FUNC)
    {
        Node *p = e1;
        std::vector<Operand *> param;
        while (p)
        {
            expr_gen_code(p);
            Operand *oprd;
            oprd = create_operand(p);
            param.push_back(oprd);
            t->label.begininst = get_begininst(t->label.begininst, p->label.begininst, nullptr);
            p = p->sibling;
        }
        Operand *dst = nullptr;
        if (t->sym_ptr->tp != typesys.void_type)
            dst = create_operand(t);
        Instruction *inst;
        inst = new CallInstruction(dst, param, t->sym_ptr, func->getInsertPoint());
        t->label.begininst = get_begininst(t->label.begininst, inst, nullptr);
        Function *succ = sym2func[functions->lookup(t->sym_ptr->name)];
        if(succ != nullptr)
        {
            func->getSuccs()[succ].insert(inst);
            succ->getPreds()[func].insert(inst);
            dynamic_cast<CallInstruction*>(inst)->setCallee(succ);
            if(succ->isCritical())
                func->setCritical();
        }
        else
            func->setCritical();
        return;
    }
    Operand *o, *o1, *o2;
    o = create_operand(t);
    if (op >= OP_E && op <= OP_GE)
    {
        int op = -1;
        expr_gen_code(e1);
        o1 = create_operand(e1);
        expr_gen_code(e2);
        o2 = create_operand(e2);

        switch (t->kind_kind)
        {
        case OP_E:
            op = CmpInstruction::E;
            break;
        case OP_NE:
            op = CmpInstruction::NE;
            break;
        case OP_L:
            op = CmpInstruction::L;
            break;
        case OP_G:
            op = CmpInstruction::G;
            break;
        case OP_LE:
            op = CmpInstruction::LE;
            break;
        case OP_GE:
            op = CmpInstruction::GE;
            break;
        default:
            break;
        }

        Instruction *instcmp, *instif, *instbr;
        instcmp = new CmpInstruction(op, o, o1, o2, func->getInsertPoint());
        if(cond)
        {
            instif = new IfInstruction(nullptr, o, func->getInsertPoint());
            instbr = new BranchInstruction(nullptr, func->getInsertPoint());
            t->label.truelist = new std::vector<Instruction *>({instif});
            t->label.falselist = new std::vector<Instruction *>({instbr});
        }
        t->label.begininst = get_begininst(e1->label.begininst, e2->label.begininst, instcmp);
    }
    else if (op == OP_AND)
    {
        expr_gen_code(e1, cond);
        if (e1->label.truelist == nullptr)
            gen_logic(e1);
        expr_gen_code(e2, cond);
        if (e2->label.truelist == nullptr)
            gen_logic(e2);
        backPatch(e1->label.truelist, e2->label.begininst);
        t->label.truelist = e2->label.truelist;
        t->label.falselist = merge(e1->label.falselist, e2->label.falselist);
        t->label.begininst = get_begininst(e1->label.begininst, e2->label.begininst, nullptr);
    }
    else if (op == OP_OR)
    {
        expr_gen_code(e1, cond);
        if (e1->label.truelist == nullptr)
            gen_logic(e1);
        expr_gen_code(e2, cond);
        if (e2->label.truelist == nullptr)
            gen_logic(e2);
        backPatch(e1->label.falselist, e2->label.begininst);
        t->label.truelist = merge(e1->label.truelist, e2->label.truelist);
        t->label.falselist = e2->label.falselist;
        t->label.begininst = get_begininst(e1->label.begininst, e2->label.begininst, nullptr);
    }
    else if (op == OP_NOT)
    {
        if(cond)
        {
            expr_gen_code(e1, cond);
            if (e1->label.truelist == nullptr)
                gen_logic(e1);
            t->label.truelist = e1->label.falselist;
            t->label.falselist = e1->label.truelist;
            t->label.begininst = e1->label.begininst;
        }
        else
        {
            recursive_gen_code(e1);
            o1 = create_operand(e1);
            Instruction *inst;
            inst = new UnaryInstruction(UnaryInstruction::NOT, o, o1, func->getInsertPoint());
            t->label.begininst = get_begininst(e1->label.begininst, inst, nullptr);
        }
    }
    else if (op >= OP_ADD && op <= OP_MOD)
    {
        recursive_gen_code(e1);
        o1 = create_operand(e1);
        recursive_gen_code(e2);
        o2 = create_operand(e2);
        unsigned op = -1;
        switch (t->kind_kind)
        {
        case OP_ADD:
            op = BinaryInstruction::ADD;
            break;
        case OP_MINUS:
            op = BinaryInstruction::MINUS;
            break;
        case OP_MUL:
            op = BinaryInstruction::MUL;
            break;
        case OP_DIV:
            op = BinaryInstruction::DIV;
            break;
        case OP_MOD:
            op = BinaryInstruction::MOD;
            break;
        }
        Instruction *inst;
        inst = new BinaryInstruction(op, o, o1, o2, func->getInsertPoint());
        t->label.begininst = get_begininst(e1->label.begininst, e2->label.begininst, inst);
    }
    else if (op == OP_UMINUS)
    {
        recursive_gen_code(e1);
        o1 = create_operand(e1);
        Instruction *inst;
        inst = new UnaryInstruction(UnaryInstruction::UMINUS, o, o1, func->getInsertPoint());
        t->label.truelist = e1->label.truelist;
        t->label.falselist = e1->label.falselist;
        t->label.begininst = get_begininst(e1->label.begininst, inst, nullptr);
    }
    else if (op == OP_ARRAY)
    {
        Operand *base;
        Operand *index;
        bool load = !isarray(t->sym_ptr->tp);
        index = create_index(t, &base);
        t->sym_ptr = symbol_table::gen_temp(typesys.ptr(typesys.int_type));
        Operand *src_addr = create_operand(t);
        Instruction *addrinst;
        addrinst = new AddrInstruction(src_addr, base, index, func->getInsertPoint());
        if (load)
        {
            t->sym_ptr = nullptr;
            new LoadInstruction(create_operand(t), src_addr, func->getInsertPoint());
        }
        t->label.begininst = get_begininst(t->label.begininst, addrinst, nullptr);
    }
    else if (t->sym_ptr->scope == PARAM)
    {
        Operand *src = create_operand(t);
        if (isptr(t->sym_ptr->tp))
            t->sym_ptr = symbol_table::gen_temp(typesys.ptr(typesys.int_type));
        else
            t->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        Operand *dst = create_operand(t);
        Instruction *ldinst;
        ldinst = new LoadInstruction(dst, src, func->getInsertPoint());
        t->label.begininst = get_begininst(t->label.begininst, ldinst, nullptr);
    }
    else if (t->sym_ptr->scope >= GLOBAL)
    {
        Operand *src;
        Operand *dst;
        src = create_operand(t);
        bool isaddr = isarray(t->sym_ptr->tp);
        t->sym_ptr = nullptr;
        Instruction *inst;
        if (isaddr)
        {
            t->sym_ptr = symbol_table::gen_temp(typesys.ptr(typesys.int_type));
            dst = create_operand(t);
            Operand *zero = unit->getConstant(0);
            inst = new AddrInstruction(dst, src, zero, func->getInsertPoint());
        }
        else
        {
            t->sym_ptr = symbol_table::gen_temp(typesys.int_type);
            dst = create_operand(t);
            inst = new LoadInstruction(dst, src, func->getInsertPoint());
        }
        t->label.begininst = get_begininst(t->label.begininst, inst, nullptr);
    }
}

void Ast::stmt_gen_code(Node *t)
{
    Node *n1 = t->child[0];
    Node *n2 = t->child[1];
    Node *n3 = t->child[2];

    switch (t->kind_kind)
    {
    case STMT_COMPOUND:
    {
        if (n1)
        {
            recursive_gen_code(n1);
            t->label.nextlist = n1->label.nextlist;
            t->label.begininst = n1->label.begininst;
        }
        break;
    }
    case STMT_EXPR:
    {
        recursive_gen_code(n1);
        t->label.nextlist = nullptr;
        t->label.begininst = n1->label.begininst;
        break;
    }
    case STMT_WHILE:
    {
        loop.push_back(t);
        expr_gen_code(n1, true);
        if (n1->label.truelist == nullptr)
            gen_logic(n1);
        t->label.nextlist = n1->label.falselist;
        recursive_gen_code(n2);
        Instruction *inst = new BranchInstruction(n1->label.begininst, func->getInsertPoint());
        n2->label.begininst = get_begininst(n2->label.begininst, inst, nullptr);
        backPatch(n1->label.truelist, n2->label.begininst);
        backPatch(n2->label.nextlist, n1->label.begininst);
        loop.pop_back();
        t->label.begininst = n1->label.begininst;
        break;
    }
    case STMT_IF:
    {
        expr_gen_code(n1, true);
        if (n1->label.truelist == nullptr)
            gen_logic(n1);
        recursive_gen_code(n2);
        Instruction *brinst;
        brinst = new BranchInstruction(nullptr, func->getInsertPoint());
        n2->label.begininst = get_begininst(n2->label.begininst, brinst, nullptr);
        backPatch(n1->label.truelist, n2->label.begininst);
        if (n3 == nullptr)
            t->label.nextlist = merge(n1->label.falselist, n2->label.nextlist);
        else
        {
            recursive_gen_code(n3);
            if(n3->label.begininst != nullptr)
            {
                backPatch(n1->label.falselist, n3->label.begininst);
                t->label.nextlist = merge(n2->label.nextlist, n3->label.nextlist);
            }
            else
                t->label.nextlist = merge(n1->label.falselist, n2->label.nextlist);
        }
        t->label.nextlist = merge(t->label.nextlist, new std::vector<Instruction *>({brinst}));
        t->label.begininst = n1->label.begininst;
        break;
    }
    case STMT_CONTINUE:
    {
        t->label.begininst = new BranchInstruction(loop.back()->child[0]->label.begininst, func->getInsertPoint());
        break;
    }
    case STMT_BREAK:
    {
        Instruction *inst;
        inst = new BranchInstruction(nullptr, func->getInsertPoint());
        loop.back()->label.nextlist->push_back(inst);
        t->label.begininst = inst;
        break;
    }
    case STMT_RETURN:
    {
        if (n1)
        {
            Instruction *inst;
            recursive_gen_code(n1);
            inst = new RetInstruction(create_operand(n1), func->getInsertPoint());
            t->label.begininst = get_begininst(n1->label.begininst, inst, nullptr);
        }
        else
            t->label.begininst = new RetInstruction(nullptr, func->getInsertPoint());
        break;
    }
    case STMT_FUNC:
    {
        func = new Function(unit, t->sym_ptr);
        sym2func[t->sym_ptr] = func;
        BasicBlock *entry = func->getEntry();
        BasicBlock *bb = new BasicBlock(func, symbol_table::gen_label(1));
        entry->setSucc(bb);
        bb->addPred(entry);
        func->setInsertPoint(bb);
        if (n2)
        {
            Instruction *inst;
            inst = nullptr;
            if (t->sym_ptr->tp->func_params != nullptr)
            {
                int i = 0;
                while (true)
                {
                    symbol_entry *param = t->sym_ptr->tp->func_params[i];
                    if (param == nullptr)
                        break;
                    Operand *dst;
                    dst = unit->getVar(param);
                    param->label = symbol_table::gen_label(1);
                    inst = new AllocaInstruction(dst, func->getInsertPoint());
                    new StoreInstruction(dst, unit->getTemp(param), func->getInsertPoint());
                    t->label.begininst = get_begininst(t->label.begininst, inst, nullptr);
                    i++;
                }
            }
            recursive_gen_code(n2);
            t->label.begininst = get_begininst(t->label.begininst, n2->label.begininst, nullptr);
            backPatch(n2->label.nextlist, func->getExit());
            func->constructCFG();
        }
        break;
    }
    case STMT_ASSIGN:
    {
        Operand *dst, *src;
        Operand *base, *index;
        Instruction *addrinst, *storeinst;
        addrinst = nullptr;
        if (n1->kind_kind == OP_ARRAY)
        {
            Node *p = n1;
            while (p->child[0] != nullptr)
                p = p->child[0];
            int scope = p->sym_ptr->scope;
            if(scope == PARAM || scope == GLOBAL)
                func->setCritical();
            index = create_index(n1, &base);
            dst = unit->getTemp(symbol_table::gen_temp(typesys.ptr(typesys.int_type)));
            addrinst = new AddrInstruction(dst, base, index, func->getInsertPoint());
        }
        else
        {
            dst = create_operand(n1);
            if(n1->sym_ptr->scope == GLOBAL)
                func->setCritical();
        }
        recursive_gen_code(n2);
        src = create_operand(n2);
        storeinst = new StoreInstruction(dst, src, func->getInsertPoint());
        t->label.begininst = get_begininst(n1->label.begininst, addrinst, n2->label.begininst);
        t->label.begininst = get_begininst(t->label.begininst, storeinst, nullptr);
        break;
    }
    case STMT_BLANK:
        t->label.begininst = nullptr;
        break;
    default:
        break;
    }
}

int Ast::get_constExpVal(Node *t)
{
    if (t->kind_kind == OP_ARRAY)
    {
        int i = get_arrayIndex(t);
        return t->sym_ptr->array_value[i];
    }
    int t1, t2;
    t1 = t2 = 0;
    if (t->child[0])
        t1 = get_constExpVal(t->child[0]);
    if (t->child[1])
        t2 = get_constExpVal(t->child[1]);
    switch (t->kind_kind)
    {
    case OP_ADD:
        return t1 + t2;
    case OP_MINUS:
        return t1 - t2;
    case OP_MUL:
        return t1 * t2;
    case OP_DIV:
        return t1 / t2;
    case OP_MOD:
        return t1 % t2;
    case OP_UMINUS:
        return -t1;
    default:
        break;
    }

    if (t->kind_kind == -1)
    {
        if (t->sym_ptr->scope == CONSTANT)
            return t->sym_ptr->constant;
        if (isconst(t->sym_ptr->tp) && isint(t->sym_ptr->tp))
            return t->sym_ptr->constant;
    }
    return 0;
}

int Ast::get_arrayIndex(Node *t)
{
    int t1, t2;
    t1 = t2 = 0;
    if (t->child[0])
        t1 = get_arrayIndex(t->child[0]);
    if (t->child[1])
        t2 = get_constExpVal(t->child[1]);
    return t1 + t2 * t->sym_ptr->tp->size / t->sym_ptr->tp->align;
}

Operand *Ast::create_operand(Node *t)
{
    if (t->sym_ptr == nullptr)
        t->sym_ptr = symbol_table::gen_temp(typesys.int_type);
    symbol_entry *s = t->sym_ptr;
    if (s->scope == LABEL)
        return unit->getTemp(s);
    if (s->scope == CONSTANT)
        return unit->getConstant(s->constant);
    else
        return unit->getVar(s);
}

Operand *Ast::create_index(Node *t, Operand **base)
{
    if (t->child[0] == nullptr)
    {
        recursive_gen_code(t);
        *base = create_operand(t);
        return nullptr;
    }
    Operand *res = create_operand(t);
    Operand *t1, *t2;
    t1 = create_index(t->child[0], base);
    recursive_gen_code(t->child[1]);
    t2 = create_operand(t->child[1]);
    Instruction *inst;

    if (!isarray(t->sym_ptr->tp) && t1 != nullptr)
        inst = new BinaryInstruction(BinaryInstruction::ADD, res, t1, t2, func->getInsertPoint());
    else
    {
        Operand *c = unit->getConstant(t->sym_ptr->tp->size / t->sym_ptr->tp->align);
        inst = new BinaryInstruction(BinaryInstruction::MUL, res, t2, c, func->getInsertPoint());
        if (t1 != nullptr)
        {
            t->sym_ptr = nullptr;
            Operand *oprd = create_operand(t);
            new BinaryInstruction(BinaryInstruction::ADD, oprd, t1, res, func->getInsertPoint());
            res = oprd;
        }
    }
    t->label.begininst = get_begininst(t->child[0]->label.begininst, t->child[1]->label.begininst, inst);
    return res;
}

void Ast::backPatch(std::vector<Instruction *> *list, Instruction *inst)
{
    if (list == nullptr || inst == nullptr)
        return;
    for (auto i : *list)
        dynamic_cast<BranchInstruction *>(i)->setBranch(inst);
    delete list;
}

void Ast::backPatch(std::vector<Instruction *> *list, BasicBlock *bb)
{
    if (list == nullptr)
        return;
    for (auto i : *list)
        dynamic_cast<BranchInstruction *>(i)->setBranch(bb);
    delete list;
}

std::vector<Instruction *> *Ast::merge(std::vector<Instruction *> *list1, std::vector<Instruction *> *list2)
{
    if (list1 == nullptr && list2 == nullptr)
        return nullptr;
    if (list1 == nullptr)
        return list2;
    if (list2 == nullptr)
        return list1;
    auto ret = new std::vector<Instruction *>(*list1);
    ret->insert(ret->end(), list2->begin(), list2->end());
    delete list1;
    delete list2;
    return ret;
}

void Ast::set_initVal(Node *t, int offset)
{
    Node *p = t;
    while (p)
    {
        if (p->kind_kind == OP_INITLIST)
        {
            if (p->child[0])
            {
                p->child[0]->sym_ptr->tp = t->sym_ptr->tp->operand;
                p->child[0]->sym_ptr->array_value = t->sym_ptr->array_value;
                set_initVal(p->child[0], offset);
            }
            offset += (t->sym_ptr->tp->size / t->sym_ptr->tp->align);
        }
        else
        {
            t->sym_ptr->array_value[offset] = get_constExpVal(p->child[0]);
            offset += 1;
        }
        p = p->sibling;
    }
}

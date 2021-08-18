#include "DeadCodeElimination.h"
#include "unit.h"

DeadCodeElimination::DeadCodeElimination(Unit *unit) : Pass(unit)
{
}

inline void Exception(string error_code){
    bool DEBUG = true;
    if(DEBUG)
        cerr << "[DCE]: " << error_code << endl;
}

void DeadCodeElimination::pass()
{
    for (auto t = unit->begin(); t != unit->end(); t++)
    {
        allChange = true;
        func = *t;
        int tot = 0;
        while (allChange)
        {
            tot += 1;
            allChange = false;
            eliminateUselessCode();
            eliminateUnreachableCode();
            eliminateSinglePhi();
            eliminateUselessControlFlow();
            if(tot >= 3) {
                Exception("Break from dce mainloop");
                break;
            }
        }
    }
}

void DeadCodeElimination::eliminateUselessCode(){
    eliminateUselessCodeMark();
    eliminateUselessCodeSweep();
}

void DeadCodeElimination::eliminateUselessCodeMark(){
    // Worklist <- empty
    std::vector<Instruction*> Worklist;
    // for each operation i
    for(auto bb = func->begin(); bb != func->end(); bb++){
        (*bb)->clearMarked();
        if((*bb)->empty()) continue;
        for(Instruction* instr = (*bb)->begin(); instr != (*bb)->end()->getNext(); instr = instr->getNext()){
            // clear i's mark
            instr->clearMarked();
            // if i is critical then
            if(instr->iscritical()){
                // mark i
                instr->setMarked();
                instr->getParent()->setMarked();
                // Worklist <- Worklist U {i}
                Worklist.push_back(instr);
            }
        }
    }

    func->compute_reverse_dom_frontier();
    while(!Worklist.empty()){
        Instruction* instr = Worklist.back();
        // remove i from Worklist, assume i is "x <- y op z"
        Worklist.pop_back();

        std::vector<Operand*> operand_ptrs = instr->getUse();
        for(auto &operand_ptr : operand_ptrs){
            std::vector<Instruction *> OperandDefInstrs = operand_ptr->getDef();
            // if def(y), def(z) [if exists] is not marked then
            for(auto &Definstr : OperandDefInstrs){
                if(!Definstr->isMarked()){
                    // mark def(y), def(z)
                    Definstr->setMarked();
                    Definstr->getParent()->setMarked();
                    // Worklist <- Worklist U {def(y), def(z)}
                    Worklist.push_back(Definstr);
                }
            }
        }

        BasicBlock* bb = instr->getParent();
        // for each block b belongs to RDF(block(i))
        for(auto &rdfbb : bb->getRDF()){
            // let j be the branch that ends b
            Instruction* brinstr = rdfbb->end();
            // if j is unmarked then
            if(brinstr && brinstr->isBranch() && !brinstr->isMarked()){
                // mark j
                brinstr->setMarked();
                brinstr->getParent()->setMarked();
                // Worklist <- Worklist U {j}
                Worklist.push_back(brinstr);
            }
        }
    }
}

void DeadCodeElimination::eliminateUselessCodeSweep(){
    
    std::vector<Instruction*> deletelist;
    // for each operation i
    for(auto bb = func->begin(); bb != func->end(); bb++){
        if((*bb)->empty()) continue;
        for(Instruction* instr = (*bb)->begin(); instr != (*bb)->end()->getNext(); instr = instr->getNext()){
            // if i is unmarked then
            if(!instr->isMarked()){
                // if i is a branch then
                if(instr->isCondBr()){
                    // rewrite i with a jump to i's nearest marked postdominators
                    BasicBlock* nearest_pdom = func->compute_nearest_marked_postdominator(instr);
                    BranchInstruction* binstr = new BranchInstruction(nearest_pdom);
                    (*bb)->insertAfter(binstr, instr);
                    // delete pred and succ
                    BasicBlock *true_branch = dynamic_cast<IfInstruction*>(instr)->getBranch();
                    BasicBlock *false_branch = dynamic_cast<IfInstruction*>(instr)->getFalseBranch();
                    (*bb)->eraseSucc(true_branch);
                    (*bb)->eraseSucc(false_branch);
                    true_branch->erasePred((*bb));
                    false_branch->erasePred(*bb);
                    // set pred and succ
                    nearest_pdom->addPred(instr->getParent());
                    (*bb)->addSucc(nearest_pdom);
                    // delete i
                    deletelist.push_back(instr);
                    instr = binstr;
                }
                // if i is not a jump then
                if(!instr->isUncondBr()){
                    // delete i
                    deletelist.push_back(instr);
                }
            }
        }
    }

    if(deletelist.size()){
        allChange = true;
    }

    // delete marked instructions
    for(auto &instr : deletelist){
        if(instr->isRet())
        {
            Instruction *ret = new RetInstruction(nullptr);
            dynamic_cast<RetInstruction*>(ret)->setRetUndef();
            instr->insertBefore(ret);
        }
        instr->eraseFromParent();
    }
}

void DeadCodeElimination::eliminateUselessControlFlow()
{
    change = true;
    std::set<BasicBlock *> visit;
    int tot = 0;
    while (change)
    {
        tot += 1;
        change = false;
        visit.clear();
        postOrder.clear();
        computePostOrder(func->getEntry(), visit);
        for (auto &bb : postOrder)
            eliminateUselessControlFlow(bb);
        if (change)
            allChange = true;
        if (tot >= 3){
            Exception("Break from Eliminate Control flow loop");
            break;
        }
    }
}

void DeadCodeElimination::eliminateUselessControlFlow(BasicBlock *bb)
{
    if (bb->empty())
        return;
    Instruction *last = bb->end();
    if (last->isCondBr())
    {
        IfInstruction *if_inst = dynamic_cast<IfInstruction *>(last);
        if (if_inst->getBranch() == if_inst->getFalseBranch())
        {
            change = true;
            Instruction *br_inst = new BranchInstruction(if_inst->getBranch());
            if_inst->insertBefore(br_inst);
            if_inst->eraseFromParent();
        }
    }
    last = bb->end();
    if (last->isUncondBr())
    {
        BasicBlock *to = dynamic_cast<BranchInstruction *>(last)->getBranch();
        if (bb->begin() == bb->end() && *func->getEntry()->succ_begin() != bb)
        {
            bool flag;
            if (to->empty() || !to->begin()->isPhi())
                flag = true;
            else
            {
                flag = true;
                for (auto pred = bb->pred_begin(); pred != bb->pred_end(); pred++)
                    if ((*pred)->isPredOf(to))
                    {
                        flag = false;
                        break;
                    }
            }
            if (flag)
            {
                change = true;
                for (auto pred = bb->pred_begin(); pred != bb->pred_end(); pred++)
                {
                    if ((*pred)->end()->isUncondBr())
                        dynamic_cast<BranchInstruction *>((*pred)->end())->setBranch(to);
                    else
                    {
                        IfInstruction *if_inst = dynamic_cast<IfInstruction *>((*pred)->end());
                        if (if_inst->getBranch() == bb)
                            if_inst->setBranch(to);
                        if (if_inst->getFalseBranch() == bb)
                            if_inst->setFalseBranch(to);
                    }
                    if(!to->empty())
                    {
                        for (auto phi_inst = to->begin(); phi_inst != to->end()->getNext() && phi_inst->isPhi(); phi_inst = phi_inst->getNext())
                        {
                            PhiInstruction *phi = dynamic_cast<PhiInstruction *>(phi_inst);
                            Operand *src = phi->getSrc(bb);
                            phi->addSrc(src, *pred);
                        }
                    }
                    (*pred)->addSucc(to);
                    to->addPred(*pred);
                }
                bb->eraseFromParent();
            }
        }
    }
}

void DeadCodeElimination::eliminateUnreachableCode()
{
    for (auto bb = func->begin(); bb != func->end(); bb++)
    {
        if (!(*bb)->empty() && (*bb)->end()->isCondBr())
        {
            IfInstruction *if_inst = dynamic_cast<IfInstruction *>((*bb)->end());
            Operand *res = if_inst->getUse()[0];
            if (res->isConstant())
            {
                allChange = true;
                int value = dynamic_cast<Constant *>(res)->getValue();
                Instruction *brinst;
                BasicBlock *true_branch, *false_branch;
                true_branch = if_inst->getBranch();
                false_branch = if_inst->getFalseBranch();
                if (value != 0)
                {
                    brinst = new BranchInstruction(true_branch);
                    (*bb)->eraseSucc(false_branch);
                    false_branch->erasePred(*bb);
                }
                else
                {
                    brinst = new BranchInstruction(false_branch);
                    (*bb)->eraseSucc(true_branch);
                    true_branch->erasePred(*bb);
                }
                if_inst->insertBefore(brinst);
                if_inst->eraseFromParent();
            }
        }
    }

    std::vector<BasicBlock *> worklist;
    std::set<BasicBlock *> visited;
    worklist.push_back(func->getEntry());
    while (!worklist.empty())
    {
        BasicBlock *bb = worklist.back();
        worklist.pop_back();
        visited.insert(bb);
        for (auto s = bb->succ_begin(); s != bb->succ_end(); s++)
        {
            if (visited.find(*s) == visited.end())
                worklist.push_back(*s);
        }
    }
    for (auto bb = func->begin(); bb != func->end(); bb++)
        if (visited.find(*bb) == visited.end())
        {
            worklist.push_back(*bb);
            allChange = true;
        }
    for (auto &bb : worklist)
        bb->eraseFromParent();
}

void DeadCodeElimination::eliminateSinglePhi()
{
    std::vector<Instruction *> delete_list;
    for (auto bb = func->begin(); bb != func->end(); bb++)
    {
        if ((*bb)->empty())
            continue;
        for (auto inst = (*bb)->begin(); inst != (*bb)->end()->getNext() && inst->isPhi(); inst = inst->getNext())
        {
            std::vector<Operand *> use = inst->getUse();
            if (use.size() > 1)
                continue;
            allChange = true;
            delete_list.push_back(inst);
            if(use.size() > 0)
            {
                Operand *phi_def = inst->getDef();
                std::vector<Instruction *> phi_use = phi_def->getUse();
                Operand *u = use[0];
                for (auto &t : phi_use)
                    t->replaceUse(phi_def, u);
            }
        }
    }
    for (auto &t : delete_list)
        t->eraseFromParent();
}

void DeadCodeElimination::computePostOrder(BasicBlock *bb, std::set<BasicBlock *> &visit)
{
    visit.insert(bb);
    for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        if (visit.find(*succ) == visit.end())
            computePostOrder(*succ, visit);
    postOrder.push_back(bb);
}

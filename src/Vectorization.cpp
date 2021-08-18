#include "Vectorization.h"
#include "symtab.h"
#include <unordered_map>
#include <queue>
Vectorization::Vectorization(Unit* unit) 
{
    this->unit = unit;
}

void Vectorization::pass() 
{
    func = nullptr;
    LoopAnalyzer* analyzer = new LoopAnalyzer();
    for(auto iter = unit->begin(); iter != unit->end(); iter++)
    {
        func = *iter;
        analyzer->FindLoops(func);
        loops = analyzer->getLoops(); 
        for(auto l = loops.begin(); l != loops.end(); l++)
        {
            vectorizable_loop(*l);
        }
    }
}

void Vectorization::vectorizable_loop(Loop* loop) 
{
    if(loop->getbbs().size() != 2) return;
    // 1. find cond and body
    BasicBlock* cond = nullptr, *body = nullptr;
    for(auto bb : loop->getbbs()){
        for(auto instr = bb->begin(); instr != bb->end()->getNext(); instr = instr->getNext()){
            if(instr->isCmp()){
                cond = bb;
                break;
            }
        }
        if(cond) break;
    }

    for(auto bb : loop->getbbs()){
        if(bb != cond){
            body = bb;
        }
    }

    if(body->getNumOfSucc() != 1){
        return;
    }
    
    // 2. whether it is a simple assign loop
    std::unordered_map<Instruction*, bool> marked_map;
    // find cmp inst
    for(auto inst = cond->begin(); inst != cond->end()->getNext(); inst = inst->getNext())
    {
        if(inst->isCmp())
        {
            switch(inst->getOP()){
                case CmpInstruction::L:
                case CmpInstruction::LE:
                case CmpInstruction::G:
                case CmpInstruction::GE:
                    break;
                default:
                    return;
            }
        }
    }

    // find stote inst
    Instruction* store_inst = nullptr;
    for(auto inst = body->begin(); inst != body->end()->getNext(); inst = inst->getNext())
    {
        if(inst->isStore())
        {
            if(store_inst) return;
            store_inst = inst;
        }
    }
    if(!store_inst) return;
    // make sure that the store val is const or array element
    if(!store_inst->getUse()[0]->isConstant()
    && !store_inst->getUse()[0]->getDef()[0]->isLoad())
    {
        return;
    }

    marked_map[store_inst] = true;

    // find iter inst
    if(!store_inst->getNext()) return;
    Instruction* iter_inst = store_inst->getNext();
    if(!iter_inst->isBinary()
    || iter_inst->getOP() != BinaryInstruction::ADD
    || !iter_inst->getUse()[1]->isConstant()
    || dynamic_cast<Constant*>(iter_inst->getUse()[1])->getValue() != 1)
    {
        return;
    }
    marked_map[iter_inst] = true;

    // find copy inst
    if(!iter_inst->getNext()) return;
    Instruction* copy_inst = iter_inst->getNext();
    Instruction* pre_inst = iter_inst;
    while(copy_inst->isCopy())
    {
        if(copy_inst->getUse()[0]->getName() != pre_inst->getDef()->getName())
        {
            return;
        }
        marked_map[copy_inst] = true;
        pre_inst = copy_inst;
        copy_inst = copy_inst->getNext();
    }

    // find branch inst at the end
    Instruction* branch_inst = copy_inst;
    if(!branch_inst->isUncondBr()
    || dynamic_cast<BranchInstruction*>(branch_inst)->getBranch() != cond)
    {
        return;
    }
    marked_map[branch_inst] = true;

    // start to mark used inst
    std::queue<Operand*> used_ope;
    auto opes = store_inst->getUse();
    for(auto &o: opes)
    {
        used_ope.push(o);
    }
    while (!used_ope.empty())
    {
        Operand* ope = used_ope.front();
        used_ope.pop();
        if(ope->isConstant()) continue;
        if(ope->getDef().empty()) continue;
        Instruction* p = ope->getDef()[0];
        if(p->getParent()->getNo() != body->getNo()) continue;
        if(!p->isBinary() && !p->isAddr() && !p->isLoad() && !p->isCopy()) 
            return;
        // if(p->isCall()) return;
        auto opes = p->getUse();
        if(!marked_map[p])
        {
            for(auto &o: opes)
            {
                used_ope.push(o);
            }
        }
        marked_map[p] = true;
    }
    
    // check if all insts have been marked already 
    for(auto inst = body->begin(); inst != body->end()->getNext(); inst = inst->getNext())
    {
        if(!marked_map[inst])
            return;
    }
    

    // 3. insert new cond body to deal with the situation that iter_num % 4 != 0
    // cal begin end stride
    // *****copy from LoopUnrolling.cpp*****
    Operand* end_ope = nullptr;
    int end = -1;
    bool IsEndCons = true;
    // auto PhiInst = dynamic_cast<PhiInstruction*> (cond->begin());
    // if(!PhiInst){
    //     return;
    // }
    // Operand* PhiOperand = nullptr;
    // for(auto it = PhiInst->getSrcBegin(); it != PhiInst->getSrcEnd(); it++){
    //     if(it->first != body){
    //         PhiOperand = it->second;
    //         break;
    //     }
    // }
    // if(!PhiOperand) return;
    // if(PhiOperand && !PhiOperand->isConstant()){
    //     IsBeginCons = false;
    //     return;
    // }
    //
    // Constant* PhiConsOperand = dynamic_cast<Constant*>(PhiOperand);
    // if(IsBeginCons)
    // {
    //     begin = PhiConsOperand->getValue();
    //     if(begin != 0) return;
    // }

    Instruction* CondBranchInstr = cond->end();
    if(!CondBranchInstr->isCondBr()){
        return;
    }
    auto CondCmpOperandDefVec = dynamic_cast<IfInstruction*>(CondBranchInstr)->getUse()[0]->getDef();
    if(CondCmpOperandDefVec.size() != 1){
        return;
    }
    Instruction* CondCmpInstr = CondCmpOperandDefVec[0];
    if(!CondCmpInstr->isCmp()){
        return;
    }
    
    // Operand* CondCmpOperand = dynamic_cast<CmpInstruction*>(CondCmpInstr)->getUse()[0];
    Operand* CondCmpConOperand = dynamic_cast<CmpInstruction*>(CondCmpInstr)->getUse()[1];
    if(!CondCmpConOperand->isConstant()){
        IsEndCons = false;
    }
    if(IsEndCons)
        end = dynamic_cast<Constant*>(CondCmpConOperand)->getValue();
    else if(CondCmpConOperand->isVar())
        end_ope = dynamic_cast<Variable*>(CondCmpConOperand);
    else
        end_ope = dynamic_cast<Temporary*>(CondCmpConOperand);
    // *****copy from LoopUnrolling.cpp*****
    
    if(!IsEndCons || end % 4 != 0)
    {
        // copy bb
        BasicBlock* copy_body = new BasicBlock(func, symbol_table::gen_label(1));
        BasicBlock* copy_cond = new BasicBlock(func, symbol_table::gen_label(1));
        copy_basicblock(copy_cond, cond);
        copy_basicblock(copy_body, body);
        auto iter = cond->succ_begin();
        while(*iter == body)
            iter++;
        auto exit_bb = *iter;

        dynamic_cast<IfInstruction*>(cond->end())->setFalseBranch(copy_cond);
        dynamic_cast<IfInstruction*>(copy_cond->end())->setBranch(copy_body);
        dynamic_cast<IfInstruction*>(copy_cond->end())->setFalseBranch(exit_bb);
        dynamic_cast<BranchInstruction*>(copy_body->end())->setBranch(copy_cond);

        copy_cond->setPred(cond);
        copy_cond->addPred(copy_body);
        copy_cond->setSucc(copy_body);
        copy_cond->addSucc(exit_bb);
        copy_body->setPred(copy_cond);
        copy_body->setSucc(copy_cond);
        body->setPred(cond);
        body->setSucc(cond);
        cond->setSucc(body);
        cond->addSucc(copy_cond);
        exit_bb->setPred(copy_cond);
        func->insertBlock(copy_cond);
        func->insertBlock(copy_body); 
        
        // insert i < n - n % 4 inst
        
        if(IsEndCons)
        {
            Constant* end_imm = new Constant(end % 4);
            CondCmpInstr->replaceUse(CondCmpInstr->getUse()[1], end_imm);
        }
        else
        {
            type* tp = new type();
            tp->op = INT_T;
            tp->size = tp->align = 4;
            Operand* tmp1 = new Temporary(symbol_table::gen_temp(tp));
            Operand* tmp2 = new Temporary(symbol_table::gen_temp(tp));
            Operand* imm_ope = new Constant(7);
            Instruction* and_inst = new BinaryInstruction(BinaryInstruction::AND, tmp1, end_ope, imm_ope);
            Instruction* sub_inst = new BinaryInstruction(BinaryInstruction::MINUS, tmp2, end_ope, tmp1);
            cond->insertBefore(and_inst, CondCmpInstr);
            cond->insertAfter(sub_inst, and_inst);   
            CondCmpInstr->replaceUse(CondCmpInstr->getUse()[1], tmp2);
        }

    }
    // mark vec inst
    store_inst->set_vec_flag(true);
    if(!store_inst->getUse()[0]->isConstant())
        store_inst->getUse()[0]->getDef()[0]->set_vec_flag(true);
    // modify stride
    Constant* imm = new Constant(8);
    iter_inst->replaceUse(iter_inst->getUse()[1], imm);
    cerr << "vec successful" << endl;
    return;
}

void Vectorization::copy_basicblock(BasicBlock* copy_bb, BasicBlock* bb) 
{
    Instruction* copy_begin = bb->begin()->copy();
    copy_begin->setParent(copy_bb);
    func->insertInst(copy_begin);
    for(auto inst = bb->begin()->getNext(); inst != bb->end()->getNext(); inst = inst->getNext())
    {
        Instruction *copy_inst = inst->copy();
        copy_inst->setParent(copy_bb);
        func->insertInst(inst->copy());
    }
    copy_bb->setInsts(copy_begin, func->instEnd()->getPrev());
}

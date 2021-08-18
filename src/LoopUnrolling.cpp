#include "LoopUnrolling.h"
#include "unit.h"

#define DEBUG false

inline void Exception(string s){
    // just for debug
    if(DEBUG)
        cerr << "[IN LOOPUNROLLING]: " << s << endl; 
}

LoopUnrolling::LoopUnrolling(Unit *unit) : Pass(unit){
    this->UnrollingFactor = 2;
}

void LoopUnrolling::pass(){
    for (auto t = unit->begin(); t != unit->end(); t++)
    {
        func = *t;
        
        FindLoops();
        PrepareWorklist();
    }
}

bool LoopUnrolling::isSubset(std::set<BasicBlock*> t_son, std::set<BasicBlock*> t_fat){
    /*
    *  [ATTENTION] Here regulate that set i is not father of itself.
    */

    for(auto EleInt_son : t_son){
        if(t_fat.find(EleInt_son) == t_fat.end()){
            return false;
        }
    }
    return t_son.size() != t_fat.size();
}

void LoopUnrolling::FindLoops(){
    cfaolun.pass(func);

    // [Step 1]: calc loop depth
    for(auto loop : cfaolun.getLoops()){
        loop->setLoopDepth(0x7fffffff);
        for(auto bb : loop->getbbs()){
            loop->setLoopDepth(min(loop->getLoopDepth(), cfaolun.getLoopDepth(bb)));
        }
    }

    /* [Step 2]: calc innerloop ( if i is innerloop i.e. i is not anyone's fath)
    *  Step 2.1: Mark all the loop isinnerloop;
    *  Step 2.2: Clear mark if any loop i is father of any loop j;
    */

    for(auto loop : cfaolun.getLoops()){
        loop->setIsInnerLoop();
    }

    for(auto loopi : cfaolun.getLoops()){
        for(auto loopj : cfaolun.getLoops()){
            if(isSubset(loopi->getbbs(), loopj->getbbs())){
                loopj->clearIsInnerLoop();
            }
        }
    }
}

void LoopUnrolling::PrepareWorklist(){
    std::vector<loopptr*> Worklist;
    for(auto loop : cfaolun.getLoops()){
        if(!loop->isInnerLoop()){
            Exception("Candidate Loop shall be innerloop");
            continue;
        }
        if(loop->getbbs().size() != 2){
            Exception("Candidate Loop shall be cond and body");
            continue;
        }

        // find cond and body
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
            Exception("Succ shall have only 1 succ");
            continue;
        }

        loopptr* CandidateLoop = new loopptr(loop);
        CandidateLoop->setCond(cond);
        CandidateLoop->setBody(body);
        Worklist.push_back(CandidateLoop);
    }

    for(auto CandidateLoop : Worklist){
        bool HasCallInBody = false;
        for(auto bodyinstr = CandidateLoop->getBody()->begin(); bodyinstr != CandidateLoop->getBody()->end()->getNext(); bodyinstr = bodyinstr->getNext()){
            if(bodyinstr->isCall()){
                HasCallInBody = true;
                break;
            }
        }
        if(HasCallInBody){
            Exception("Candidate loop shall have no call in body");
            continue;
        }

        bool CheckFlag = false;
        for(auto bb = CandidateLoop->getCond()->succ_begin(); bb != CandidateLoop->getCond()->succ_end(); bb++){
            CheckFlag = CheckFlag || (*bb == CandidateLoop->getBody());
        }
        if(!CheckFlag){
            Exception("One of succ of cond of Candidate Loop shall be body");
            continue;
        }

        WorkToUnrolling(CandidateLoop);
    }
}

Operand* LoopUnrolling::mycopy(Operand* src){
    // [TODO] maybe should handle "prev" in symptr
    if(dynamic_cast<Temporary*>(src)){
        symbol_entry* OldSymPtr = dynamic_cast<Temporary*>(src)->getSymPtr();
        symbol_entry* NewSymPtr = new symbol_entry();
        NewSymPtr->scope = OldSymPtr->scope;
        NewSymPtr->lineno = OldSymPtr->lineno;
        NewSymPtr->tp = new type(*(OldSymPtr->tp));
        NewSymPtr->str = OldSymPtr->str;
        NewSymPtr->undefined = OldSymPtr->undefined;
        NewSymPtr->label = symbol_table::gen_label(1);
        NewSymPtr->constant = OldSymPtr->constant;
        return new Temporary(NewSymPtr);
    }
    else if(dynamic_cast<Variable*>(src)){
        symbol_entry* OldSymPtr = dynamic_cast<Variable*>(src)->getSymPtr();
        symbol_entry* NewSymPtr = new symbol_entry();
        NewSymPtr->name = OldSymPtr->name;
        NewSymPtr->scope = OldSymPtr->scope;
        NewSymPtr->lineno = OldSymPtr->lineno;
        NewSymPtr->tp = new type(*(OldSymPtr->tp));
        NewSymPtr->str = OldSymPtr->str;
        NewSymPtr->undefined = OldSymPtr->undefined;
        NewSymPtr->label = OldSymPtr->label + 1;
        NewSymPtr->constant = OldSymPtr->constant;
        return new Variable(NewSymPtr); 
    }
    
    Exception("[SERIOUS] ERROR IN MYCOPY");
    return nullptr;
}

bool LoopUnrolling::SuitableForSpecialLun(int begin, int end, int stride, int InsNumOfbb){
    return ceil((end - begin) * 1.0 / stride) <= MAX_FOLDER
        && ceil((end - begin) * 1.0 / stride) * InsNumOfbb <= MAX_INSTRUCTIONS;
}

Operand* findnewop(std::map<Operand*, Operand*>& op2op, std::set<Operand*>& ops_phi, Operand* src){
    Operand* dst = src;
    std::map<Operand*, Operand*>::iterator it;
    while(op2op.find(dst) != op2op.end()){
        it = op2op.find(dst);
        dst = it->second;
    }

    if(ops_phi.find(src) != ops_phi.end()){
        return it->first;
    }
    else return dst;
}

void LoopUnrolling::WorkToUnrolling(loopptr* LoopPtr){
    /*
    * [Step 1] Calc begin, end, stride. They shall all be constant in Special LoopUnrolling.
    * [Step 2] Choose Normal LoopUnrolling or Special LoopUnrolling.
    * [Step 2.1] IF begin, end and stride are all constant, try Special LoopUnrolling.
    * [Step 2.2] IF stride is constant, while begin or end is not constant, try Normal LoopUnrolling.
    * 
    *                     --------
    *                     ↓      ↑
    * Special:  pred -> cond -> body  Exitbb      ==>     pred -> newbody -> Exitbb
    *                     ↓             ↑
    *                     ---------------
    * 
    * 
    * 
    *                     --------                                    ------------        -----------
    *                     ↓      ↑                                    ↓          ↑        ↓         ↑
    * Normal:  pred ->  cond -> body  Exitbb      ==>     pred -> maincond -> mainbody rescond -> resbody Exitbb
    *                     ↓             ↑                             ↓                  ↑   ↓              ↑
    *                     ---------------                             --------------------   ----------------
    * 
    */

    // [Step 1] Calc begin, end, stride.
    int begin = -1, end = -1, stride = -1;
    bool IsBeginCons, IsEndCons, IsStrideCons;
    IsBeginCons = IsEndCons = IsStrideCons = true;
    BasicBlock* cond = LoopPtr->getCond(), *body = LoopPtr->getBody();

    auto PhiInst = dynamic_cast<PhiInstruction*> (cond->begin());
    if(!PhiInst){
        Exception("Phi instr shall be the first instr of cond");
        return;
    }

    Operand* PhiOperand = nullptr;
    for(auto it = PhiInst->getSrcBegin(); it != PhiInst->getSrcEnd(); it++){
        if(it->first != body){
            PhiOperand = it->second;
            break;
        }
    }
    if(!PhiOperand) return;
    if(PhiOperand && !PhiOperand->isConstant()){
        Exception("Phi instr operand shall be constant");
        IsBeginCons = false;
    }

    Constant* PhiConsOperand = dynamic_cast<Constant*>(PhiOperand);
    if(IsBeginCons)
        begin = PhiConsOperand->getValue();

    Instruction* CondBranchInstr = cond->end();
    if(!CondBranchInstr->isCondBr()){
        Exception("Cond's last instr shall be Condbranch");
        return;
    }
    
    auto CondCmpOperandDefVec = dynamic_cast<IfInstruction*>(CondBranchInstr)->getUse()[0]->getDef();
    if(CondCmpOperandDefVec.size() != 1){
        Exception("[SERIOUS] Cond's def shall be only 1 !?");
        return;
    }
    Instruction* CondCmpInstr = CondCmpOperandDefVec[0];
    if(!CondCmpInstr->isCmp()){
        Exception("This pass can only handle cmp");
        return;
    }
    
    Operand* CondCmpOperand = dynamic_cast<CmpInstruction*>(CondCmpInstr)->getUse()[0];
    Operand* CondCmpConOperand = dynamic_cast<CmpInstruction*>(CondCmpInstr)->getUse()[1];
    if(!CondCmpConOperand->isConstant()){
        Exception("Cond boundary shall be constant");
        IsEndCons = false;
    }
    
    if(IsEndCons)
        end = dynamic_cast<Constant*>(CondCmpConOperand)->getValue();

    bool NeedReverse = false;
    switch(CondCmpInstr->getOP()){
        case CmpInstruction::L:{
            break;
        }
        case CmpInstruction::LE:{
            end = end + 1;
            break;
        }
        case CmpInstruction::G:{
            NeedReverse = true;
            break;
        }
        case CmpInstruction::GE:{
            NeedReverse = true;
            end = end + 1;
            break;
        }
        default:{
            Exception("This CmpInstruction Opcode don't support..");
            return;
        }
    }
    if(NeedReverse){
        swap(begin, end);
    }

    Instruction* DefCondCmpOperInstr = CondCmpOperand->getDef()[0];
    PhiInstruction* DefCondCmpOperPhiInstr = dynamic_cast<PhiInstruction*>(DefCondCmpOperInstr);
    if(!DefCondCmpOperPhiInstr){
        Exception("DefCondOperPhiInstr not exist");
        return;
    }
    
    Operand* IterOperand = nullptr;
    for(auto it = DefCondCmpOperPhiInstr->getSrcBegin(); it != DefCondCmpOperPhiInstr->getSrcEnd(); it++){
        if(it->second->isConstant()){
            continue;
        }
        else{
            IterOperand = it->second;
        }
    }

    if(!IterOperand){
        Exception("Non-const value in cond-phi not exist");
        return;
    }

    Instruction* DefCondCmpOperandInstr = (IterOperand->getDef()[0]->getUse()[0])->getDef().size() > 0 ? (IterOperand->getDef())[0]->getUse()[0]->getDef()[0] : nullptr;
    if(!DefCondCmpOperandInstr) return;
    if(!DefCondCmpOperandInstr->isBinary()){
        Exception("iter instr shall be binary");
        return;
    }

    auto IterOperands = dynamic_cast<BinaryInstruction*>(DefCondCmpOperandInstr)->getUse();
    int counter = 0;
    Operand* IterConsOperand = nullptr;
    for(auto IterOperand : IterOperands){
        if(IterOperand != CondCmpOperand){
            counter += 1;
            IterConsOperand = IterOperand;
        }
    }
    if(counter != 1){
        Exception("[SERIOUS] iter operand shall have itself");
        return;
    }

    if(!IterConsOperand->isConstant()){
        Exception("iter stride shall be constant");
        IsStrideCons = false;
    }

    if(IsStrideCons)
        stride = dynamic_cast<Constant*>(IterConsOperand)->getValue();

    Exception("Check finish, begin unrolling..!");
    if(DEBUG)
        cerr << "begin: " << begin << ", end: " << end << ", stride: " << stride << endl;
    
    if(SuitableForSpecialLun(begin, end, stride, body->getNumOfInst()) && IsBeginCons && IsEndCons && IsStrideCons){
        /*
        *  Special LoopUnrolling:
        *  Unrolling it up.
        *  This branch will copy instructions in the body as the whole time.
        */

        std::set<Instruction*> IgnoreInstrs{PhiInst, CondBranchInstr, CondCmpInstr};
        BasicBlock* Exitbb = nullptr;
        for(auto bb = cond->succ_begin(); bb != cond->succ_end(); bb++){
            if(*bb != body){
                Exitbb = *bb;
            }
        }
        if(!Exitbb) return;

        BasicBlock* Newbb;
        Newbb = new BasicBlock(cond->getParent(), symbol_table::gen_label(1));

        std::map<Operand*, Operand*> op2op;
        std::map<Operand*, Operand*> op2op_phi;
        std::set<Operand*> ops_phi;
        // init op2op from cond
        for(auto CondInstr = cond->begin(); CondInstr != cond->end()->getNext(); CondInstr = CondInstr->getNext()){
            if(CondInstr->isPhi()){
                Instruction* NewInstr = CondInstr->copy(Newbb);
                PhiInstruction* NewPhiInstr = dynamic_cast<PhiInstruction*>(NewInstr);
                Operand* AnotherBranch = NewPhiInstr->getSrc(body);
                NewPhiInstr->removeSrc(body);

                // init ops_phi
                ops_phi.insert(NewPhiInstr->getDef());
                // init op2op_phi
                Operand* AimOp = NewPhiInstr->getDef();
                op2op_phi[AimOp] = AnotherBranch;
            }
            else if(IgnoreInstrs.find(CondInstr) != IgnoreInstrs.end()){
                continue;
            }
            else{
                Exception("[WARNING!!] UNEXPECTED BEHAVIOR");
                Instruction* NewInstr = CondInstr->copy(Newbb);
                dynamic_cast<PhiInstruction*>(NewInstr)->removeSrc(cond);
            }
        }

        // init op2op from body
        for(auto BodyInstr = body->begin(); BodyInstr != body->end()->getNext(); BodyInstr = BodyInstr->getNext()){
            if(BodyInstr->isBranch()){
                continue;
            }
            
            Operand* OperDef = BodyInstr->getDef();
            if(OperDef){
                Operand* NewOp = this->mycopy(OperDef);
                op2op[OperDef] = NewOp;
            }
        }

        int left = begin, right = end;
        while(left < right){
            std::map<Operand*, Operand*> op2op_tmp;
            for(auto BodyInstr = body->begin(); BodyInstr != body->end()->getNext(); BodyInstr = BodyInstr->getNext()){
                if(BodyInstr->isBranch()){
                    continue;
                }

                if(BodyInstr == DefCondCmpOperandInstr){
                    left += stride;
                }

                Instruction* NewInstr = BodyInstr->copy(Newbb);

                // Modify Use Operands
                int NumOfUse = NewInstr->getUse().size();
                for(int i = 0; i < NumOfUse; i++){
                    int eps = NewInstr->getDef() ? 1 : 0;
                    Operand* OpUse = NewInstr->getOperandByIndex(i + eps);
                    if(op2op.find(OpUse) != op2op.end()){
                        Operand* NewOpUse = findnewop(op2op, ops_phi, OpUse);
                        NewInstr->setOperandByIndex(i + eps, NewOpUse);
                    }
                }

                // Modify Def Operands, if exists
                Operand* OpDef = NewInstr->getDef();
                if(OpDef){
                    Operand* NewOpDef = findnewop(op2op, ops_phi, OpDef);
                    NewInstr->setOperandByIndex(0, NewOpDef);

                    Operand* NewOp = this->mycopy(NewOpDef);
                    op2op_tmp[NewOpDef] = NewOp;
                }

                // InsertBack is necessary, cuz set parent when copy
                // Newbb->insertBack(NewInstr);
            }

            for(auto it = op2op_tmp.begin(); it != op2op_tmp.end(); it++){
                op2op[it->first] = it->second;
            }

            if(left == begin + stride){
                for(auto it = op2op_phi.begin(); it != op2op_phi.end(); it++){
                    op2op[it->first] = it->second;
                }
            }
        }

        BranchInstruction* Newbr = new BranchInstruction(Exitbb);
        Newbb->insert(Newbr);

        // replace operands replaced before that used in other basicblocks
        for(auto CondInstr = cond->begin(); CondInstr != cond->end()->getNext(); CondInstr = CondInstr->getNext()){
            if(CondInstr->isBranch()) continue;

            Operand* OperDef = CondInstr->getDef();
            if(OperDef){
                for(auto UseInstr : OperDef->getUse()){
                    std::set<BasicBlock*> IgnoreBBs {cond, body, Newbb};
                    if(IgnoreBBs.find(UseInstr->getParent()) == IgnoreBBs.end()){
                        int NumOfUse = UseInstr->getUse().size();
                        int eps = UseInstr->getDef() ? 1 : 0;
                        for(int i = 0; i < NumOfUse; i++){
                            if(UseInstr->getOperandByIndex(eps + i) == OperDef){
                                Operand* NewOpUse = findnewop(op2op, ops_phi, OperDef);
                                UseInstr->setOperandByIndex(eps + i, NewOpUse);
                            }
                        }
                    }
                    else continue;
                }                
            }
        }
        for(auto BodyInstr = body->begin(); BodyInstr != body->end()->getNext(); BodyInstr = BodyInstr->getNext()){
            if(BodyInstr->isBranch()) continue;

            Operand* OperDef = BodyInstr->getDef();
            if(OperDef){
                for(auto UseInstr : OperDef->getUse()){
                    std::set<BasicBlock*> IgnoreBBs {cond, body, Newbb};
                    if(IgnoreBBs.find(UseInstr->getParent()) == IgnoreBBs.end()){
                        int NumOfUse = UseInstr->getUse().size();
                        int eps = UseInstr->getDef() ? 1 : 0;
                        for(int i = 0; i < NumOfUse; i++){
                            if(UseInstr->getOperandByIndex(eps + i) == OperDef){
                                Operand* NewOpUse = findnewop(op2op, ops_phi, OperDef);
                                UseInstr->setOperandByIndex(eps + i, NewOpUse);
                            }
                        }
                    }
                    else continue;
                }
            }
        }

        // reconstruct phi src in exitbb
        for(auto ExitbbIns = Exitbb->begin(); ExitbbIns != Exitbb->end()->getNext() && ExitbbIns->isPhi(); ExitbbIns = ExitbbIns->getNext()){
            PhiInstruction* PhiIns = dynamic_cast<PhiInstruction*>(ExitbbIns);
            if(PhiIns->getSrc(cond)){
                Operand* PhiInsUse = PhiIns->getSrc(cond);
                PhiInsUse = findnewop(op2op, ops_phi, PhiInsUse);
                PhiIns->addSrc(PhiInsUse, Newbb);
            }
        }

        // remove old pred & succ relations
        Exitbb->erasePred(cond);
        BasicBlock* CondPred = nullptr;
        for(auto bb = cond->pred_begin(); bb != cond->pred_end(); bb++){
            if(*bb != body){
                CondPred = *bb;
                (*bb)->eraseSucc(cond);
            }
        }

        // add new pred & succ relations
        if(CondPred){
            Instruction* CondPredBr = CondPred->end();
            if(CondPredBr->isBranch()){
                if(CondPredBr->isUncondBr()){
                    dynamic_cast<BranchInstruction*>(CondPredBr)->setBranch(Newbb);
                }
                else{
                    if(dynamic_cast<IfInstruction*>(CondPredBr)->getBranch() == cond){
                        dynamic_cast<IfInstruction*>(CondPredBr)->setBranch(Newbb);
                    }
                    if(dynamic_cast<IfInstruction*>(CondPredBr)->getFalseBranch() == cond){
                        dynamic_cast<IfInstruction*>(CondPredBr)->setFalseBranch(Newbb);
                    }
                }
            }
            CondPred->addSucc(Newbb);
            Newbb->addPred(CondPred);
        }
        else{
            Exception("No Available pred..?");
            return;
        }
        if(Exitbb){
            Newbb->addSucc(Exitbb);
            Exitbb->addPred(Newbb);
        }

        // delete old relations between bbs and func
        func->erase(cond);
        func->erase(body);

        // add new relations between bbs and func is not necessary, cuz we make it when we new the bb
        Exception("Special Unrolling Succeed..");
    }
    else if(IsStrideCons){
        /*
        *  Normal LoopUnrolling:
        *  UnrollingFactor is 2 as default.
        *  This branch will copy instructions in the body as <UnrollingFactor> times.
        * 
        *  Remember: When you wanna replace an operand, FIRST REMOVE USE(or DEF), second replace operand!!!
        */
        return;
        int NewStride = stride * this->getUnrollingFactor();

        // find Exitbb and CondPred
        BasicBlock* Exitbb = nullptr;
        for(auto bb = cond->succ_begin(); bb != cond->succ_end(); bb++){
            if(*bb != body){
                Exitbb = *bb;
            }
        }
        
        BasicBlock* CondPred = nullptr;
        for(auto bb = cond->pred_begin(); bb != cond->pred_end(); bb++){
            if(*bb != body){
                CondPred = *bb;
            }
        }

        // modify bb from origin cond to main cond
        {
            // replace stride with (newStride)
            if(!IterConsOperand) return;
            dynamic_cast<Constant*>(IterConsOperand)->setValue(NewStride);
            Instruction* DefEndIns = CondCmpConOperand->getDef().size() == 1 ? CondCmpConOperand->getDef()[0] : nullptr;
            if(DefEndIns){
                Exception("[Serious Bug] there are multi instr define end?");
            }
            // replace end with (end - stride * UnRollingFactor)
            Constant* MainCondEps = new Constant(NewStride);
            Operand* MainCondNewEnd = this->mycopy(CondCmpConOperand);
            BinaryInstruction* NewEndDefInst = new BinaryInstruction(BinaryInstruction::MINUS, MainCondNewEnd, CondCmpConOperand, MainCondEps);
            CondPred->insertAfter(NewEndDefInst, DefEndIns);

            if(!CondCmpInstr) return;
            int eps = CondCmpInstr->getDef() ? 1 : 0, round = CondCmpInstr->getUse().size();
            for(int i = 0; i < round; i++){
                if(CondCmpInstr->getOperandByIndex(i + eps) == CondCmpConOperand){
                    CondCmpInstr->setOperandByIndex(i + eps, MainCondNewEnd);
                }
            }
        }

        // modify bb from origin body to main body
        {
            BranchInstruction* MainBodyBrInst = dynamic_cast<BranchInstruction*>(body->end());
            if(!MainBodyBrInst) { Exception("Body's last instr shall be branch"); return;}
            // remove MainBodyBrInst temporary, will add it later
            body->remove(MainBodyBrInst);

            // copy body instructions UnrollingFactor times
            std::map<Operand*, Operand*> op2op;
            std::vector<Instruction*> Worklist;
            std::vector<Instruction*> Worklist_next;
            for(auto BodyIns = body->begin(); BodyIns != body->end()->getNext(); BodyIns = BodyIns->getNext()){
                if(BodyIns->isBranch()) continue;
                Worklist.push_back(BodyIns);
            }
            for(int factor = 0; factor < this->getUnrollingFactor(); factor++){
                for(auto WorkInstr : Worklist){
                    Instruction* copy_ins = WorkInstr->copy(body);
                    // replace uses
                    int eps = copy_ins->getDef() ? 1 : 0, round = copy_ins->getUse().size();
                    for(int i = 0; i < round; i++){
                        Operand* oper_now = copy_ins->getOperandByIndex(i + eps);
                        if(op2op.find(oper_now) != op2op.end()){
                            Operand* NewOp = op2op.find(oper_now)->second;
                            copy_ins->setOperandByIndex(i + eps, NewOp);
                        }
                    }
                    // replace def
                    if(eps){
                        Operand* NewOp = this->mycopy(copy_ins->getDef());
                        op2op.insert({copy_ins->getDef(), NewOp});

                        copy_ins->setOperandByIndex(0, NewOp);
                    }
                    Worklist_next.push_back(copy_ins);
                }
                Worklist = Worklist_next;
                Worklist_next.clear();
            }

            // add MainBodyBrInst
            body->insert(MainBodyBrInst);
        }

        BasicBlock* ResCondbb = new BasicBlock(func, symbol_table::gen_label(1));
        BasicBlock* ResBodybb = new BasicBlock(func, symbol_table::gen_label(1));

        dynamic_cast<IfInstruction*>(CondBranchInstr)->setFalseBranch(ResCondbb);
        
        // create res cond
        std::map<Operand*, Operand*> op2op_rescond;
        for(auto CondIns = cond->begin(); CondIns != cond->end()->getNext(); CondIns = CondIns->getNext()){
            if(CondIns->isBranch()){
                continue;
            }

            Instruction* copy_ins = CondIns->copy(ResCondbb);

            // replace use operands
            int NumOfUse = copy_ins->getUse().size(), eps = copy_ins->getDef() ? 1 : 0;
            for(int i = 0; i < NumOfUse; i++){
                Operand* OperUse = copy_ins->getOperandByIndex(i + eps);
                if(op2op_rescond.find(OperUse) != op2op_rescond.end()){
                    copy_ins->setOperandByIndex(i + eps, op2op_rescond[OperUse]);
                }
            }

            // replace def operands
            Operand* OperDef = copy_ins->getDef();
            if(OperDef){
                Operand* NewOper = this->mycopy(OperDef);
                op2op_rescond.insert({OperDef, NewOper});
                CondIns->setOperandByIndex(0, NewOper);
            }
        }
        Operand* CmpOper = ResCondbb->end()->getDef();
        IfInstruction* IfInst = new IfInstruction(ResBodybb, Exitbb, CmpOper);
        ResCondbb->insert(IfInst);

        // create res body
        for(auto BodyIns = body->begin(); BodyIns != body->end()->getNext(); BodyIns = BodyIns->getNext()){
            if(BodyIns->isBranch()){
                continue;
            }

            Instruction* copy_ins = BodyIns->copy(ResBodybb);

            // replace use operands
            int NumOfUse = copy_ins->getUse().size(), eps = copy_ins->getDef() ? 1 : 0;
            for(int i = 0; i < NumOfUse; i++){
                Operand* OperUse = copy_ins->getOperandByIndex(i + eps);
                if(op2op_rescond.find(OperUse) != op2op_rescond.end()){
                    copy_ins->setOperandByIndex(i + eps, op2op_rescond[OperUse]);
                }
            }

            // replace def operands
            Operand* OperDef = copy_ins->getDef();
            if(OperDef){
                Operand* NewOper = this->mycopy(OperDef);
                op2op_rescond.insert({OperDef, NewOper});
                BodyIns->setOperandByIndex(0, NewOper);
            }     
        }
        BranchInstruction* BranInst = new BranchInstruction(ResCondbb);
        ResBodybb->insert(BranInst);

        // replace phi instruction operand

        // modify basicblock's pred & succ relations
        {
            Exitbb->addPred(ResCondbb);
            ResCondbb->addSucc(Exitbb);
            Exitbb->erasePred(cond);
            cond->eraseSucc(Exitbb);
            ResCondbb->addPred(cond);
            cond->addSucc(ResCondbb);

            ResCondbb->addSucc(ResBodybb);
            ResBodybb->addPred(ResCondbb);
            ResBodybb->addSucc(ResCondbb);
            ResCondbb->addPred(ResBodybb);
        }

        // add new relations between bbs and func is not necessary, cuz we make it when we new the bb
        Exception("Normal Unrolling Succeed..");
        return;
    }
    else{
        /*
        *  Other Conditions:
        *  Something todo;
        */
       Exception("todo");
       return;
    }
}
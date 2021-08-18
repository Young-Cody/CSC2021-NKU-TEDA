#include "AutomaticInlining.h"
#include "unit.h"

#define DEBUG false

void AutomaticInlining::Exception(std::string error_code){
    // just for debug
    if(DEBUG)
        cerr << "[IN AutomaticInlining]: " << error_code << endl;     
}

AutomaticInlining::AutomaticInlining(Unit *unit) : Pass(unit){
    ;
}

void AutomaticInlining::pass(){
    for(auto t = unit->begin(); t != unit->end(); t++)
    {
        func = *t;
        Label();
    }

    for(auto t = unit->begin(); t != unit->end(); t++){
        func = *t;
        VerySimpleAMI();
    }
}

void AutomaticInlining::Label(){
    if(func->getSymPtr()->name == "main"){
        return;
    }

    int InsTot = 0;
    if(func->getBlockList().size() <= 3){
        for(auto bb : func->getBlockList()){
            if(bb->empty()) continue;
            for(auto ins = bb->begin(); ins != bb->end()->getNext(); ins = ins->getNext()){
                InsTot += 1;
                if(ins->isCall()){
                    Suitable[func] = 0;
                    return;
                }

                if(ins->isBranch() && ins != bb->end()){
                    Suitable[func] = 0;
                    return;
                }

                if(InsTot > MAX_INSNUM_TOINLINE){
                    Suitable[func] = 0;
                    return;
                }
            }
        }
        Suitable[func] = 1;
    }
    else Suitable[func] = 0;
}

Operand* AutomaticInlining::mycopy(Operand* src){
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
    return nullptr;
}

bool AutomaticInlining::isSuitable(Function* func){
    if(Suitable.find(func) != Suitable.end()){
        return Suitable.find(func)->second;
    }
    return false;
}

void AutomaticInlining::VerySimpleAMI(){
    for(auto bb : func->getBlockList()){
        if(bb->empty()) continue;
        for(auto ins = bb->begin(); ins != bb->end()->getNext(); ins = ins->getNext()){
            if(!ins->isCall() || !dynamic_cast<CallInstruction*>(ins)){
                continue;
            }
            CallInstruction* CallIns = dynamic_cast<CallInstruction*>(ins);
            Function* FuncToInline = CallIns->getCallee();
            if(!isSuitable(FuncToInline)) continue;

            Exception("AutomaticInlining begin..");

            // map from "virtual param" to "real param"
            std::map<Operand*, Operand*> op2param;
            // map from operands defined in function to new operands
            std::map<Operand*, Operand*> op2op;

            // construct op2param
            type* tp = FuncToInline->getSymPtr()->tp;
           
            for(int i = 0; tp->func_params && tp->func_params[i]; i++){
                for(auto funcbb : FuncToInline->getBlockList()){
                    if(funcbb->empty()) continue;
                    for(auto funcins = funcbb->begin(); funcins != funcbb->end()->getNext(); funcins = funcins->getNext()){
                        std::vector<Operand*> funcinsOpers = funcins->getOperands();
                        for(auto funcinsOper : funcinsOpers){
                            if(!funcinsOper->isTemp()) continue;
                            Temporary* tmp = dynamic_cast<Temporary*>(funcinsOper);
                            if(tmp && op2op.find(tmp) == op2op.end() && tmp->getNo() == tp->func_params[i]->label){
                                op2param.insert({tmp, CallIns->getUse()[i]});
                            }
                        }
                    }
                }
            }

            // for(auto item : op2param){
                // cerr<<item.first->getName()<<" "<<item.second->getName()<<endl;
            // }

            // copy instructions
            for(auto Calleebb : FuncToInline->getBlockList()){
                if(Calleebb->empty()) continue;
                for(auto CalleeIns = Calleebb->begin(); CalleeIns != Calleebb->end()->getNext(); CalleeIns = CalleeIns->getNext()){
                    if(CalleeIns->isRet()){
                        RetInstruction* CalleeRetIns = dynamic_cast<RetInstruction*>(CalleeIns);
                        if(CalleeRetIns->getRetVoid() || CalleeRetIns->getRetUndef()) continue;
                        Constant* ZeroCons = new Constant(0);
                        
                        Operand* NewRetUse = CalleeRetIns->getUse()[0];
                        if(op2op.find(NewRetUse) != op2op.end()){
                            NewRetUse = op2op.find(NewRetUse)->second;
                        }

                        BinaryInstruction* NewBiInst = new BinaryInstruction(BinaryInstruction::ADD, CallIns->getDef(), NewRetUse, ZeroCons);
                        NewBiInst->setParent(bb);
                        bb->insertBefore(NewBiInst, CallIns);
                    }
                    else if(CalleeIns->isBranch()){
                        continue;
                    }
                    else{
                        Instruction* copy_ins = CalleeIns->copy();
                        copy_ins->setParent(bb);
                        bb->insertBefore(copy_ins, CallIns);

                        int eps = copy_ins->getDef() ? 1 : 0, round = copy_ins->getUse().size();
                        // replace use
                        for(int i = 0; i < round; i++){
                            Operand* olduseop = copy_ins->getOperandByIndex(i + eps);
                            if(op2param.find(olduseop) != op2param.end()){
                                copy_ins->setOperandByIndex(i + eps, op2param[olduseop]);
                            }
                            else if(op2op.find(olduseop) != op2op.end()){
                                copy_ins->setOperandByIndex(i + eps, op2op[olduseop]);
                            }
                        }
                        // replace def
                        if(eps){
                            Operand* newdefop = this->mycopy(copy_ins->getDef());
                            op2op.insert({copy_ins->getDef(), newdefop});
                            copy_ins->setOperandByIndex(0, newdefop);
                        }
                    }
                }
            }

            CallIns->eraseFromParent();
            Exception("AutomaticInlining finish..");
        }
    }
}
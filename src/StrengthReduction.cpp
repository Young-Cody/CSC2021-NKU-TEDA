#include "StrengthReduction.h"
#include "unit.h"

void StrengthReduction::pass() 
{
    for (auto func = unit->begin(); func != unit->end(); func++)
        pass(*func);
}

void StrengthReduction::pass(Function *func) 
{
    std::vector<Instruction*> worklist;
    for(auto inst = func->instBegin(); inst != func->instEnd(); inst = inst->getNext())
    {
        if(!inst->isBinary())
            continue;
        Instruction *insert_inst;
        insert_inst = nullptr;
        auto rhs = inst->getUse();
        Operand *u1 = rhs[0];
        Operand *u2 = rhs[1];
        if(!u1->isConstant() && !u2->isConstant())
            continue;
        if (inst->getOP() == BinaryInstruction::MUL)
        {
            if(u1->isConstant())
            {
                int n = dynamic_cast<Constant*>(u1)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
                else if(n == 1)
                    insert_inst = new CopyInstruction(inst->getDef(), u2);
                else if(!(n & (n - 1)) && n > 0)
                    insert_inst = new BinaryInstruction(BinaryInstruction::LSL, inst->getDef(), u2, new Constant(__builtin_ctz(n)));
            }
            else if(insert_inst == nullptr && u2->isConstant())
            {
                int n = dynamic_cast<Constant*>(u2)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u2);
                else if(n == 1)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
                else if(!(n & (n - 1)) && n > 0)
                    insert_inst = new BinaryInstruction(BinaryInstruction::LSL, inst->getDef(), u1, new Constant(__builtin_ctz(n)));
            }
        }
        else if (inst->getOP() == BinaryInstruction::DIV)
        {
            if(u1->isConstant())
            {
                int n = dynamic_cast<Constant*>(u1)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
            }
            else if (insert_inst == nullptr && u2->isConstant())
            {
                int n = dynamic_cast<Constant*>(u2)->getValue();
                if(n == 1)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
            }
        }
        else if (inst->getOP() == BinaryInstruction::ADD)
        {
            if(u1->isConstant())
            {
                int n = dynamic_cast<Constant*>(u1)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u2);
            }
            else if(insert_inst == nullptr && u2->isConstant())
            {
                int n = dynamic_cast<Constant*>(u2)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
            }
        }
        else if (inst->getOP() == BinaryInstruction::MINUS)
        {
            if(u2->isConstant())
            {
                int n = dynamic_cast<Constant*>(u2)->getValue();
                if(n == 0)
                    insert_inst = new CopyInstruction(inst->getDef(), u1);
            }
        }
        else if(inst->getOP() == BinaryInstruction::MOD)
        {
            if(u2->isConstant())
            {
                int n = dynamic_cast<Constant*>(u2)->getValue();
                if(n == 1)
                    insert_inst = new CopyInstruction(inst->getDef(), new Constant(0));
            }
        }
        if(insert_inst != nullptr)
        {
            inst->insertBefore(insert_inst);
            worklist.push_back(inst);
        }
    }
    for(auto &inst:worklist)
        inst->eraseFromParent();
}

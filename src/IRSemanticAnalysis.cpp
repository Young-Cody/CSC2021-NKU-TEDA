#include "IRSemanticAnalysis.h"
#include <set>
#include "basicblock.h"
#include "unit.h"

IRSemAnalysis::IRSemAnalysis(Unit *unit) : Pass(unit)
{
}

void IRSemAnalysis::pass()
{
    for (auto func = unit->begin(); func != unit->end(); func++)
    {
        eraseInstAfterRet(*func);
        movAlloca(*func);
        prune(*func);
    }
}

void IRSemAnalysis::prune(Function *func)
{
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
            worklist.push_back(*bb);
    for (auto &bb : worklist)
        bb->eraseFromParent();
}

void IRSemAnalysis::movAlloca(Function *func)
{
    BasicBlock *first_bb = *func->getEntry()->succ_begin();
    for (auto &inst : promoteAlloca)
    {
        inst->getParent()->remove(inst);
        first_bb->insertBefore(inst, first_bb->begin());
    }
}

void IRSemAnalysis::eraseInstAfterRet(Function *func)
{
    promoteAlloca.clear();
    for (auto bb = func->begin(); bb != func->end(); bb++)
    {
        if (func->isEntry(*bb) || func->isExit(*bb))
            continue;
        for (auto inst = (*bb)->begin(); inst != (*bb)->end()->getNext(); inst = inst->getNext())
        {
            if (inst->isRet() && inst != (*bb)->end())
            {
                (*bb)->eraseAllSucc();
                (*bb)->erase(inst->getNext(), (*bb)->end());
                (*bb)->addSucc(func->getExit());
                func->getExit()->addPred(*bb);
                break;
            }
            else if (inst->isAlloca())
                promoteAlloca.push_back(inst);
        }
    }
}

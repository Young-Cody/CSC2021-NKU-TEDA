#include "SSCP.h"
#include "unit.h"

SSCP::SSCP(Unit *unit) : Pass(unit)
{
}

void SSCP::pass()
{
    for (auto func = unit->begin(); func != unit->end(); func++)
        constantPropagation(*func);
}

void SSCP::constantPropagation(Function *func)
{
    std::map<Operand *, std::pair<char, int>> value;
    std::vector<Operand *> worklist;
    for (auto inst = func->instBegin(); inst != func->instEnd(); inst = inst->getNext())
    {
        Operand *def = inst->getDef();
        if (def == nullptr)
            continue;
        if (def->isSSAName())
        {
            value[def] = inst->getLatticeValue(value);
            if (value[def].first != 1)
                worklist.push_back(def);
        }
    }
    while (!worklist.empty())
    {
        Operand *d = worklist.back();
        worklist.pop_back();
        for (auto &inst : d->getUse())
        {
            Operand *m = inst->getDef();
            if (m == nullptr || value.find(m) == value.end())
                continue;
            if (value[m].first != -1)
            {
                std::pair<char, int> old_val = value[m];
                value[m] = inst->getLatticeValue(value);
                if (value[m] != old_val)
                    worklist.push_back(m);
            }
        }
    }
    std::vector<Instruction *> delete_list;
    for (auto &op : value)
    {
        if (op.second.first == -1)
            continue;
        Operand *cst;
        cst = unit->getConstant(op.second.second);
        Instruction *def = op.first->getDef()[0];
        std::vector<Instruction *> use = op.first->getUse();
        delete_list.push_back(def);
        for (auto &use_inst : use)
            use_inst->replaceUse(op.first, cst);
    }
    for (auto &i : delete_list)
        i->eraseFromParent();
}

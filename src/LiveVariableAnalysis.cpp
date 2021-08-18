#include "LiveVariableAnalysis.h"
#include "machine.h"
#include <algorithm>

void LiveVariableAnalysis::pass(m_unit *unit)
{
    for (auto &func : unit->get_func_list())
    {
        computeUsePos(func);
        computeDefUse(func);
        iterate(func);
    }
}

void LiveVariableAnalysis::pass(m_function *func)
{
    computeUsePos(func);
    computeDefUse(func);
    iterate(func);
}

void LiveVariableAnalysis::computeDefUse(m_function *func)
{
    for (auto &block : func->get_block_list())
    {
        for (auto inst = block->get_inst_list().begin(); inst != block->get_inst_list().end(); inst++)
        {
            auto user = (*inst)->getUse();
            std::set<m_operand *> temp(user.begin(), user.end());
            set_difference(temp.begin(), temp.end(),
                           def[block].begin(), def[block].end(), inserter(use[block], use[block].end()));
            auto defs = (*inst)->getDef();
            for (auto &d : defs)
                def[block].insert(def_pos[*d].begin(), def_pos[*d].end());
        }
    }
}

void LiveVariableAnalysis::iterate(m_function *func)
{
    for (auto &block : func->get_block_list())
        block->get_live_in().clear();
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &block : func->get_block_list())
        {
            block->get_live_out().clear();
            auto old = block->get_live_in();
            for (auto &succ : block->get_succ())
                block->get_live_out().insert(succ->get_live_in().begin(), succ->get_live_in().end());
            block->get_live_in() = use[block];
            std::vector<m_operand *> temp;
            set_difference(block->get_live_out().begin(), block->get_live_out().end(),
                           def[block].begin(), def[block].end(), inserter(block->get_live_in(), block->get_live_in().end()));
            if (old != block->get_live_in())
                change = true;
        }
    }
}

void LiveVariableAnalysis::computeUsePos(m_function *func)
{
    for (auto &block : func->get_block_list())
    {
        for (auto &inst : block->get_inst_list())
        {
            auto uses = inst->getUse();
            for (auto &use : uses)
                def_pos[*use].insert(use);
        }
    }
}

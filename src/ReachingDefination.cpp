#include "ReachingDefination.h"
#include "machine.h"
#include <algorithm>

void ReachingDefination::pass(m_unit *unit)
{
    for (auto &func : unit->get_func_list())
    {
        computeDefPos(func);
        computeGenKill(func);
        iterate(func);
    }
}

void ReachingDefination::pass(m_function *func)
{
    computeDefPos(func);
    computeGenKill(func);
    iterate(func);
}

void ReachingDefination::computeGenKill(m_function *func)
{
    for (auto &block : func->get_block_list())
    {
        for (auto inst = block->get_inst_list().rbegin(); inst != block->get_inst_list().rend(); inst++)
        {
            auto defs = (*inst)->getDef();
            for (auto def : defs)
            {
                std::set<m_operand *> temp({def}), res;
                set_difference(temp.begin(), temp.end(),
                               kill[block].begin(), kill[block].end(), inserter(gen[block], gen[block].end()));
                kill[block].insert(def_pos[*def].begin(), def_pos[*def].end());
                kill[block].erase(def);
            }
        }
    }
}

void ReachingDefination::computeDefPos(m_function *func)
{
    for (auto &block : func->get_block_list())
    {
        for (auto &inst : block->get_inst_list())
        {
            auto defs = inst->getDef();
            for (auto &def : defs)
                def_pos[*def].insert(def);
        }
    }
}

void ReachingDefination::iterate(m_function *func)
{
    for (auto &block : func->get_block_list())
        block->get_def_out().clear();
    symbol_entry *sym_ptr = func->get_sym_ptr();
    if(sym_ptr->tp->func_params != nullptr)
    {
        int i = 0;
        while(sym_ptr->tp->func_params[i] != nullptr && i < 4)
        {
            m_operand *param = new m_operand(m_operand::REG, i);
            func->get_entry()->get_def_out().insert(param);
            i++;
        }
    }
    func->get_entry()->get_def_in() = func->get_entry()->get_def_out();
    bool change;
    change = true;
    while (change)
    {
        change = false;
        for (auto &block : func->get_block_list())
        {
            if(block == func->get_entry())
                continue;
            block->get_def_in().clear();
            std::set<m_operand *> old = block->get_def_out();
            for (auto &pred : block->get_pred())
                block->get_def_in().insert(pred->get_def_out().begin(), pred->get_def_out().end());
            block->get_def_out() = gen[block];
            std::vector<m_operand *> temp;
            set_difference(block->get_def_in().begin(), block->get_def_in().end(),
                           kill[block].begin(), kill[block].end(), inserter(temp, temp.end()));
            block->get_def_out().insert(temp.begin(), temp.end());
            if (old != block->get_def_out())
                change = true;
        }
    }
}

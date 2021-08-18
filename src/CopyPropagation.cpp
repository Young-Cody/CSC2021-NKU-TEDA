#include "CopyPropagation.h"
#include "ReachingDefination.h"
#include "algorithm"

void copy_propagation::pass() 
{
    for(auto &func:unit->get_func_list())
        pass(func);
}

void copy_propagation::pass(m_function*func) 
{
    ReachingDefination rd;
    std::map<m_operand, std::set<m_operand *>> reachingDef;
    std::map<m_operand*, std::set<m_operand*>> ud_chains;
    rd.pass(func);
    for (auto &bb : func->get_block_list())
    {
        reachingDef.clear();
        for (auto &t : bb->get_def_in())
            reachingDef[*t].insert(t);
        for (auto &inst : bb->get_inst_list())
        {
            for(auto &use:inst->getUse())
                ud_chains[use] = reachingDef[*use];
            for (auto def : inst->getDef())
            {
                auto &t = reachingDef[*def];
                auto &s = rd.getDefPos()[*def];
                std::set<m_operand *> res;
                set_difference(t.begin(), t.end(), s.begin(), s.end(), inserter(res, res.end()));
                reachingDef[*def] = res;
                reachingDef[*def].insert(def);
            }
        }
    }


    for (auto &bb : func->get_block_list())
    {
        reachingDef.clear();
        for (auto &t : bb->get_def_in())
            reachingDef[*t].insert(t);
        for (auto &inst : bb->get_inst_list())
        {
            auto used_opes = inst->getUse();
            for(auto used_ope : used_opes)
            {
                auto def_list = reachingDef[*used_ope];
                if(def_list.size() != 1) continue;
                auto def_inst = (*def_list.begin())->get_parent();
                if(def_inst == nullptr)
                    continue;
                if(def_inst->getType() == m_instruction::MOV
                && dynamic_cast<mov_mi*>(def_inst)->is_pure_mov()
                && !def_inst->getUse().empty())
                {
                    if(reachingDef[*def_inst->getUse()[0]] == 
                    ud_chains[def_inst->getUse()[0]])
                    {
                        // replace opes
                        m_operand* new_ope = new m_operand(*def_inst->getUse()[0]);
                        new_ope->set_shift(used_ope->get_shift());
                        inst->replace_use(used_ope, new_ope);
                    } 
                }
            }
            for (auto def : inst->getDef())
            {
                auto &t = reachingDef[*def];
                auto &s = rd.getDefPos()[*def];
                std::set<m_operand *> res;
                set_difference(t.begin(), t.end(), s.begin(), s.end(), inserter(res, res.end()));
                reachingDef[*def] = res;
                reachingDef[*def].insert(def);
            }
        }
    }
}

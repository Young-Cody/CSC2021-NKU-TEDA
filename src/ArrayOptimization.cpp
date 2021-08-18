#include "ArrayOptimization.h"
#include "ReachingDefination.h"
#include <algorithm>

array_optimization::array_optimization(m_unit *u)
{
    this->unit = u;
}

void array_optimization::pass()
{
    for (auto &func : unit->get_func_list())
        pass(func);
}

void array_optimization::pass(m_function *func)
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
            if(inst->getType() == m_instruction::ACCESS)
            {
                if(!inst->get_imm_ope()
                && !dynamic_cast<access_mi*>(inst)->get_offset()
                && inst->getUse().size() > 0)
                {
                    auto def_list = reachingDef[*inst->getUse()[0]];
                    if(def_list.size() == 1)
                    {
                        auto replace_ope = *def_list.begin();
                        auto def_inst = replace_ope->get_parent();
                        if(def_inst->getType() == m_instruction::BINARY 
                        && def_inst->getOP() == binary_mi::ADD)
                        {
                            // if def_inst have been changed after it
                            bool replace_flag = true;
                            auto used_opes = def_inst->getUse(); 
                            for(auto iter = used_opes.begin(); iter!=used_opes.end(); iter++)
                            {
                                if(reachingDef[**iter] != ud_chains[*iter])
                                {
                                    replace_flag = false;
                                    break;
                                }
                            }
                            if(replace_flag)
                            {
                                if(def_inst->getUse().size() > 1 || def_inst->get_imm_ope())
                                {
                                    m_operand* new_offset = nullptr;
                                    if(def_inst->get_imm_ope())
                                        new_offset = new m_operand(*def_inst->get_imm_ope());
                                    else
                                        new_offset = new m_operand(*def_inst->getUse()[0]);
                                    dynamic_cast<access_mi*>(inst)->add_offset(new_offset);
                                }
                                m_operand* new_ope = new m_operand(*def_inst->getUse().back());
                                inst->replace_use(inst->getUse()[0], new_ope);
                            }
                        }
                        if(def_inst->getType() == m_instruction::MOV)
                        {
                            // if def_inst->use has been change after it
                            if(reachingDef[*def_inst->getUse()[0]] == 
                            ud_chains[def_inst->getUse()[0]])
                            {
                                // replace ope
                                m_operand* new_ope = new m_operand(*def_inst->getUse()[0]);
                                inst->replace_use(inst->getUse()[0], new_ope);
                            }
                        }
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

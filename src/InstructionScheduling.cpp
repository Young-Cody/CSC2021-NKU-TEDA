#include "InstructionScheduling.h"
#include <list>

void InstructionScheduling::pass() 
{
    for(auto &func:unit->get_func_list())
        for(auto &block:func->get_block_list())
        {
            if(block->get_inst_list().empty())
                continue;
            bb = block;
            init();
            if(inst_list.size() <= 1)
                continue;
            build_dependence_graph();
            schedule();
            modify_code();
        }
}

void InstructionScheduling::schedule() 
{
    int cur_time = 0;
    std::set<int> cands;
    for(int i = 0; i < (int)in_degree.size(); i++)
        if(!in_degree[i])
            cands.insert(i);
    compute_Estart();
    compute_Lstart();
    while (!cands.empty())
    {
        std::set<int> mcands, ecands;
        for(auto &cand:cands)
            if(Estart[cand] <= cur_time) // Todo
                mcands.insert(cand);
        int min_Ltime = INT32_MAX;
        for(auto &cand:cands)
        {
            if(mcands.find(cand) != mcands.end())
                continue;
            min_Ltime = min(min_Ltime, Lstart[cand]);
        }
        for(auto mcand:mcands)
            if(Lstart[mcand] < min_Ltime)
                ecands.insert(mcand);
        int selected;
        if(!ecands.empty())
            selected = *ecands.begin();
        else
        {
            cur_time = cur_time + 1;
            continue;
        }
        cands.erase(selected);
        sched_order.push_back(selected);
        for(auto succ:succs[selected])
        {
            in_degree[succ]--;
            if(!in_degree[succ])
                cands.insert(succ);
        }
    }
}

void InstructionScheduling::init() 
{
    sched_order.clear();
    auto it = bb->get_last_nonbranch_inst();
    if(it == bb->get_inst_list().end())
    {
        inst_list.clear();
        return;
    }
    it++;
    inst_list = std::vector<m_instruction*>(bb->get_inst_list().begin(), it);
    if(inst_list.size() <= 1)
        return;
    dependence_graph.resize(inst_list.size());
    for(auto &v:dependence_graph)
    {
        v.resize(inst_list.size());
        for(auto &i:v)
            i = 0;
    }
    
    succs.resize(inst_list.size());
    for(auto &i:succs)
        i.clear();
    preds.resize(inst_list.size());
    for(auto &i:preds)
        i.clear();
    Estart.resize(inst_list.size());
    for(auto &i:Estart)
        i = -1;
    Lstart.resize(inst_list.size());
    for(auto &i:Lstart)
        i = INT32_MAX;
    in_degree.resize(inst_list.size());
    for(auto &i:in_degree)
        i = 0;
}

void InstructionScheduling::modify_code() 
{
    std::list<m_instruction*> new_inst_order;
    for(auto &i:sched_order)
        new_inst_order.push_back(inst_list[i]);
    auto it = bb->get_last_nonbranch_inst();
    it++;
    for(auto i = it; i != bb->get_inst_list().end(); i++)
        new_inst_order.push_back(*i);
    bb->get_inst_list() = new_inst_order;
}

void InstructionScheduling::build_dependence_graph() 
{
    for(int i = 0; i < (int)inst_list.size(); i++)
    {
        m_instruction *inst_i;
        inst_i = inst_list[i];
        int et = inst_i->get_exec_time();
        if(inst_i->getType() == m_instruction::TST)
            dependence_graph[i][i+1] = et;
        else if(inst_i->isBranch())
        {
            for (int j = i + 1; j < (int)inst_list.size(); j++)
                dependence_graph[i][j] = et;
            continue;
        }
        auto defs_i = inst_i->getDef();
        auto uses_i = inst_i->getUse();
        for(int j = i + 1; j < (int)inst_list.size(); j++)
        {
            m_instruction *inst_j;
            inst_j = inst_list[j];
            auto defs_j = inst_j->getDef();
            auto uses_j = inst_j->getUse();
            for(auto &def_i:defs_i)
                for(auto &use_j:uses_j)
                    if(*def_i == *use_j)
                        dependence_graph[i][j] = et;
            for(auto &use_i:uses_i)
                for(auto &def_j:defs_j)
                    if(*use_i == *def_j)
                        dependence_graph[i][j] = et;
            for(auto &def_i:defs_i)
                for(auto &def_j:defs_j)
                    if(*def_i == *def_j)
                        dependence_graph[i][j] = et;
            if(inst_i->isMemAccess() && inst_j->isMemAccess())
            {
                access_mi *acc_i, *acc_j;
                acc_i = dynamic_cast<access_mi*>(inst_i);
                acc_j = dynamic_cast<access_mi*>(inst_j);
                if(acc_i->is_load() && uses_i.empty())
                    continue;
                if(acc_j->is_load() && uses_j.empty())
                    continue;
                if(acc_i->is_load() && acc_j->is_store())
                    dependence_graph[i][j] = et;
                if(acc_i->is_store() && acc_j->is_load())
                    dependence_graph[i][j] = et;
                if(acc_i->is_store() && acc_j->is_store())
                    dependence_graph[i][j] = et;
            }
            if(inst_j->isBranch())
                dependence_graph[i][j] = et;
        }
    }
    for(int i = 0; i < (int)inst_list.size(); i++)
        for(int j = 0; j < (int)inst_list.size(); j++)
            if(dependence_graph[i][j])
            {
                succs[i].push_back(j);
                preds[j].push_back(i);
                in_degree[j]++;
            }
}

void InstructionScheduling::compute_Estart(int i) 
{
    if(preds[i].empty())
        Estart[i] = 0;
    for(auto &pred:preds[i])
    {
        if(Estart[pred] == -1)
            compute_Estart(pred);
        Estart[i] = max(Estart[i], Estart[pred] + dependence_graph[pred][i]);
    }
}

void InstructionScheduling::compute_Lstart(int i) 
{
    for(auto &succ:succs[i])
    {
        if(Lstart[succ] == INT32_MAX)
            compute_Lstart(succ);
        Lstart[i] = min(Lstart[i], Lstart[succ] - dependence_graph[i][succ]);
    }
}

void InstructionScheduling::compute_Estart() 
{
    for(int i = 0; i < (int)Estart.size(); i++)
        if(Estart[i] == -1)
            compute_Estart(i);
}

void InstructionScheduling::compute_Lstart() 
{
    int v = -1;
    for(int i = 0; i < (int)Estart.size(); i++)
        v = max(v, Estart[i]);
    for(int i = 0; i < (int)Lstart.size(); i++)
        if (succs[i].empty())
            Lstart[i] = v;
    for(int i = 0; i < (int)Lstart.size(); i++)
        if(Lstart[i] == INT32_MAX)
            compute_Lstart(i);
}

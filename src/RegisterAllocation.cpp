#include "RegisterAllocation.h"
#include "machine.h"
#include "ReachingDefination.h"
#include "LiveVariableAnalysis.h"
#include "ControlFlowAnalysis.h"
#include <math.h>
#include <algorithm>

RegisterAllocation::RegisterAllocation(m_unit *unit)
{
    this->unit = unit;
    nregs = 12;
    defWt = 2;
    useWt = 4;
    copyWt = 1;
}

void RegisterAllocation::allocateRegisters()
{
    for (auto &f : unit->get_func_list())
    {
        func = f;
        bool success;
        success = false;
        while (!success)
        {
            bool change;
            change = true;
            while (change)
            {
                makeWebs();
                buildAdjMatrix();
                change = regCoalesce();
            }
            buildAdjLists();
            computeSpillCosts();
            pruneGraph();
            success = assignRegs();
            if (success)
                modifyCode();
            else
                genSpillCode();
        }
    }
}

void RegisterAllocation::makeDuChains()
{
    ReachingDefination rd;
    rd.pass(func);
    std::map<m_operand, std::set<m_operand *>> reachingDef;
    du_chains.clear();
    for (auto &bb : func->get_block_list())
    {
        for (auto &inst : bb->get_inst_list())
        {
            auto defs = inst->getDef();
            for (auto &def : defs)
                if (def->need_color())
                    du_chains[def].insert({});
        }
    }

    for (auto &bb : func->get_block_list())
    {
        reachingDef.clear();
        for (auto &t : bb->get_def_in())
            reachingDef[*t].insert(t);
        for (auto &inst : bb->get_inst_list())
        {
            for (auto &use : inst->getUse())
            {
                if (use->need_color())
                {
                    if (reachingDef[*use].empty())
                        du_chains[use].insert(use);
                    for (auto &def : reachingDef[*use])
                        du_chains[def].insert(use);
                }
            }
            auto defs = inst->getDef();
            for (auto &def : defs)
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

void RegisterAllocation::makeWebs()
{
    makeDuChains();
    webs.clear();
    operand2web.clear();
    for (auto &du_chain : du_chains)
    {
        Web *web;
        double initSpillCost = 0;
        if (du_chain.first->isreg())
            initSpillCost = __DBL_MAX__ / 2;
        web = new Web({{du_chain.first}, du_chain.second, false, initSpillCost, 0, -1, -1});
        webs.push_back(web);
    }
    bool change;
    change = true;
    while (change)
    {
        change = false;
        std::vector<Web *> t(webs.begin(), webs.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++)
            {
                Web *w1 = t[i];
                Web *w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin())
                {
                    std::set<m_operand *> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(), w2->uses.begin(), w2->uses.end(), inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        auto it = std::find(webs.begin(), webs.end(), w2);
                        if (it != webs.end())
                            webs.erase(it);
                    }
                }
            }
    }
    int i = 0;
    std::vector<Web *> temp;
    for (i = 0; i < nregs; i++)
    {
        Web *reg = new Web({std::set<m_operand *>(), std::set<m_operand *>(), false, __DBL_MAX__ / 2, -1, -1, i});
        temp.push_back(reg);
    }
    for (auto &web : webs)
    {
        web->sreg = i;
        for (auto &def : web->defs)
            operand2web[def] = i;
        for (auto &use : web->uses)
            operand2web[use] = i;
        i++;
    }
    webs.insert(webs.begin(), temp.begin(), temp.end());
}

// build the adjacency matrix representation of the inteference graph
void RegisterAllocation::buildAdjMatrix()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    adjMtx.resize(webs.size());
    for (int i = 0; i < (int)webs.size(); i++)
        adjMtx[i].resize(webs.size());
    for (int i = 0; i < (int)webs.size(); i++)
        for (int j = 0; j < (int)webs.size(); j++)
            adjMtx[i][j] = 0;

    for (int i = 0; i < nregs; i++)
        for (int j = 0; j < i; j++)
        {
            adjMtx[i][j] = true;
            adjMtx[j][i] = true;
        }
    for (int i = nregs; i < (int)webs.size(); i++)
    {
        m_operand *def = *webs[i]->defs.begin();
        if (!def->isreg())
            continue;
        webs[i]->rreg = def->get_reg();
        int u = operand2web[def];
        for (int i = 0; i < nregs; i++)
        {
            if (i == def->get_reg())
                continue;
            adjMtx[u][i] = 1;
            adjMtx[i][u] = 1;
        }
    }
    for (auto &bb : func->get_block_list())
    {
        auto livenow = bb->get_live_out();
        for (auto inst = bb->get_inst_list().rbegin(); inst != bb->get_inst_list().rend(); inst++)
        {
            auto defs = (*inst)->getDef();
            for (auto &def : defs)
            {
                if (operand2web.find(def) != operand2web.end())
                {
                    int u = operand2web[def];
                    for (auto &live : livenow)
                    {
                        if (operand2web.find(live) == operand2web.end())
                            continue;
                        int v = operand2web[live];
                        adjMtx[u][v] = 1;
                        adjMtx[v][u] = 1;
                    }
                }
                std::set<m_operand *> &use_pos = lva.getUsePos()[*def];
                for (auto &del : use_pos)
                    if (livenow.find(del) != livenow.end())
                        livenow.erase(del);
            }
            for (auto &use : (*inst)->getUse())
                livenow.insert(use);
        }
    }
}

// build the adjacency list representation of the inteference graph
void RegisterAllocation::buildAdjLists()
{
    adjList.resize(adjMtx.size(), std::vector<int>());

    for (size_t u = 0; u < adjMtx.size(); u++)
        for (size_t v = 0; v < u; v++)
        {
            if (adjMtx[u][v])
            {
                adjList[u].push_back(v);
                adjList[v].push_back(u);
            }
        }
    rmvList = adjList;
}

void RegisterAllocation::computeSpillCosts()
{
    ControlFlowAnalysis cfa;
    cfa.pass(func);
    for (auto &bb : func->get_block_list())
    {
        double factor = pow(10, cfa.getLoopDepth(bb));
        for (auto &inst : bb->get_inst_list())
        {
            auto defs = inst->getDef();
            for (auto &def : defs)
            {
                if (def->isreg())
                    continue;
                int w = operand2web[def];
                webs[w]->spillCost += factor * defWt;
                if (inst->isMov())
                    webs[w]->spillCost -= factor * copyWt;
            }
            for (auto &use : inst->getUse())
            {
                if (use->isreg())
                    continue;
                int w = operand2web[use];
                webs[w]->spillCost += factor * useWt;
                if (inst->isMov())
                    webs[w]->spillCost -= factor * copyWt;
            }
        }
    }
}

void RegisterAllocation::adjustIG(int i)
{
    for (auto v : adjList[i])
    {
        auto it = std::find(adjList[v].begin(), adjList[v].end(), i);
        adjList[v].erase(it);
        if (adjList[v].empty())
            pruneStack.push_back(v);
    }
    adjList[i].clear();
}

void RegisterAllocation::pruneGraph()
{
    pruneStack.clear();
    for (size_t i = 0; i < adjList.size(); i++)
        if (adjList[i].size() == 0)
            pruneStack.push_back(i);
    bool success;
    success = true;
    while (success)
    {
        success = false;
        for (size_t i = 0; i < adjList.size(); i++)
        {
            if (adjList[i].size() > 0 && adjList[i].size() < (size_t)nregs && webs[i]->rreg == -1)
            {
                success = true;
                pruneStack.push_back(i);
                adjustIG(i);
            }
        }
    }
    while (pruneStack.size() < adjList.size())
    {
        double minSpillCost = __DBL_MAX__;
        int spillnode = -1;
        for (size_t i = 0; i < adjList.size(); i++)
        {
            int deg = adjList[i].size();
            if (deg == 0)
                continue;
            if (webs[i]->spillCost / deg < minSpillCost)
            {
                minSpillCost = webs[i]->spillCost / deg;
                spillnode = i;
            }
        }
        pruneStack.push_back(spillnode);
        adjustIG(spillnode);
    }
}

bool RegisterAllocation::regCoalesce()
{
    bool flag;
    flag = false;
    std::vector<m_instruction *> del_list;
    for (auto &bb : func->get_block_list())
    {
        for (auto &inst : bb->get_inst_list())
        {
            if (!inst->isMov())
                continue;
            if (!dynamic_cast<mov_mi *>(inst)->is_pure_mov())
                continue;
            m_operand *dst = *inst->getDef().begin();
            auto uses = inst->getUse();
            if (uses.empty())
                continue;
            m_operand *src = *uses.begin();
            if (operand2web.find(dst) == operand2web.end())
                continue;
            if (operand2web.find(src) == operand2web.end())
                continue;
            if (dst->isreg() || src->isreg())
                continue;
            int u = operand2web[dst];
            int v = operand2web[src];
            if (!adjMtx[u][v])
            {
                flag = true;
                for (size_t i = 0; i < adjMtx.size(); i++)
                    adjMtx[i][u] = adjMtx[i][v] = adjMtx[v][i] = adjMtx[u][i] = adjMtx[u][i] || adjMtx[v][i];
                for (auto &use : webs[u]->uses)
                {
                    m_instruction *parent = use->get_parent();
                    m_operand *new_version = new m_operand(*src);
                    operand2web[new_version] = v;
                    webs[v]->uses.insert(new_version);
                    new_version->set_shift(use->get_shift());
                    parent->replace_use(use, new_version);
                }
                for (auto &def: webs[u]->defs)
                {
                    m_instruction *parent = def->get_parent();
                    m_operand *new_version = new m_operand(*src);
                    operand2web[new_version] = v;
                    webs[v]->defs.insert(new_version);
                    parent->replace_def(def, new_version);
                }
                del_list.push_back(inst);
            }
        }
    }
    for (auto &inst : del_list)
    {
        m_block *bb = inst->get_parent();
        bb->remove(inst);
    }
    return flag;
}

bool RegisterAllocation::assignRegs()
{
    bool success;
    success = true;
    while (!pruneStack.empty())
    {
        int w = pruneStack.back();
        pruneStack.pop_back();
        int color = minColor(w);
        if (color < 0)
        {
            success = false;
            webs[w]->spill = true;
        }
        else
            webs[w]->rreg = color;
    }
    return success;
}

void RegisterAllocation::modifyCode()
{
    for (int i = nregs; i < (int)webs.size(); i++)
    {
        Web *web = webs[i];
        if (web->rreg > 3)
            func->add_saved_regs(web->rreg);
        for (auto def : web->defs)
            def->set_reg(web->rreg);
        for (auto use : web->uses)
            use->set_reg(web->rreg);
    }
    std::vector<m_instruction *> del_list;
    for (auto &bb : func->get_block_list())
    {
        for (auto &inst : bb->get_inst_list())
        {
            if (!inst->isMov())
                continue;
            if (!dynamic_cast<mov_mi *>(inst)->is_pure_mov())
                continue;
            m_operand *dst = *inst->getDef().begin();
            auto uses = inst->getUse();
            if (uses.empty())
                continue;
            m_operand *src = *uses.begin();
            if (dst->get_reg() == src->get_reg())
                del_list.push_back(inst);
        }
    }
    for (auto &inst : del_list)
    {
        m_block *bb = inst->get_parent();
        bb->remove(inst);
    }
}

void RegisterAllocation::genSpillCode()
{
    for (auto &web : webs)
    {
        if (!web->spill)
            continue;
        func->set_stack_size(4);
        web->disp = -func->get_stack_size();
        for (auto &use : web->uses)
        {
            m_operand *ld = new m_operand(*use);
            ld->set_shift(nullptr);
            m_operand *base = new m_operand(m_operand::REG, 13);
            m_operand *offset = new m_operand(m_operand::IMM, web->disp);
            m_instruction *ld_inst = new access_mi(access_mi::LDR, ld, base, offset);
            use->get_parent()->insert_before(ld_inst);
            func->add_local_backpatch(ld_inst);
        }
        for (auto &def : web->defs)
        {
            m_operand *ld = new m_operand(*def);
            m_operand *base = new m_operand(m_operand::REG, 13);
            m_operand *offset = new m_operand(m_operand::IMM, web->disp);
            int reg_no = unit->assign_vreg();
            m_operand *reg1 = new m_operand(m_operand::VREG, reg_no);
            m_operand *reg2 = new m_operand(m_operand::VREG, reg_no);
            m_instruction *ld_inst = new access_mi(access_mi::LDR, reg1, offset);
            m_instruction *st_inst = new access_mi(access_mi::STR, ld, base, reg2);
            def->get_parent()->insert_after(st_inst);
            def->get_parent()->insert_after(ld_inst);
            func->add_local_backpatch(ld_inst);
        }
    }
}

int RegisterAllocation::minColor(int u)
{
    std::vector<bool> bitv(nregs, 0);
    for (auto &v : rmvList[u])
    {
        if (webs[v]->rreg != -1)
            bitv[webs[v]->rreg] = 1;
    }
    for (int i = 0; i < nregs; i++)
        if (bitv[i] == 0)
            return i;
    return -1;
}

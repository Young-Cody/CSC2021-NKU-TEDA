#include "ControlFlowAnalysis.h"
#include "machine.h"

void ControlFlowAnalysis::pass(m_function *func)
{
    edgeType.clear();
    loopDepth.clear();
    visit.clear();
    for (auto &bb : func->get_block_list())
        loopDepth[bb] = 0;
    i = j = 0;
    dfs(func->get_entry());
    computeLoopDepth();
}

int ControlFlowAnalysis::getLoopDepth(m_block *bb)
{
    return loopDepth[bb];
}

void ControlFlowAnalysis::dfs(m_block *bb)
{
    visit.insert(bb);
    preOrder[bb] = i;
    i++;
    for (auto succ : bb->get_succ())
    {
        if (visit.find(succ) == visit.end())
        {
            edgeType[{bb, succ}] = TREE;
            dfs(succ);
        }
        else if (preOrder[bb] < preOrder[succ])
            edgeType[{bb, succ}] = FORWARD;
        else if (PostOrder.find(succ) == PostOrder.end())
            edgeType[{bb, succ}] = BACKWARD;
        else
            edgeType[{bb, succ}] = CROSS;
    }
    PostOrder[bb] = j;
    j = j + 1;
}

void ControlFlowAnalysis::computeLoopDepth()
{
    for (auto edge : edgeType)
    {
        if (edge.second == BACKWARD)
        {
            auto loop = computeNaturalLoop(edge.first.first, edge.first.second);
            for (auto &bb : loop)
                loopDepth[bb]++;
        }
    }
}

std::set<m_block *> ControlFlowAnalysis::computeNaturalLoop(m_block *u, m_block *v)
{
    std::set<m_block *> loop;
    std::vector<m_block *> s;
    if (u == v)
        loop.insert(u);
    else
    {
        loop.insert(u);
        loop.insert(v);
        s.push_back(u);
    }
    while (!s.empty())
    {
        auto t = s.back();
        s.pop_back();
        for (auto pred : t->get_pred())
        {
            if (loop.find(pred) == loop.end())
            {
                s.push_back(pred);
                loop.insert(pred);
            }
        }
    }
    return loop;
}

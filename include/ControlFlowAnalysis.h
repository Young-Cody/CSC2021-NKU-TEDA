#ifndef _CFA_H__
#define _CFA_H__

#include <map>
#include <set>
class m_function;
class m_unit;
class m_operand;
class m_block;

class ControlFlowAnalysis
{
private:
    enum{NONE, BACKWARD, FORWARD, TREE, CROSS};
    int i, j;
    std::map<std::pair<m_block*, m_block*>, unsigned> edgeType;
    std::map<m_block*, int> preOrder;
    std::map<m_block*, int> PostOrder;
    std::map<m_block*, int> loopDepth;
    std::set<m_block*> visit;
    void dfs(m_block*);
    void computeLoopDepth();
    std::set<m_block*> computeNaturalLoop(m_block*, m_block*);
    
public:
    void pass(m_function *);
    int getLoopDepth(m_block*);
};

#endif
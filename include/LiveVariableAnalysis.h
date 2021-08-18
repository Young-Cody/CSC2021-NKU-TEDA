#ifndef __LIVE_VARIABLE_ANALYSIS_H__
#define __LIVE_VARIABLE_ANALYSIS_H__

#include <set>
#include <map>

class m_function;
class m_unit;
class m_operand;
class m_block;
class LiveVariableAnalysis
{
private:
    std::map<m_operand, std::set<m_operand *>> def_pos;
    std::map<m_block *, std::set<m_operand *>> def, use;
    void computeUsePos(m_function *);
    void computeDefUse(m_function *);
    void iterate(m_function *);

public:
    void pass(m_unit *unit);
    void pass(m_function *func);
    std::map<m_operand, std::set<m_operand *>> &getUsePos() { return def_pos; };
};

#endif
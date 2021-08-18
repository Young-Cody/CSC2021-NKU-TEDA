#ifndef __REACHING_DEFINATION_H__
#define __REACHING_DEFINATION_H__

#include <set>
#include <map>

class m_function;
class m_unit;
class m_operand;
class m_block;
class ReachingDefination
{
private:
    std::map<m_operand, std::set<m_operand *>> def_pos;
    std::map<m_block *, std::set<m_operand *>> gen, kill;
    void computeDefPos(m_function *);
    void computeGenKill(m_function *);
    void iterate(m_function *);

public:
    std::map<m_operand, std::set<m_operand *>> &getDefPos() { return def_pos; };
    void pass(m_unit *unit);
    void pass(m_function *func);
};

#endif
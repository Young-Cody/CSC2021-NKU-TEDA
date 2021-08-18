#ifndef _PRE_H__
#define _PRE_H__

#include <set>
#include <map>
#include <vector>
#include "machine.h"

class PartialRedundancyElimination
{
public:
    using Expression = std::tuple<int, int, std::vector<m_operand>, shift_type>;
    PartialRedundancyElimination(m_unit *u) { unit = u; };
    void pass();

private:
    m_function *func;
    m_unit *unit;
    std::vector<m_instruction *> worklist;
    std::set<Expression> expressions;
    std::map<Expression, m_instruction*> expr2inst;
    std::map<m_operand, std::set<Expression>> oprd_expr;
    std::map<m_block *, std::set<Expression>> anticipated_in;
    std::map<m_block *, std::set<Expression>> anticipated_out;
    std::map<m_block *, std::set<Expression>> available_in;
    std::map<m_block *, std::set<Expression>> available_out;
    std::map<m_block *, std::set<Expression>> postponable_in;
    std::map<m_block *, std::set<Expression>> postponable_out;
    std::map<m_block *, std::set<Expression>> used_in;
    std::map<m_block *, std::set<Expression>> used_out;
    std::map<m_block *, std::set<Expression>> earliest;
    std::map<m_block *, std::set<Expression>> latest;
    std::map<m_block *, std::set<Expression>> use, kill;
    void insertEmptyBlock();
    void computeOprdExpr();
    void computeUseKill();
    void computeAnticipated();
    void computeAvailable();
    void computeEarliest();
    void computePostponable();
    void computeLatest();
    void computeUsed();
    void modifyCode();
    void init();
};

#endif
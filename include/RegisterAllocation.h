#ifndef __REGISTER_ALLOCATION_H__
#define __REGISTER_ALLOCATION_H__
#include <set>
#include <map>
#include <vector>

class m_unit;
class m_operand;
class m_function;

struct Web
{
    std::set<m_operand *> defs;
    std::set<m_operand *> uses;
    bool spill;
    double spillCost;
    int sreg;
    int disp;
    int rreg;
};

class RegisterAllocation
{
private:
    m_unit *unit;
    int nregs;
    double defWt;
    double useWt;
    double copyWt;
    std::vector<int> pruneStack;
    m_function *func;
    std::map<m_operand *, std::set<m_operand *>> du_chains;
    std::vector<Web *> webs;
    std::map<m_operand *, int> operand2web;
    std::vector<std::vector<bool>> adjMtx;
    std::vector<std::vector<int>> adjList;
    std::vector<std::vector<int>> rmvList;
    int minColor(int);
    void makeDuChains();
    void makeWebs();
    void buildAdjMatrix();
    void buildAdjLists();
    void computeSpillCosts();
    void adjustIG(int i);
    void pruneGraph();
    bool regCoalesce();
    bool assignRegs();
    void modifyCode();
    void genSpillCode();

public:
    RegisterAllocation(m_unit *);
    void allocateRegisters();
};

#endif
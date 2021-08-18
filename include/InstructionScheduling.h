#ifndef __INST_SCHED_H__
#define __INST_SCHED_H__

#include "machine.h"
#include <vector>

class InstructionScheduling
{
private:
    m_unit* unit;
    m_block *bb;
    std::vector<m_instruction*> inst_list;
    std::vector<std::vector<int>> dependence_graph;
    std::vector<std::vector<int>> succs;
    std::vector<std::vector<int>> preds;
    std::vector<int> in_degree;
    std::vector<int> Estart;
    std::vector<int> Lstart;
    std::vector<int> sched_order;
    void build_dependence_graph();
    void compute_Estart(int);
    void compute_Lstart(int);
    void compute_Estart();
    void compute_Lstart();
    void schedule();
    void init();
    void modify_code();
    
public:
    InstructionScheduling(m_unit* u){unit = u;};
    void pass();
};

#endif
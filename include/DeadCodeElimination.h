#ifndef _DCE_H__
#define _DCE_H__

#include "pass.h"
#include <set>
#include <vector>

class Function;
class BasicBlock;

class DeadCodeElimination : public Pass
{
private:
    bool allChange;
    bool change;
    Function *func;
    std::vector<BasicBlock *> postOrder;
    void computePostOrder(BasicBlock *, std::set<BasicBlock *> &);
    void eliminateUselessCode();
    void eliminateUselessCodeMark();
    void eliminateUselessCodeSweep();
    void eliminateUselessControlFlow();
    void eliminateUselessControlFlow(BasicBlock *);
    void eliminateUnreachableCode();
    void eliminateSinglePhi();

public:
    DeadCodeElimination(Unit *);
    void pass();
};

#endif
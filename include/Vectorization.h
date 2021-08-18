#ifndef __VECTORIZATION_H
#define __VECTORIZATION_H
#include "LoopAnalyzer.h"
#include "unit.h"

class Vectorization{
private:
    Unit* unit;
    Function* func;
    std::set<Loop*> loops;
    void copy_basicblock(BasicBlock* copy_bb, BasicBlock* bb);
    void vectorizable_loop(Loop* loop);
public:
    Vectorization(Unit* unit);
    void pass();
};

#endif
#ifndef _LUN_H__
#define _LUN_H__

#include "pass.h"
#include "LoopAnalyzer.h"
#include <map>
#include <set>
#include <vector>
#include <cmath>

class Function;
class BasicBlock;
class Operand;
class LoopAnalyzer;

class LoopUnrolling : public Pass
{
private:
    Function *func;
    LoopAnalyzer cfaolun;
    int UnrollingFactor;
    const int MAX_UNROLLING_FACTOR = 4;
    const int MIN_UNROLLING_FACTOR = 2;
    const int MAX_FOLDER = 40;
    const int MAX_INSTRUCTIONS = 500;

    std::set<Loop*> Loops;
    
    bool isSubset(std::set<BasicBlock*> t_son, std::set<BasicBlock*> t_fat);
    void FindLoops();
    void PrepareWorklist();
    void WorkToUnrolling(loopptr*);
    Operand* mycopy(Operand*);
    bool SuitableForSpecialLun(int, int, int, int);
    
public:
    LoopUnrolling(Unit *);
    void pass();
    int getUnrollingFactor(){ return UnrollingFactor; }
    void setUnrollingFactor(int fac){ this->UnrollingFactor = std::max(std::min(fac, MAX_UNROLLING_FACTOR), MIN_UNROLLING_FACTOR); }
};

#endif
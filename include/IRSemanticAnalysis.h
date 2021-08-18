#ifndef _IRSEMANTICANALYSY_H__
#define _IRSEMANTICANALYSY_H__
#include "pass.h"
#include <vector>

class Instruction;
class Function;
class IRSemAnalysis : public Pass
{
private:
    std::vector<Instruction *> promoteAlloca;
    void prune(Function *);
    void movAlloca(Function *);
    void eraseInstAfterRet(Function *);

public:
    IRSemAnalysis(Unit *);
    void pass();
};

#endif
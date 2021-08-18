#ifndef _AMI_H__
#define _AMI_H__

#include "pass.h"
#include <map>
#include <set>
#include <vector>

class Function;
class BasicBlock;
class Operand;

class AutomaticInlining : public Pass
{
private:
    Function *func;
    std::map<Function*, int> Suitable;
    const int MAX_INSNUM_TOINLINE = 40;

    Operand* mycopy(Operand*);
    void Exception(std::string);
    bool isSuitable(Function*);

    void Label();
    void VerySimpleAMI();

public:
    AutomaticInlining(Unit *);
    void pass();
};

#endif
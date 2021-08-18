#ifndef _STR_RED_H__
#define _STR_RED_H__

#include "pass.h"
class Function;

class StrengthReduction : public Pass
{
private:
    void pass(Function *);

public:
    StrengthReduction(Unit *u) :Pass(u){};
    void pass();
};

#endif
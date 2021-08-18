#ifndef __SSA_DESTRUCTION_H__
#define __SSA_DESTRUCTION_H__
#include "pass.h"
#include <map>
#include <vector>
class Function;
class BasicBlock;
class Operand;
class Instruction;
class SSADestruction : public Pass
{
public:
    SSADestruction(Unit *u) : Pass(u){};
    void pass();
};

#endif
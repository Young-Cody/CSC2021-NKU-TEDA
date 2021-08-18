#ifndef __SSA_H__
#define __SSA_H__
#include "pass.h"
#include <map>
#include <vector>
class Function;
class BasicBlock;
class Operand;
class Instruction;
class AllocaInstruction;
class Mem2Reg : public Pass
{
private:
    std::map<Operand *, std::vector<Operand *>> reaching_def;
    std::vector<AllocaInstruction *> allocas;
    void insertPhi();
    void rename(Function *);
    void rename(BasicBlock *);
    Operand *getNewName(Operand *old);
    void getPromotableName(Function *);

public:
    Mem2Reg(Unit *);
    void pass();
};

#endif
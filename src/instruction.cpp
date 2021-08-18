#include "instruction.h"
#include "basicblock.h"
#include <iostream>
#include "function.h"

Instruction::Instruction(unsigned kind, BasicBlock *insert_bb)
{
    prev = next = nullptr;
    opcode = -1;
    this->kind = kind;
    insert_bb->insert(this);
    parent = insert_bb;
    vec_flag = false;
}

Instruction::Instruction(unsigned kind)
{
    prev = next = nullptr;
    opcode = -1;
    this->kind = kind;
    vec_flag = false;
}

bool Instruction::isBranch() const
{
    return kind == IF || kind == BRANCH;
}

bool Instruction::isCondBr() const
{
    return kind == IF;
}

bool Instruction::isUncondBr() const
{
    return kind == BRANCH;
}

bool Instruction::isPhi() const
{
    return kind == PHI;
}

bool Instruction::isRet() const
{
    return kind == RET;
}

bool Instruction::isCall() const
{
    return kind == CALL;
}

bool Instruction::isAlloca() const
{
    return kind == ALLOCA;
}

bool Instruction::isLoad() const
{
    return kind == LOAD;
}

bool Instruction::isStore() const
{
    return kind == STORE;
}

bool Instruction::isCmp() const
{
    return kind == CMP;
}

bool Instruction::isCopy() const
{
    return kind == COPY;
}

bool Instruction::isAddr() const
{
    return kind == ADDR;
}

bool Instruction::isBinary() const
{
    return kind == BINARY;
}

bool Instruction::isUnary() const
{
    return kind == UNARY;
}

bool Instruction::isMarked()
{
    return marked;
}

void Instruction::setMarked()
{
    marked = true;
}

void Instruction::clearMarked()
{
    marked = false;
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

// erase the instruction from its parent basicblock.
void Instruction::eraseFromParent()
{
    removeFromParent();
    delete this;
}

// just remove the instruction from its parent but not erase it.
void Instruction::removeFromParent()
{
    parent->remove(this);
}

// insert instruction inst to the front of this instruction.
void Instruction::insertBefore(Instruction *inst)
{
    parent->insertBefore(inst, this);
}

bool Instruction::iscritical()
{
    /*
    * An operation is critical if 
    * [CASE 1] it sets return values for the procedure,
    * [CASE 2] it is an input/output statement, or
    * [CASE 3] it affects the value in a storage location that may be accessible from outside the current procedure
    */
    // [CASE 1]
    if (isRet())
    {
        if(getUse().empty())
            return true;
        auto preds = parent->getParent()->getPreds();
        if(preds.empty())
            return true;
        for (auto call : preds)
        {
            for (auto &callInst : call.second)
                if (!callInst->getDef()->getUse().empty())
                    return true;
        }
        return false;
    }
    // [CASE 2] func call all considered as input/output statement
    if (isCall())
    {
        Function *callee = dynamic_cast<CallInstruction *>(this)->getCallee();
        return callee == nullptr || callee->isCritical();
    }
    // [case 3] storage location not temporary
    return (this->getKind() == STORE) ||
           (this->getKind() == BRANCH);
}

Instruction *Instruction::getNext() const
{
    return next;
}

Instruction *Instruction::getPrev() const
{
    return prev;
}

UnaryInstruction::UnaryInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(UNARY, insert_bb)
{
    this->opcode = opcode;
    this->dst = dst;
    this->src = src;
    dst->addDef(this);
    src->addUse(this);
}

UnaryInstruction::UnaryInstruction(unsigned opcode, Operand *dst, Operand *src) : Instruction(UNARY)
{
    this->opcode = opcode;
    this->dst = dst;
    this->src = src;
    dst->addDef(this);
    src->addUse(this);
}

Operand *UnaryInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return src;
    default:
        return nullptr;
    }
}

int UnaryInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper; 
        return 0;
    case 1:
        src->removeUse(this); oper->addUse(this);
        src = oper; 
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* UnaryInstruction::copy(){
    return new UnaryInstruction(this->opcode, this->dst, this->src);
}

Instruction *UnaryInstruction::copy(BasicBlock *insert_bb)
{
    return new UnaryInstruction(this->opcode, this->dst, this->src, insert_bb);
}

void UnaryInstruction::print() const
{
    std::string s1, s2;
    s1 = dst->getName();
    s2 = src->getName();
    switch (opcode)
    {
    case UMINUS:
        printf("  %s = sub i32 0, %s\n", s1.c_str(), s2.c_str());
        break;
    default:
        break;
    }
}

void UnaryInstruction::reLabel()
{
    dst->reLabel();
}

void UnaryInstruction::eraseFromParent()
{
    dst->removeDef(this);
    src->removeUse(this);
    Instruction::eraseFromParent();
}

std::pair<char, int> UnaryInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> l;
    if (value.find(src) == value.end())
        l = src->getInitLatticeValue();
    else
        l = value[src];
    if (l.first == 0)
    {
        if (opcode == UMINUS)
            l.second = -l.second;
        else if (opcode == NOT)
            l.second = !l.second;
    }
    return l;
}

void UnaryInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

void UnaryInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src == o)
    {
        src->removeUse(this);
        src = n;
        src->addUse(this);
    }
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    this->dst = dst;
    this->src1 = src1;
    this->src2 = src2;
    dst->addDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2) : Instruction(BINARY)
{
    this->opcode = opcode;
    this->dst = dst;
    this->src1 = src1;
    this->src2 = src2;
    dst->addDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

Operand *BinaryInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return src1;
    case 2:
        return src2;
    default:
        return nullptr;
    }
}

int BinaryInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper; 
        return 0;
    case 1:
        src1->removeUse(this); oper->addUse(this);
        src1 = oper;
        return 0;
    case 2:
        src2->removeUse(this); oper->addUse(this);
        src2 = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* BinaryInstruction::copy(){
    return new BinaryInstruction(this->opcode, this->dst, this->src1, this->src2);
}

Instruction *BinaryInstruction::copy(BasicBlock *insert_bb)
{
    return new BinaryInstruction(this->opcode, this->dst, this->src1, this->src2, insert_bb);
}

void BinaryInstruction::print() const
{
    std::string s1, s2, s3, op;
    s1 = dst->getName();
    s2 = src1->getName();
    s3 = src2->getName();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case MINUS:
        op = "sub";
        break;
    case MUL:
        op = "mul";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
    default:
        break;
    }
    printf("  %s = %s i32 %s, %s\n", s1.c_str(), op.c_str(), s2.c_str(), s3.c_str());
}

void BinaryInstruction::reLabel()
{
    dst->reLabel();
}

void BinaryInstruction::eraseFromParent()
{
    dst->removeDef(this);
    src1->removeUse(this);
    src2->removeUse(this);
    Instruction::eraseFromParent();
}

std::pair<char, int> BinaryInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> res, val1, val2;
    if (value.find(src1) == value.end())
        val1 = src1->getInitLatticeValue();
    else
        val1 = value[src1];
    if (value.find(src2) == value.end())
        val2 = src2->getInitLatticeValue();
    else
        val2 = value[src2];

    if (val1.first == -1 || val2.first == -1)
        res = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        switch (opcode)
        {
        case ADD:
            res.second = val1.second + val2.second;
            break;
        case MINUS:
            res.second = val1.second - val2.second;
            break;
        case MUL:
            res.second = val1.second * val2.second;
            break;
        case DIV:
            res.second = val1.second / val2.second;
            break;
        case MOD:
            res.second = val1.second % val2.second;
            break;
        case AND:
            res.second = val1.second & val2.second;
            break;
        case OR:
            res.second = val1.second | val2.second;
            break;
        case LSL:
            res.second = val1.second << val2.second;
            break;
        case LSR:
            res.second = (unsigned)val1.second >> val2.second;
            break;
        case ASR:
            res.second = val1.second >> val2.second;
            break;
        default:
            break;
        }
    }
    else
        res.first = 1;
    return res;
}

void BinaryInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

void BinaryInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src1 == o)
    {
        src1->removeUse(this);
        src1 = n;
        src1->addUse(this);
    }
    if (src2 == o)
    {
        src2->removeUse(this);
        src2 = n;
        src2->addUse(this);
    }
}

CopyInstruction::CopyInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(COPY, insert_bb)
{
    this->dst = dst;
    this->src = src;
    dst->addDef(this);
    src->addUse(this);
}

CopyInstruction::CopyInstruction(Operand *dst, Operand *src) : Instruction(COPY)
{
    this->dst = dst;
    this->src = src;
    dst->addDef(this);
    src->addUse(this);
}

Operand *CopyInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return src;
    default:
        return nullptr;
    }
}

int CopyInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    case 1:
        src->removeUse(this); oper->addUse(this);
        src = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* CopyInstruction::copy(){
    return new CopyInstruction(dst, src);
}

Instruction *CopyInstruction::copy(BasicBlock *insert_bb)
{
    return new CopyInstruction(dst, src, insert_bb);
}

void CopyInstruction::print() const
{
    std::string s1, s2, t;
    s1 = dst->getName();
    s2 = src->getName();
    t = src->getType();
    printf("  %s = add %s %s, 0\n", s1.c_str(), t.c_str(), s2.c_str());
}

void CopyInstruction::reLabel()
{
    dst->reLabel();
}

void CopyInstruction::eraseFromParent()
{
    dst->removeDef(this);
    src->removeUse(this);
    Instruction::eraseFromParent();
}

std::pair<char, int> CopyInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> res, val;
    if (value.find(src) == value.end())
        val = src->getInitLatticeValue();
    else
        val = value[src];
    return val;
}

void CopyInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

void CopyInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src == o)
    {
        src->removeUse(this);
        src = n;
        src->addUse(this);
    }
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(CMP, insert_bb)
{
    this->opcode = opcode;
    this->src1 = src1;
    this->src2 = src2;
    this->dst = dst;
    dst->addDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2): Instruction(CMP){
    this->opcode = opcode;
    this->src1 = src1;
    this->src2 = src2;
    this->dst = dst;
    dst->addDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

Operand *CmpInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return src1;
    case 2:
        return src2;
    default:
        return nullptr;
    }
}

int CmpInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    case 1:
        src1->removeUse(this); oper->addUse(this);
        src1 = oper;
        return 0;
    case 2:
        src2->removeUse(this); oper->addUse(this);
        src2 = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* CmpInstruction::copy(){
    return new CmpInstruction(this->opcode, dst, src1, src2);
}

Instruction *CmpInstruction::copy(BasicBlock *insert_bb)
{
    return new CmpInstruction(this->opcode, dst, src1, src2, insert_bb);
}

void CmpInstruction::print() const
{
    std::string s, s1, s2, op;
    s = dst->getName();
    s1 = src1->getName();
    s2 = src2->getName();
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    printf("  %s = icmp %s i32 %s, %s\n", s.c_str(), op.c_str(), s1.c_str(), s2.c_str());
}

void CmpInstruction::reLabel()
{
    dst->reLabel();
}

void CmpInstruction::eraseFromParent()
{
    dst->removeDef(this);
    src1->removeUse(this);
    src2->removeUse(this);
    Instruction::eraseFromParent();
}

std::pair<char, int> CmpInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> res, val1, val2;
    if (value.find(src1) == value.end())
        val1 = src1->getInitLatticeValue();
    else
        val1 = value[src1];
    if (value.find(src2) == value.end())
        val2 = src2->getInitLatticeValue();
    else
        val2 = value[src2];

    if (val1.first == -1 || val2.first == -1)
        res = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res.first = 0;
        switch (opcode)
        {
        case E:
            res.second = val1.second == val2.second;
            break;
        case NE:
            res.second = val1.second != val2.second;
            break;
        case L:
            res.second = val1.second < val2.second;
            break;
        case GE:
            res.second = val1.second >= val2.second;
            break;
        case G:
            res.second = val1.second > val2.second;
            break;
        case LE:
            res.second = val1.second <= val2.second;
            break;
        default:
            break;
        }
    }
    else
        res.first = 1;
    return res;
}

void CmpInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

void CmpInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src1 == o)
    {
        src1->removeUse(this);
        src1 = n;
        src1->addUse(this);
    }
    if (src2 == o)
    {
        src2->removeUse(this);
        src2 = n;
        src2->addUse(this);
    }
}

BranchInstruction::BranchInstruction(Instruction *to, BasicBlock *insert_bb) : Instruction(BRANCH, insert_bb)
{
    if (to != nullptr)
        setBranch(to);
}

BranchInstruction::BranchInstruction(BasicBlock *to) : Instruction(BRANCH)
{
    branch = to;
}

Operand *BranchInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    default:
        return nullptr;
    }
}

int BranchInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    default:
        return -1;
    }
    return -1;
}

Instruction* BranchInstruction::copy(){
    return new BranchInstruction(this->branch);
}

Instruction *BranchInstruction::copy(BasicBlock *insert_bb)
{
    BranchInstruction *new_instr = new BranchInstruction(this->branch);
    new_instr->setParent(insert_bb);
    insert_bb->insert(this);
    return new_instr;
}

void BranchInstruction::print() const
{
    printf("  br label %%%d\n", branch->getNo());
}

void BranchInstruction::setBranch(Instruction *inst)
{
    BasicBlock *target = inst->getParent();
    if (inst != target->begin())
        branch = target->split(inst);
    else
        branch = target;
    parent->split(this->getNext());
}

void BranchInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *BranchInstruction::getBranch()
{
    return branch;
}

IfInstruction::IfInstruction(Instruction *branch, Operand *res, BasicBlock *insert_bb) : BranchInstruction(nullptr, insert_bb)
{
    this->res = res;
    this->kind = IF;
    false_branch = nullptr;
    if (branch != nullptr)
        setBranch(branch);
    res->addUse(this);
}

IfInstruction::IfInstruction(BasicBlock *true_branch, BasicBlock *false_branch, Operand *res) : BranchInstruction(true_branch)
{
    this->res = res;
    this->kind = IF;
    this->false_branch = false_branch;
    res->addUse(this);
}

Operand *IfInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return res;
    default:
        return nullptr;
    }
}

int IfInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        res->removeUse(this); oper->addUse(this);
        res = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* IfInstruction::copy(){
    return new IfInstruction(this->branch, this->false_branch, this->res);
}

Instruction *IfInstruction::copy(BasicBlock *insert_bb)
{
    IfInstruction *new_instr = new IfInstruction(this->branch, this->false_branch, this->res);
    new_instr->setParent(insert_bb);
    insert_bb->insert(this);
    return new_instr;
}

void IfInstruction::print() const
{
    std::string s = res->getName();
    int true_label = branch->getNo();
    int false_label = false_branch->getNo();
    printf("  br i1 %s, label %%%d, label %%%d\n", s.c_str(), true_label, false_label);
}

void IfInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *IfInstruction::getFalseBranch()
{
    return false_branch;
}

void IfInstruction::eraseFromParent()
{
    res->removeUse(this);
    Instruction::eraseFromParent();
}

void IfInstruction::replaceUse(Operand *o, Operand *n)
{
    if (res == o)
    {
        res->removeUse(this);
        res = n;
        res->addUse(this);
    }
}

RetInstruction::RetInstruction(Operand *src) : Instruction(RET)
{
    this->src = src;
    retUndef = false;
    retVoid = false;
    if (src != nullptr)
        src->addUse(this);
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    this->src = src;
    retUndef = false;
    retVoid = false;
    if (src != nullptr)
        src->addUse(this);
}

Operand *RetInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return src;
    default:
        return nullptr;
    }
}

int RetInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        src->removeUse(this); oper->addUse(this);
        src = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* RetInstruction::copy(){
    RetInstruction *new_instr = new RetInstruction(src);
    new_instr->retUndef = this->getRetUndef();
    new_instr->retVoid = this->getRetVoid();
    return new_instr;
}

Instruction *RetInstruction::copy(BasicBlock *insert_bb)
{
    RetInstruction *new_instr = new RetInstruction(src, insert_bb);
    new_instr->retUndef = this->getRetUndef();
    new_instr->retVoid = this->getRetVoid();
    return new_instr;
}

void RetInstruction::print() const
{
    if (retUndef)
        printf("  ret i32 0\n");
    else if (retVoid)
        printf("  ret void\n");
    else if (src != nullptr)
    {
        std::string name, type;
        name = src->getName();
        type = src->getType();
        printf("  ret %s %s\n", type.c_str(), name.c_str());
    }
    else
    {
        Function *func = parent->getParent();
        if(isvoid(func->getSymPtr()->tp->operand))
            printf("  ret void\n");
        else
            printf("  ret i32 0\n");
            
    }
}

void RetInstruction::eraseFromParent()
{
    if (src != nullptr)
        src->removeUse(this);
    Instruction::eraseFromParent();
}

void RetInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src == o)
    {
        src->removeUse(this);
        src = n;
        src->addUse(this);
    }
}

CallInstruction::CallInstruction(Operand *dst, std::vector<Operand *> &param, symbol_entry *p, BasicBlock *insert_bb) : Instruction(CALL, insert_bb), params(param)
{
    for (auto &p : params)
        p->addUse(this);
    this->dst = dst;
    sym_ptr = p;
    callee = nullptr;
    if (dst)
        dst->addDef(this);
}

CallInstruction::CallInstruction(Operand *dst, std::vector<Operand *> &param, symbol_entry *p) : Instruction(CALL), params(param){
    for (auto &p : params)
        p->addUse(this);
    this->dst = dst;
    sym_ptr = p;
    callee = nullptr;
    if (dst)
        dst->addDef(this);
}

CallInstruction::CallInstruction(std::vector<Operand *> &param, symbol_entry *p, BasicBlock *insert_bb) : CallInstruction(nullptr, param, p, insert_bb)
{
}

std::vector<Operand *> CallInstruction::getOperands()
{
    std::vector<Operand *> v_ret;
    v_ret.push_back(dst);
    std::vector<Operand *> v_use = getUse();
    v_ret.insert(v_ret.end(), v_use.begin(), v_use.end());
    return v_ret;
}

Operand *CallInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    if (i == 0)
        return dst;
    else if (i > 0 && i < getOperandsNum())
    {
        return this->params[i - 1];
    }
    else
        return nullptr;
}

int CallInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }

    if (i == 0)
    {
        dst->removeDef(this); oper->addDef(this);
        this->dst = oper;
        return 0;
    }
    else if (i > 0 && i < getOperandsNum())
    {
        params[i - 1]->removeUse(this); oper->addUse(this);
        this->params[i - 1] = oper;
        return 0;
    }

    return -1;
}

Instruction* CallInstruction::copy(){
    return new CallInstruction(dst, params, sym_ptr);
}

Instruction *CallInstruction::copy(BasicBlock *insert_bb)
{
    return new CallInstruction(dst, params, sym_ptr, insert_bb);
}

void CallInstruction::print() const
{
    printf("  ");
    if (dst != nullptr)
    {
        std::string str = dst->getName();
        printf("%s = ", str.c_str());
    }
    printf("call %s ", type_system::getName(sym_ptr->tp).c_str());
    if (sym_ptr->undefined)
    {
        printf("bitcast (i32 (...)* @%s to i32 (", sym_ptr->name.c_str());
        if (params.empty())
            printf(")*)");
        else
        {
            std::string s;
            s = params[0]->getName();
            printf("%s", params[0]->getType().c_str());
            for (size_t i = 1; i < params.size(); i++)
            {
                s = params[i]->getName();
                printf(", %s", params[i]->getType().c_str());
            }
            printf(")*)");
        }
    }
    else
        printf("@%s", sym_ptr->name.c_str());

    if (params.empty())
        printf("()\n");
    else
    {
        std::string s;
        s = params[0]->getName();
        printf("(%s %s", params[0]->getType().c_str(), s.c_str());
        for (size_t i = 1; i < params.size(); i++)
        {
            s = params[i]->getName();
            printf(", %s %s", params[i]->getType().c_str(), s.c_str());
        }
        printf(")\n");
    }
}

void CallInstruction::reLabel()
{
    if (dst != nullptr)
        dst->reLabel();
}

void CallInstruction::eraseFromParent()
{
    if (dst != nullptr)
        dst->removeDef(this);
    for (auto &param : params)
        param->removeUse(this);
    Function *func = parent->getParent();
    func->getSuccs()[callee].erase(this);
    callee->getPreds()[func].erase(this);
    Instruction::eraseFromParent();
}

std::vector<Operand *> CallInstruction::getUse()
{
    return params;
}

void CallInstruction::replaceDef(Operand *o)
{
    if (dst != nullptr)
        dst->removeDef(this);
    dst = o;
    dst->addDef(this);
}

void CallInstruction::replaceUse(Operand *o, Operand *n)
{
    for (size_t i = 0; i < params.size(); i++)
        if (params[i] == o)
        {
            params[i]->removeUse(this);
            params[i] = n;
            params[i]->addUse(this);
        }
}

PhiInstruction::PhiInstruction(Operand *dst, int num_src) : Instruction(PHI)
{
    this->dst = dst;
    dst->addDef(this);
    _rn_ = dst;
}

Operand *PhiInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return _rn_;
    case 1:
        return dst;
    default:
        return nullptr;
    }
}

int PhiInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    case 1:
        _rn_ = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* PhiInstruction::copy(){
    PhiInstruction *new_instr = new PhiInstruction(this->dst, this->src.size());
    for (auto it = this->src.begin(); it != this->src.end(); it++)
    {
        new_instr->addSrc(it->second, it->first);
    }
    return new_instr;
}

Instruction *PhiInstruction::copy(BasicBlock *insert_bb)
{
    PhiInstruction *new_instr = new PhiInstruction(this->dst, this->src.size());
    for (auto it = this->src.begin(); it != this->src.end(); it++)
    {
        new_instr->addSrc(it->second, it->first);
    }

    new_instr->setParent(insert_bb);
    insert_bb->insert(new_instr);
    return new_instr;
}

void PhiInstruction::print() const
{
    std::vector<std::pair<BasicBlock *, Operand *>> v(src.begin(), src.end());
    std::string s = dst->getName();
    std::string t = dst->getType();
    int bb;
    printf("  %s = phi %s ", s.c_str(), t.c_str());
    if (v.empty())
        return;
    s = v[0].second->getName();
    bb = v[0].first->getNo();
    printf("[ %s, %%%d ]", s.c_str(), bb);
    for (size_t i = 1; i < src.size(); i++)
    {
        s = v[i].second->getName();
        bb = v[i].first->getNo();
        printf(", [ %s, %%%d ]", s.c_str(), bb);
    }
    printf("\n");
}

void PhiInstruction::reLabel()
{
    dst->reLabel();
}

void PhiInstruction::eraseFromParent()
{
    dst->removeDef(this);
    for (auto &s : src)
        s.second->removeUse(this);
    Instruction::eraseFromParent();
}

std::vector<Operand *> PhiInstruction::getUse()
{
    std::vector<Operand *> res;
    for (auto &t : src)
        res.push_back(t.second);
    return res;
}

std::pair<char, int> PhiInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> res, tmp;
    res = {1, 0};
    for (auto i = src.begin(); i != src.end(); i++)
    {
        if (value.find((*i).second) == value.end())
            tmp = (*i).second->getInitLatticeValue();
        else
            tmp = value[(*i).second];
        if (res.first > tmp.first)
            res = tmp;
        else if (res.first == tmp.first)
        {
            if (res.first == 0)
            {
                if (res.second != tmp.second)
                    res = {-1, 0};
            }
        }
    }
    return res;
}

Operand *PhiInstruction::getSrc(BasicBlock *bb)
{
    if (src.find(bb) != src.end())
        return src[bb];
    return nullptr;
}

void PhiInstruction::addSrc(Operand *op, BasicBlock *bb)
{
    src.insert({bb, op});
    op->addUse(this);
}

void PhiInstruction::removeSrc(BasicBlock *bb)
{
    src[bb]->removeUse(this);
    src.erase(bb);
}

void PhiInstruction::replaceDef(Operand *o)
{
    dst->removeDef(this);
    dst = o;
    dst->addDef(this);
}

void PhiInstruction::replaceUse(Operand *o, Operand *n)
{
    for (auto &t : src)
        if (t.second == o)
        {
            src[t.first]->removeUse(this);
            src[t.first] = n;
            n->addUse(this);
        }
}

AddrInstruction::AddrInstruction(Operand *dst, Operand *base, Operand *index, BasicBlock *insert_bb) : Instruction(ADDR, insert_bb)
{
    this->dst = dst;
    this->base = base;
    this->index = index;
    dst->addDef(this);
    base->addUse(this);
    index->addUse(this);
}

AddrInstruction::AddrInstruction(Operand *dst, Operand *base, Operand *index) : Instruction(ADDR)
{
    this->dst = dst;
    this->base = base;
    this->index = index;
    dst->addDef(this);
    base->addUse(this);
    index->addUse(this);
}

Operand *AddrInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return base;
    case 2:
        return index;
    default:
        return nullptr;
    }
}

int AddrInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    case 1:
        base->removeUse(this); oper->addUse(this);
        base = oper;
        return 0;
    case 2:
        index->removeUse(this); oper->addUse(this);
        index = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* AddrInstruction::copy(){
    return new AddrInstruction(this->dst, this->base, this->index);
}

Instruction *AddrInstruction::copy(BasicBlock *insert_bb)
{
    return new AddrInstruction(this->dst, this->base, this->index, insert_bb);
}

void AddrInstruction::print() const
{
    std::string s = dst->getName();
    std::string s1 = base->getName();
    std::string s2 = index->getName();
    std::string src_type, dst_type;
    if (base->isTemp())
    {
        src_type = base->getType();
        dst_type = "i32";
    }
    else
    {
        dst_type = base->getType();
        src_type = dst_type + "*";
    }
    if (base->isArray())
        printf("  %s = getelementptr inbounds %s, %s %s, i32 0, i32 %s\n", s.c_str(), dst_type.c_str(), src_type.c_str(), s1.c_str(), s2.c_str());
    else
        printf("  %s = getelementptr inbounds %s, %s %s, i32 %s\n", s.c_str(), dst_type.c_str(), src_type.c_str(), s1.c_str(), s2.c_str());
}

void AddrInstruction::reLabel()
{
    dst->reLabel();
}

void AddrInstruction::eraseFromParent()
{
    dst->removeDef(this);
    base->removeUse(this);
    index->removeUse(this);
    Instruction::eraseFromParent();
}

// replace dst by op
void AddrInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

void AddrInstruction::replaceUse(Operand *o, Operand *n)
{
    if (o == base)
    {
        base->removeUse(this);
        base = n;
        base->addUse(this);
    }
    if (o == index)
    {
        index->removeUse(this);
        index = n;
        index->addUse(this);
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    this->dst = dst;
    dst->addDef(this);
}

AllocaInstruction::AllocaInstruction(Operand *dst) : Instruction(ALLOCA)
{
    this->dst = dst;
    dst->addDef(this);
}

bool AllocaInstruction::isPromotable()
{
    return !dst->isArray();
}

Operand *AllocaInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    default:
        return nullptr;
    }
}

int AllocaInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* AllocaInstruction::copy(){
    return new AllocaInstruction(this->dst);
}

Instruction *AllocaInstruction::copy(BasicBlock *insert_bb)
{
    return new AllocaInstruction(this->dst, insert_bb);
}

void AllocaInstruction::print() const
{
    std::string s = dst->getName();
    std::string t = dst->getType();
    printf("  %s = alloca %s, align 4\n", s.c_str(), t.c_str());
}

void AllocaInstruction::reLabel()
{
    dst->reLabel();
}

void AllocaInstruction::eraseFromParent()
{
    dst->removeDef(this);
    Instruction::eraseFromParent();
}

void AllocaInstruction::replaceDef(Operand *op)
{
    dst->removeDef(this);
    dst = op;
    dst->addDef(this);
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    this->dst = dst;
    this->src_addr = src_addr;
    dst->addDef(this);
    src_addr->addUse(this);
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr) : Instruction(LOAD)
{
    this->dst = dst;
    this->src_addr = src_addr;
    dst->addDef(this);
    src_addr->addUse(this);
}

Operand *LoadInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst;
    case 1:
        return src_addr;
    default:
        return nullptr;
    }
}

int LoadInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst->removeDef(this); oper->addDef(this);
        dst = oper;
        return 0;
    case 1:
        src_addr->removeUse(this); oper->addUse(this);
        src_addr = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* LoadInstruction::copy(){
    return new LoadInstruction(this->dst, this->src_addr);
}

Instruction *LoadInstruction::copy(BasicBlock *insert_bb)
{
    return new LoadInstruction(this->dst, this->src_addr, insert_bb);
}

void LoadInstruction::print() const
{
    std::string s1 = dst->getName();
    std::string s2 = src_addr->getName();
    std::string src_type;
    std::string dst_type;
    if (src_addr->isTemp())
    {
        src_type = src_addr->getType();
        dst_type = "i32";
    }
    else
    {
        dst_type = dst->getType();
        src_type = dst_type + "*";
    }
    printf("  %s = load %s, %s %s, align 4\n", s1.c_str(), dst_type.c_str(), src_type.c_str(), s2.c_str());
}

void LoadInstruction::reLabel()
{
    dst->reLabel();
}

void LoadInstruction::eraseFromParent()
{
    dst->removeDef(this);
    src_addr->removeUse(this);
    Instruction::eraseFromParent();
}

std::pair<char, int> LoadInstruction::getLatticeValue(std::map<Operand *, std::pair<char, int>> &value)
{
    std::pair<char, int> l;
    if (value.find(src_addr) == value.end())
        l = src_addr->getInitLatticeValue();
    else
        l = value[src_addr];
    return l;
}

void LoadInstruction::replaceDef(Operand *o)
{
    dst->removeDef(this);
    dst = o;
    dst->addDef(this);
}

void LoadInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src_addr == o)
    {
        src_addr->removeUse(this);
        src_addr = n;
        src_addr->addUse(this);
    }
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    this->dst_addr = dst_addr;
    this->src = src;
    dst_addr->addDef(this);
    src->addUse(this);
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src) : Instruction(STORE)
{
    this->dst_addr = dst_addr;
    this->src = src;
    dst_addr->addDef(this);
    src->addUse(this);
}

Operand *StoreInstruction::getOperandByIndex(int i)
{
    if (i >= this->getOperandsNum())
    {
        return nullptr;
    }
    switch (i)
    {
    case 0:
        return dst_addr;
    case 1:
        return src;
    default:
        return nullptr;
    }
}

int StoreInstruction::setOperandByIndex(int i, Operand *oper)
{
    if (i >= this->getOperandsNum())
    {
        return -1;
    }
    switch (i)
    {
    case 0:
        dst_addr->removeUse(this); oper->addUse(this);
        dst_addr = oper;
        return 0;
    case 1:
        src->removeUse(this); oper->addUse(this);
        src = oper;
        return 0;
    default:
        return -1;
    }
    return -1;
}

Instruction* StoreInstruction::copy(){
    return new StoreInstruction(this->dst_addr, this->src);
}

Instruction *StoreInstruction::copy(BasicBlock *insert_bb)
{
    return new StoreInstruction(this->dst_addr, this->src, insert_bb);
}

void StoreInstruction::print() const
{
    std::string s1 = dst_addr->getName();
    std::string s2 = src->getName();
    std::string t1 = dst_addr->getType();
    std::string t2 = src->getType();

    std::string src_type;
    std::string dst_type;
    if (dst_addr->isTemp())
    {
        dst_type = dst_addr->getType();
        src_type = "i32";
    }
    else
    {
        src_type = src->getType();
        dst_type = src_type + "*";
    }
    printf("  store %s %s, %s %s, align 4\n", src_type.c_str(), s2.c_str(), dst_type.c_str(), s1.c_str());
}

void StoreInstruction::eraseFromParent()
{
    dst_addr->removeDef(this);
    src->removeUse(this);
    Instruction::eraseFromParent();
}

void StoreInstruction::replaceDef(Operand *op)
{
    dst_addr->removeDef(this);
    dst_addr = op;
    dst_addr->addDef(this);
}

void StoreInstruction::replaceUse(Operand *o, Operand *n)
{
    if (src == o)
    {
        src->removeUse(this);
        src = n;
        src->addUse(this);
    }
}

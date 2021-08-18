#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "operand.h"
#include "PartialRedundancyElimination.h"
#include <vector>
#include <map>

class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned kind, BasicBlock *insert_bb);
    Instruction(unsigned kind);
    virtual ~Instruction() = default;
    virtual std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&){return {-1,0};};
    bool isBinary() const;
    bool isUnary() const;
    bool isBranch() const;
    bool isCondBr() const;
    bool isUncondBr() const;
    bool isPhi() const;
    bool isRet() const;
    bool isCall() const;
    bool isAlloca() const;
    bool isLoad() const;
    bool isStore() const;
    bool isCmp() const;
    bool isCopy() const;
    bool isAddr() const;
    bool isMarked();
    void setMarked();
    void clearMarked();
    BasicBlock *getParent();
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    virtual void eraseFromParent();
    void removeFromParent();
    void insertBefore(Instruction *);
    bool iscritical();
    Instruction *getNext() const;
    Instruction *getPrev() const;
    // if u call this function without a param insert_bb, remember to set parent relation between the instruction and that bb
    virtual Instruction* copy() = 0;
    virtual Instruction* copy(BasicBlock*) = 0;
    virtual void reLabel() = 0;
    virtual void print() const = 0;
    virtual Operand* getDef() = 0;
    virtual void replaceDef(Operand*) = 0;
    virtual void replaceUse(Operand*, Operand*) = 0;
    virtual std::vector<Operand*> getUse() = 0;
    // for loop unrolling, only return operands whose origin type is Operand*, exclude types like vector<Operand*>..!
    virtual std::vector<Operand*> getOperands() = 0;
    virtual int getOperandsNum() = 0;
    virtual Operand* getOperandByIndex(int) = 0;
    virtual int setOperandByIndex(int, Operand*) = 0;
    int getKind() {return kind;};
    int getOP() {return opcode;};
    void set_vec_flag(bool flag){vec_flag = flag;};
    bool get_vec_flag(){return vec_flag;};
protected:
    bool vec_flag; // for vectorization
    bool marked; // for useless code elimination
    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    BasicBlock *parent;
    enum {BINARY, UNARY, COPY, BRANCH, IF, RET, CALL, LOAD, STORE, CMP, ADDR, ALLOCA, PHI};
    unsigned kind;
};

class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1){};
    Instruction* copy(){return nullptr;};
    Instruction* copy(BasicBlock*){return nullptr;};
    void print() const {};
    void reLabel(){};
    Operand* getDef() {return nullptr;};
    std::vector<Operand*> getUse() {return std::vector<Operand*>();};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>();};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int){return nullptr;};
    int setOperandByIndex(int, Operand*){return -1;};
    void replaceDef(Operand*){};
    void replaceUse(Operand*, Operand*){};
};

class AddrInstruction : public Instruction
{
public:
    AddrInstruction(Operand *dst, Operand *base, Operand *index, BasicBlock *);
    AddrInstruction(Operand *dst, Operand *base, Operand *index);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::vector<Operand*> getUse() {return std::vector<Operand*>({base, index});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, base, index});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst, *base, *index;
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst, BasicBlock *);
    AllocaInstruction(Operand *dst);
    bool isPromotable();
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::vector<Operand*> getUse() {return std::vector<Operand*>({});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*){};
protected:
    Operand *dst;
};

class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *);
    LoadInstruction(Operand *dst, Operand *src_addr);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src_addr});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, src_addr});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst, *src_addr;
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *);
    StoreInstruction(Operand *dst_addr, Operand *src);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel(){};
    void eraseFromParent();
    Operand* getDef() {return nullptr;};
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src, dst_addr});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst_addr, src});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst_addr, *src;
};

class CopyInstruction : public Instruction
{
public:
    CopyInstruction(Operand *dst, Operand *src, BasicBlock *);
    CopyInstruction(Operand *dst, Operand *src);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, src});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst, *src;
};

class UnaryInstruction : public Instruction
{
public:
    UnaryInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *);
    UnaryInstruction(unsigned opcode, Operand *dst, Operand *src);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, src});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
    enum {UMINUS, NOT};
protected:
    Operand *dst, *src;
};

class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *);
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    enum {MINUS, ADD, MUL, DIV, MOD, AND, OR, LSL, LSR, ASR};
    Operand* getDef() {return dst;};
    void eraseFromParent();
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src1, src2});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, src1, src2});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst, *src1, *src2;
};

class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *);
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    enum {E, NE, L, GE, G, LE};
    Operand* getDef() {return dst;};
    void eraseFromParent();
    std::vector<Operand*> getUse() {return std::vector<Operand*>({src1, src2});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({dst, src1, src2});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
protected:
    Operand *dst, *src1, *src2;
};

class BranchInstruction : public Instruction
{
public:
    BranchInstruction(Instruction *to, BasicBlock *);
    BranchInstruction(BasicBlock*);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel(){};
    void setBranch(Instruction *);
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    Operand* getDef() {return nullptr;};
    std::vector<Operand*> getUse() {return std::vector<Operand*>();};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*){};
    void replaceUse(Operand*, Operand*){};
protected:
    BasicBlock *branch;
};

class IfInstruction : public BranchInstruction
{
public:
    IfInstruction(Instruction *, Operand *, BasicBlock *);
    IfInstruction(BasicBlock*, BasicBlock*, Operand *);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel() {};
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    Operand* getDef() {return nullptr;};
    void eraseFromParent();
    std::vector<Operand*> getUse() {return std::vector<Operand*>({res});};
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({res});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*) {};
    void replaceUse(Operand*, Operand*);
protected:
    Operand *res;
    BasicBlock *false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src);
    RetInstruction(Operand *src, BasicBlock *insert_bb);
    Instruction* copy();
    Instruction* copy(BasicBlock*);    
    void print() const;
    void reLabel() {};
    void eraseFromParent();
    void setRetVoid() {retVoid = true;};
    void setRetUndef() {retUndef = true;};
    bool getRetVoid() {return this->retVoid;};
    bool getRetUndef() {return this->retUndef;};
    Operand* getDef() {return nullptr;};
    std::vector<Operand*> getUse() {return src == nullptr ? std::vector<Operand*>() : std::vector<Operand*>({src});};
    std::vector<Operand*> getOperands() {return src == nullptr ? std::vector<Operand*>() : std::vector<Operand*>({src});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    void replaceDef(Operand*) {};
    void replaceUse(Operand*, Operand*);
protected:
    Operand *src;
    bool retVoid;
    bool retUndef;
};

class CallInstruction : public Instruction
{
public:
    CallInstruction(Operand *dst, std::vector<Operand *> &param, symbol_entry *p, BasicBlock *insert_bb);
    CallInstruction(Operand *dst, std::vector<Operand *> &param, symbol_entry *p);
    CallInstruction(std::vector<Operand *> &param, symbol_entry *p, BasicBlock *insert_bb);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    void eraseFromParent();
    Operand* getDef() {return dst;};
    std::vector<Operand*> getUse();
    std::vector<Operand*> getOperands();
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    symbol_entry * getSymPtr() {return sym_ptr;};
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
    void setCallee(Function*f) {callee = f;};
    Function* getCallee() {return callee;};

protected:
    std::vector<Operand*> params;
    symbol_entry *sym_ptr;
    Function *callee;
    Operand *dst;
};

class PhiInstruction : public Instruction
{
public:
    PhiInstruction(Operand *dst, int num_src);
    Instruction* copy();
    Instruction* copy(BasicBlock*);
    void print() const;
    void reLabel();
    Operand* getRename() {return _rn_;};
    Operand* getDef() {return dst;};
    void eraseFromParent();
    std::vector<Operand*> getUse();
    std::vector<Operand*> getOperands() {return std::vector<Operand*>({_rn_, dst});};
    int getOperandsNum() {return this->getOperands().size();};
    Operand* getOperandByIndex(int);
    int setOperandByIndex(int, Operand*);
    std::pair<char, int> getLatticeValue(std::map<Operand*, std::pair<char, int>>&);
    Operand* getSrc(BasicBlock*);
    void addSrc(Operand*, BasicBlock*);
    void removeSrc(BasicBlock*);
    void replaceDef(Operand*);
    void replaceUse(Operand*, Operand*);
    std::map<BasicBlock*, Operand*>::iterator getSrcBegin(){ return src.begin(); };
    std::map<BasicBlock*, Operand*>::iterator getSrcEnd(){ return src.end(); };
private:
    Operand *_rn_;
    Operand *dst;
    std::map<BasicBlock*, Operand*> src;
};

#endif
#ifndef __OPERAND_H__
#define __OPERAND_H__

#include "symtab.h"
#include <vector>

class Instruction;
class Function;

class Operand
{
protected:
    enum
    {
        VARIABLE,
        CONSTANT,
        TEMPORARY
    };
    int type;
    std::vector<Instruction *> def, use;

public:
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
    virtual void reLabel() = 0;
    virtual Operand* copy() = 0;
    bool operator <(const Operand&) const;
    bool isVar() const { return type == VARIABLE; };
    bool isConstant() const { return type == CONSTANT; };
    bool isTemp() const { return type == TEMPORARY; };
    virtual bool isSSAName() const = 0;
    virtual bool isArray() const = 0;
    virtual bool isPtr() const = 0;
    virtual std::pair<char, int> getInitLatticeValue() = 0;
    void addDef(Instruction *inst) { def.push_back(inst); };
    void addUse(Instruction *inst) { use.push_back(inst); };
    void removeDef(Instruction *inst);
    void removeUse(Instruction *inst);
    std::vector<Instruction *> &getDef() { return def; };
    std::vector<Instruction *> &getUse() { return use; };
};

class Variable : public Operand
{
private:
    symbol_entry *sym_ptr;

public:
    Variable(symbol_entry *sym_ptr);
    Variable(Variable&);
    std::string getName() const;
    std::string getType() const;
    bool operator <(const Variable&) const;
    Operand* copy();
    bool isSSAName() const;
    bool isArray() const;
    bool isPtr() const;
    void reLabel(){};
    std::pair<char, int> getInitLatticeValue();
    symbol_entry *getSymPtr() const { return sym_ptr; };
};

class Constant : public Operand
{
private:
    int value;

public:
    Constant(int);
    Constant(Constant&);
    std::string getName() const;
    std::string getType() const;
    Operand* copy();
    bool operator <(const Constant&a) const {return value < a.value;};
    bool isSSAName() const {return false;};
    std::pair<char, int> getInitLatticeValue();
    bool isArray() const {return false;};
    bool isPtr() const {return false;};
    void reLabel(){};
    int getValue() const { return value; };
    void setValue(int newval) { this->value = newval; };
};

class Temporary : public Operand
{
private:
    symbol_entry *sym_ptr;

public:
    Temporary(symbol_entry *);
    Temporary(const Temporary&);
    std::string getName() const;
    std::string getType() const;
    Operand* copy();
    bool operator <(const Temporary&a) const {return sym_ptr->label < a.sym_ptr->label;};
    std::pair<char, int> getInitLatticeValue();
    bool isSSAName() const;
    bool isArray() const;
    bool isPtr() const;
    void reLabel();
    int getNo() const { return sym_ptr->label; };
    symbol_entry *getSymPtr() const { return sym_ptr; };
    
};

#endif
#include "operand.h"
#include "function.h"
#include <sstream>
#include <string.h>

Variable::Variable(symbol_entry *p)
{
    sym_ptr = p;
    type = VARIABLE;
}

Variable::Variable(Variable &var)
{
    sym_ptr = new symbol_entry(*var.sym_ptr);
    sym_ptr->label = symbol_table::gen_label(1);
    type = VARIABLE;
}

std::string Variable::getName() const
{
    std::ostringstream buffer;
    if (sym_ptr->scope >= PARAM)
    {
        if (isconst(sym_ptr->tp))
            buffer << "@" << sym_ptr->name << "." << sym_ptr->label;
        else
            buffer << "%" << sym_ptr->name << sym_ptr->label;
    }
    else
        buffer << "@" << sym_ptr->name;
    return buffer.str();
}

std::string Variable::getType() const
{
    return type_system::getName(sym_ptr->tp);
}

bool Variable::operator <(const Variable&a) const
{
    if(sym_ptr->name < a.sym_ptr->name)
        return true;
    if(sym_ptr->scope < a.sym_ptr->scope)
        return true;
    return false;
}

Operand *Variable::copy()
{
    return new Variable(*this);
}

bool Variable::isSSAName() const
{
    if(sym_ptr->scope == GLOBAL)
        return false;
    if(isArray())
        return false;
    return true;
}

bool Variable::isArray() const
{
    return isarray(sym_ptr->tp);
}

bool Variable::isPtr() const
{
    return isptr(sym_ptr->tp);
}

std::pair<char, int> Variable::getInitLatticeValue() 
{
    std::pair<char, int> res;
    if(isconst(sym_ptr->tp))
        res = {0, sym_ptr->constant};
    else
        res = {-1, 0};
    return res;
}

Constant::Constant(int v)
{
    value = v;
    type = CONSTANT;
}

Constant::Constant(Constant &cst)
{
    value = cst.value;
    type = CONSTANT;
}

std::string Constant::getName() const
{
    std::ostringstream buffer;
    buffer << value;
    return buffer.str();
}

std::string Constant::getType() const
{
    return "i32";
}

Operand *Constant::copy()
{
    return new Constant(*this);
}

std::pair<char, int> Constant::getInitLatticeValue() 
{
    return {0, value};
}

Temporary::Temporary(symbol_entry *s)
{
    this->sym_ptr = s;
    type = TEMPORARY;
}

Temporary::Temporary(const Temporary &temp)
{
    sym_ptr = temp.sym_ptr;
    type = TEMPORARY;
}

std::string Temporary::getName() const
{
    std::ostringstream buffer;
    buffer << "%" << sym_ptr->label;
    return buffer.str();
}

std::string Temporary::getType() const
{
    return type_system::getName(sym_ptr->tp);
}

Operand *Temporary::copy()
{
    return new Temporary(*this);
}

std::pair<char, int> Temporary::getInitLatticeValue() 
{
    if(sym_ptr->scope == PARAM)
        return {-1, 0};
    return {1, 0};
}

bool Temporary::isSSAName() const
{
    return true;
}

bool Temporary::isArray() const
{
    return isarray(sym_ptr->tp);
}

bool Temporary::isPtr() const
{
    return isptr(sym_ptr->tp);
}

void Temporary::reLabel()
{
    sym_ptr->label = def[0]->getParent()->getParent()->getLabel(sym_ptr->label);
}

void Operand::removeDef(Instruction *inst)
{
    auto i = std::find(def.begin(), def.end(), inst);
    def.erase(i);
}

void Operand::removeUse(Instruction *inst)
{
    auto i = std::find(use.begin(), use.end(), inst);
    if(i == use.end())
        return;
    use.erase(i);
}
bool Operand::operator <(const Operand&a) const
{
    if(this->type != a.type)
        return this->type < a.type;
    if(a.type == VARIABLE)
        return dynamic_cast<const Variable*>(this)->operator<(dynamic_cast<const Variable&>(a));
    else if(a.type == CONSTANT)
        return dynamic_cast<const Constant*>(this)->operator<(dynamic_cast<const Constant&>(a));
    else
        return dynamic_cast<const Temporary*>(this)->operator<(dynamic_cast<const Temporary&>(a));
}

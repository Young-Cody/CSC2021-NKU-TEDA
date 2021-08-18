#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "function.h"

class Unit
{
    typedef std::vector<Function *>::iterator iterator;
    typedef std::vector<Function *>::reverse_iterator reverse_iterator;

private:
    std::vector<Function *> func_list;
    symbol_table *global_decl;
    symbol_table *func_decl;
    void printGlobalDecl();
    void printInitVal(int *, type*, int);
    void printFuncDecl();
public:
    Unit();
    void insertFunc(Function *);
    void print();
    void setGlobal(symbol_table*st) {global_decl = st;};
    void setFuncDecl(symbol_table*st) {func_decl = st;};
    Operand *getConstant(int value);
    Operand *getTemp(symbol_entry *);
    Operand *getVar(symbol_entry *);
    Function *getInsertPoint();
    iterator begin() { return func_list.begin(); };
    iterator end() { return func_list.end(); };
    reverse_iterator rbegin() { return func_list.rbegin(); };
    reverse_iterator rend() { return func_list.rend(); };
    symbol_table * getGlobal() {return global_decl;};
};

#endif
#ifndef __SYMTAB_H__
#define __SYMTAB_H__
#include "typesys.h"
#include <iostream>
using namespace std;
#define HASH_SIZE 256
enum
{
    CONSTANT = 1,
    LABEL,
    GLOBAL,
    PARAM,
    LOCAL
};

struct symbol_entry
{
    string name;
    int scope;
    int lineno;
    type *tp;
    symbol_entry *prev;
    bool str;
    bool undefined;
    int *array_value;
    int offset;
    int label;
    int constant;
};

struct hash_entry
{
    symbol_entry sym;
    hash_entry *next;
};

class symbol_table
{
private:
    hash<string> f;
public:
    symbol_table(int lvl);
    symbol_table *new_table(int lvl);
    symbol_entry *install(string name);
    symbol_entry *lookup(string name);
    symbol_entry *find_label(int lbl);
    symbol_entry *find_constant(int cst);
    static void enter_scope();
    static void leave_scope();
    static int gen_label(int n);
    static symbol_entry *gen_temp(type *tp);
    static int level;
    static int label;
    int table_level;
    symbol_table *prev;
    hash_entry *buckets[HASH_SIZE];
    symbol_entry *all;
};

extern symbol_table *identifiers;
extern symbol_table *globals;
extern symbol_table *constants;
extern symbol_table *labels;
extern symbol_table *types;
extern symbol_table *functions;

#endif
#include "symtab.h"
#include <sstream>
#include <memory.h>
#define leave_scope() symbol_table::leave_scope()
#define enter_scope() symbol_table::enter_scope()

static symbol_table t[5] = {GLOBAL, GLOBAL, GLOBAL, GLOBAL, CONSTANT};
symbol_table *identifiers = &t[0];
symbol_table *globals = &t[0];
symbol_table *constants = &t[4];
symbol_table *types = &t[1];
symbol_table *labels = &t[2];
symbol_table *functions = &t[3];

int symbol_table::level = GLOBAL;
int symbol_table::label = 0;

symbol_table::symbol_table(int lvl)
{
    this->table_level = lvl;
    all = NULL;
    prev = NULL;
    memset(buckets, 0, sizeof(hash_entry*) * HASH_SIZE);
}

symbol_table *symbol_table::new_table(int lvl)
{
    symbol_table *t;
    t = new symbol_table(lvl);
    t->prev = this;
    t->all = all;
    return t;
}

symbol_entry *symbol_table::install(string name)
{
    hash_entry *he = new hash_entry();
    he->sym.name = name;
    int h = f(he->sym.name) & (HASH_SIZE - 1); 
    he->sym.array_value = NULL;
    he->sym.constant = 0;
    he->sym.label = 0;
    he->sym.prev = all;
    all = &he->sym;
    he->sym.scope = table_level;
    he->next = buckets[h];
    buckets[h] = he;
    return &he->sym;
}

symbol_entry *symbol_table::lookup(string name)
{
    int h = f(name) & (HASH_SIZE - 1);
    symbol_table *pt = this;
    while (pt != NULL)
    {
        hash_entry *he = pt->buckets[h];
        while (he != NULL)
        {
            if (he->sym.name == name)
                return &he->sym;
            he = he->next;
        }
        pt = pt->prev;
    }
    return NULL;
}

int symbol_table::gen_label(int n)
{
    label += n;
    return label - n;
}

symbol_entry *symbol_table::find_label(int lbl)
{
    int h = lbl & (HASH_SIZE - 1);
    hash_entry *he = this->buckets[h];
    while (he != NULL)
    {
        if (he->sym.label == lbl)
            return &he->sym;
        he = he->next;
    }
    he = new hash_entry;
    ostringstream os;
    os << lbl;
    he->sym.name = os.str();
    he->sym.scope = LABEL;
    he->sym.label = lbl;
    he->sym.prev = all;
    all = &he->sym;
    he->next = buckets[h];
    buckets[h] = he;
    return &he->sym;
}

symbol_entry *symbol_table::find_constant(int cst)
{
    int h = cst & (HASH_SIZE - 1);
    hash_entry *he = this->buckets[h];
    while (he != NULL)
    {
        if (he->sym.constant == cst)
            return &he->sym;
        he = he->next;
    }
    he = new hash_entry;
    ostringstream os;
    os << cst;
    he->sym.name = os.str();
    he->sym.scope = CONSTANT;
    he->sym.constant = cst;
    he->sym.prev = all;
    all = &he->sym;
    he->next = buckets[h];
    buckets[h] = he;
    return &he->sym;
}

symbol_entry *symbol_table::gen_temp(type *tp)
{
    symbol_entry *t = new symbol_entry;
    int lbl = gen_label(1);
    ostringstream os;
    os << "%" << lbl;
    t->name = os.str();
    t->scope = LABEL;
    t->label = lbl;
    t->tp = tp;
    return t;
}

void symbol_table::enter_scope()
{
    level++;
}

void symbol_table::leave_scope()
{
    if (types->table_level == level)
    {
        symbol_table *p = types;
        types = types->prev;
        delete p;
    }
    if (identifiers->table_level == level)
    {
        symbol_table *p = identifiers;
        identifiers = identifiers->prev;
        delete p;
    }
    level--;
}
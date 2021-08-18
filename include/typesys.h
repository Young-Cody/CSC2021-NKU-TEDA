#ifndef __TYPESYS_H__
#define __TYPESYS_H__
#define HASH_SIZE 256
#include <iostream>
#include <string>

enum
{
    CONST_T = 1,
    VOID_T,
    INT_T,
    ARRAY,
    POINTER,
    FUNCTION
};

#define unqual(t) ((t)->op == CONST_T ? (t)->operand : (t))
#define isint(t) (unqual(t)->op == INT_T)
#define isptr(t) (unqual(t)->op == POINTER)
#define isvoid(t) (unqual(t)->op == VOID_T)
#define isfunc(t) (unqual(t)->op == FUNCTION)
#define isarray(t) (unqual(t)->op == ARRAY)
#define init_type(s, type, name, op, align, size) \
    do                                            \
    {                                             \
        s = types->install(name);                 \
        type = find_type(op, 0, align, size);     \
        s->tp = type;                             \
    } while (0)

struct symbol_entry;

struct type
{
    int op;
    type *operand;
    int align;
    int size;
    symbol_entry **func_params;
};

struct entry
{
    type tp;
    entry *next;
};

bool isconst(type *tp);
class type_system
{
public:
    type_system();
    type *ptr(type *tp);
    type *deref(type *tp);
    type *array(type *tp, int n);
    type *func(type *tp, symbol_entry **params);
    type *a2p(type *tp);
    type *qual(type *tp);
    static std::string getName(type *);
    static bool type_equal(type *a, type *b);

    type *int_type;
    type *void_type;

private:
    type *find_type(int op, type *operand, int align, int size);
    entry *type_table[HASH_SIZE];
};

extern type_system typesys;

#endif
#include "typesys.h"
#include "symtab.h"
#include <iostream>
#include <sstream>
using namespace std;
#define INT_MAX 0x7fffffff
type_system typesys;

bool isconst(type *tp)
{
    if (tp == nullptr)
        return false;
    if (tp->op == CONST_T)
        return true;
    if (tp->op == ARRAY)
        return isconst(tp->operand);
    return false;
}

type_system::type_system()
{
    symbol_entry *s;
    init_type(s, int_type, "int", INT_T, 4, 4);
    init_type(s, void_type, "void", VOID_T, 0, 0);
}

type *type_system::find_type(int op, type *operand, int align, int size)
{
    int h = op & (HASH_SIZE - 1);
    entry *p;
    if (op != FUNCTION && (op != ARRAY || size > 0))
        for (p = type_table[h]; p; p = p->next)
        {
            if (p->tp.op == op && p->tp.operand == operand &&
                p->tp.align == align && p->tp.size == size)
                return &p->tp;
        }
    p = new entry;
    p->tp.op = op;
    p->tp.operand = operand;
    p->tp.align = align;
    p->tp.size = size;
    p->next = type_table[h];
    type_table[h] = p->next;
    return &p->tp;
}

type *type_system::ptr(type *tp)
{
    return find_type(POINTER, tp, 4, 4);
}

type *type_system::deref(type *tp)
{
    if (isptr(tp))
        tp = tp->operand;
    else
        fprintf(stderr, "type error: %s\n", "pointer expected");
    return tp;
}

type *type_system::array(type *tp, int n)
{
    if (isfunc(tp))
    {
        fprintf(stderr, "illegal type 'array of function'\n");
        return array(int_type, n);
    }
    if (isvoid(tp))
    {
        fprintf(stderr, "illegal type 'array of void'\n");
        return array(int_type, n);
    }

    if (isarray(tp) && tp->size == 0)
        fprintf(stderr, "missing array size\n");
    else if (n > INT_MAX / tp->size)
    {
        fprintf(stderr, "array size exceeds %d bytes", INT_MAX);
        n = 1;
    }

    return find_type(ARRAY, tp, tp->align, n * tp->size);
}

type *type_system::func(type *tp, symbol_entry **params)
{
    if (tp && tp->op == ARRAY)
        fprintf(stderr, "illegal return type 'array'\n");
    if (tp && tp->op == FUNCTION)
        fprintf(stderr, "illegal return type 'function'\n");
    type *t = new type;
    t->op = FUNCTION;
    t->operand = tp;
    t->func_params = params;
    return t;
}

type *type_system::a2p(type *tp)
{
    if (isarray(tp))
        return ptr(tp->operand);
    fprintf(stderr, "type error: %s\n", "array expected");
    return ptr(tp);
}

type *type_system::qual(type *tp)
{
    if (isarray(tp))
        tp = find_type(ARRAY, qual(tp->operand), tp->align, tp->size);
    else if (isfunc(tp))
        fprintf(stdout, "qualified function type ignored\n");
    else if (isconst(tp))
        fprintf(stderr, "type error\n");
    else
        tp = find_type(CONST_T, tp, tp->align, tp->size);
    return tp;
}

std::string type_system::getName(type *tp)
{
    if (tp == nullptr)
        return "";
    if (isint(tp))
        return "i32";
    if (isvoid(tp))
        return "void";
    if (isptr(tp))
    {
        return "i32*";
    }
    if (isarray(tp))
    {
        ostringstream buffer;
        buffer << "[" << tp->size / tp->align << " x i32]";
        return buffer.str();
    }
    return "";
}

bool type_system::type_equal(type *a, type *b)
{
    if (a == b)
        return true;
    if (a->op != b->op)
        return false;
    switch (a->op)
    {
    case CONST_T:
    case POINTER:
        return type_equal(a->operand, b->operand);
    case ARRAY:
    {
        if (type_equal(a->operand, b->operand))
        {
            if (a->size == b->size)
                return true;
            if (a->size == 0 || b->size == 0)
                return true;
        }
        return false;
    }
    case FUNCTION:
    {
        if (type_equal(a->operand, b->operand))
        {
            symbol_entry **i = a->func_params;
            symbol_entry **j = a->func_params;
            while (*i && *j)
            {
                if (!type_equal(unqual((*i)->tp), unqual((*j)->tp)))
                    return false;
                i++;
                j++;
            }
            if (!(*i) && !(*j))
                return true;
        }
        return false;
    }

    default:
        break;
    }
    return false;
}

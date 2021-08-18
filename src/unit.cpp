#include "unit.h"

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::print()
{
    printGlobalDecl();
    for (auto &i : func_list)
        i->print();
    printFuncDecl();
}

void Unit::printGlobalDecl()
{
    symbol_entry *se = globals->all;
    vector<symbol_entry *> data, rodata, bss;
    while (se)
    {
        if (se->str == 1)
            rodata.push_back(se);
        else if (isarray(se->tp))
        {
            if (se->array_value == NULL)
                bss.push_back(se);
            else
            {
                type *t = se->tp;
                while (isarray(t))
                    t = t->operand;
                if (isconst(t))
                    rodata.push_back(se);
                else
                {
                    int length = se->tp->size / se->tp->align;
                    int i;
                    for (i = 0; i < length; i++)
                    {
                        if (se->array_value[i])
                        {
                            data.push_back(se);
                            break;
                        }
                    }
                    if (i >= length)
                        bss.push_back(se);
                }
            }
        }
        else if (isconst(se->tp))
            rodata.push_back(se);
        else
        {
            if (se->constant == 0)
                bss.push_back(se);
            else
                data.push_back(se);
        }
        se = se->prev;
    }
    while (!data.empty())
    {
        se = data.back();
        data.pop_back();
        std::string t = type_system::getName(se->tp);
        printf("@%s = dso_local global %s ", se->name.c_str(), t.c_str());
        if (isarray(se->tp))
        {
            int length = se->tp->size / se->tp->align;
            printf("[i32 %d", se->array_value[0]);
            for (int i = 1; i < length; i++)
                printf(", i32 %d", se->array_value[i]);
            printf("]");
        }
        else
            printf("%d", se->constant);
        printf(", align %d\n", se->tp->align);
    }
    while (!rodata.empty())
    {
        se = rodata.back();
        rodata.pop_back();
        std::string t = type_system::getName(se->tp);
        printf("@%s = dso_local constant %s ", se->name.c_str(), t.c_str());
        if (isarray(se->tp))
        {
            int length = se->tp->size / se->tp->align;
            printf("[i32 %d", se->array_value[0]);
            for (int i = 1; i < length; i++)
                printf(", i32 %d", se->array_value[i]);
            printf("]");
        }
        else
            printf("%d", se->constant);
        printf(", align %d\n", se->tp->align);
    }
    while (!bss.empty())
    {
        se = bss.back();
        bss.pop_back();
        std::string t = type_system::getName(se->tp);
        printf("@%s = common dso_local global %s zeroinitializer, align %d\n", se->name.c_str(), t.c_str(), se->tp->align);
    }
    printf("\n");
}

void Unit::printInitVal(int *array_value, type *tp, int i)
{
    if (isint(tp))
        printf("%d", array_value[i]);
    else if (isarray(tp))
    {
        std::string t = type_system::getName(tp->operand);
        int j = 0;
        int len = tp->operand->size / tp->align;
        printf("[");
        while (j < tp->size / tp->align - len)
        {
            printf("%s ", t.c_str());
            printInitVal(array_value, tp->operand, i + j);
            printf(", ");
            j += len;
        }
        printf("%s ", t.c_str());
        printInitVal(array_value, tp->operand, i + j);
        printf("]");
    }
}

void Unit::printFuncDecl()
{
    symbol_entry *se = func_decl->all;
    while (se)
    {
        if (se->undefined)
            printf("declare dso_local i32 @%s(...) #1\n", se->name.c_str());
        se = se->prev;
    }
}

Operand *Unit::getConstant(int value)
{
    return new Constant(value);
}

Operand *Unit::getTemp(symbol_entry *se)
{
    static std::map<symbol_entry *, Operand *> s;
    auto t = s.find(se);
    Operand *res;
    if (t == s.end())
    {
        res = new Temporary(se);
        s[se] = res;
    }
    else
        res = t->second;
    return res;
}

Operand *Unit::getVar(symbol_entry *se)
{
    static std::map<symbol_entry *, Operand *> s;
    auto t = s.find(se);
    Operand *res;
    if (t == s.end())
    {
        res = new Variable(se);
        s[se] = res;
    }
    else
        res = t->second;
    return res;
}

Function *Unit::getInsertPoint()
{
    if (func_list.empty())
        return nullptr;
    return func_list.back();
}

Unit::Unit()
{
    global_decl = nullptr;
}

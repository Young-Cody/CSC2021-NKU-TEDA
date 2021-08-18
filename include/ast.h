#ifndef __AST_H_
#define __AST_H_
#include "symtab.h"
#include "instruction.h"
#include "function.h"
#include "basicblock.h"
#include "unit.h"
#include <cstddef>
#include <string>
#include <vector>
#include <string.h>
using namespace std;

//kind
enum
{
    NODE_STMT,
    NODE_DECL,
    NODE_EXPR,
    NODE_PROG
};

//kind_kind
enum
{
    STMT_WHILE,
    STMT_IF,
    STMT_BLANK,
    STMT_CONTINUE,
    STMT_BREAK,
    STMT_RETURN,
    STMT_COMPOUND,
    STMT_EXPR,
    STMT_FUNC,
    STMT_ASSIGN
};

//kind_kind
enum
{
    OP_E,
    OP_NE,
    OP_L,
    OP_G,
    OP_LE,
    OP_GE,
    OP_AND,
    OP_OR,
    OP_NOT, //unary !
    OP_ADD,
    OP_MINUS,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_UMINUS, //unary -
    OP_FUNC,
    OP_ARRAY,
    OP_INITLIST
};

struct Label
{
    std::vector<Instruction *> *truelist;
    std::vector<Instruction *> *falselist;
    std::vector<Instruction *> *nextlist;
    Instruction *begininst;
};

struct Node
{
    int kind;
    int kind_kind;
    int seq;
    Node *child[3];
    Node *sibling;
    symbol_entry *sym_ptr; // note: pointer to symbol entry, which includes lineno, type, temporary value, constant value, etc.

    Label label;
    void addSibling(Node *);
    void output(ostream &out);
    void printNodeInfo(ostream &out);
};

class Ast
{
private:
    Node *root;
    int node_seq;
    ostream &out;
    std::vector<Node *> loop;
    std::map<symbol_entry*, Function*> sym2func;
    Unit *unit;
    Function *func;

private:
    void recursive_gen_code(Node *t);
    void expr_gen_code(Node *t, bool cond = 0);
    void stmt_gen_code(Node *t);
    void decl_gen_code(Node *t);
    void gen_logic(Node *t);
    void recursive_gen_decl(Node *, int&, Operand *);
    Instruction *get_begininst(Instruction *, Instruction *, Instruction *);
    Operand *create_operand(Node *t);
    Operand *create_index(Node *t, Operand **);
    void backPatch(std::vector<Instruction *> *list, Instruction *inst);
    void backPatch(std::vector<Instruction *> *list, BasicBlock *bb);
    std::vector<Instruction *> *merge(std::vector<Instruction *> *, std::vector<Instruction *> *);

public:
    Ast(ostream &o);
    Node *NewRoot(int kind, int kind_kind, symbol_entry *s = nullptr,
                  Node *child1 = nullptr, Node *child2 = nullptr, Node *child3 = nullptr);
    void printAst();
    Unit *gen_code();
    static int get_arrayIndex(Node *t);
    static int get_constExpVal(Node *t);
    static void set_initVal(Node *t, int offset);
};

#endif
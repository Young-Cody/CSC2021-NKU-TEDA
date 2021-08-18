%{
    #include "common.h"
    #include "symtab.h"
    #include <iostream>
    #include <sstream>
    #include <vector>
    extern Ast *ast;
    int yylex();
    int yyerror( char const * );
    type *global_type;
%}
%defines

%start program

%token ID INTEGER CONSTSTR
%token IF ELSE WHILE
%token INT VOID
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA
%token ADD ASSIGN EQUAL NOT MINUS MUL DIV MOD OR AND NOTEQUAL LESS GREATER LESSEQUAL GREATEREQUAL 
%token CONST
%token RETURN CONTINUE BREAK

%precedence THEN
%precedence ELSE
%%
program
    : Stmts {
        ast->NewRoot(NODE_PROG,-1,$1->sym_ptr, $1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{$$=$1;$$->addSibling($2);}
    ;
Stmt
    : AssignStmt {$$=$1;}
    | ExprStmt {$$=$1;}
    | BlankStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | Decl {$$=$1;}
    | FuncDef {$$=$1;}
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_ASSIGN, $1->sym_ptr, $1, $3);
    }
    ;
ExprStmt
    : Exp SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_EXPR, $1->sym_ptr, $1);
    }
    ;
BlankStmt
    : SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_BLANK);
    }
    ;
BlockStmt
    : LBRACE {symbol_table::enter_scope();} Stmts RBRACE {
        $$ = ast->NewRoot(NODE_STMT,STMT_COMPOUND,NULL,$3);
        symbol_table::leave_scope();
    }
    | LBRACE RBRACE
    {
        $$ = ast->NewRoot(NODE_STMT,STMT_COMPOUND);
    }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = ast->NewRoot(NODE_STMT,STMT_IF,$3->sym_ptr,$3,$5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = ast->NewRoot(NODE_STMT,STMT_IF,$3->sym_ptr,$3,$5,$7);
    }
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = ast->NewRoot(NODE_STMT,STMT_WHILE,$3->sym_ptr,$3,$5);
    }
    ;
BreakStmt
    : BREAK SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_BREAK);
    }
    ;
ContinueStmt
    : CONTINUE SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_CONTINUE);
    }
    ;
ReturnStmt
    : RETURN SEMICOLON {
        $$ = ast->NewRoot(NODE_STMT,STMT_RETURN);
    }
    | RETURN Exp SEMICOLON{
        $$ = ast->NewRoot(NODE_STMT,STMT_RETURN,$2->sym_ptr, $2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
LVal
    :
    ID {
        symbol_entry *t;
        t = identifiers->lookup($1->sym_ptr->name);
        $$ = ast->NewRoot(NODE_EXPR, -1, t);
        }
    | LVal LBRACKET Exp RBRACKET {
        $$ = ast->NewRoot(NODE_EXPR,OP_ARRAY, NULL, $1, $3);
        $$->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        $$->sym_ptr->tp = $1->sym_ptr->tp->operand;
        $$->sym_ptr->array_value = $1->sym_ptr->array_value;
    }
    ;
PrimaryExp
    :
    LPAREN Exp RPAREN {$$ = $2;}
    | LVal {$$ = $1;}
    | INTEGER {$$ = $1;}
    | CONSTSTR {$$ = $1;}
    | ID LPAREN FuncRParams RPAREN {
        symbol_entry *t;
        t = functions->lookup($1->sym_ptr->name);
        if(t == NULL)
        {
            t = functions->install($1->sym_ptr->name);
            t->undefined = true;
            t->tp = typesys.int_type;
            t->tp = typesys.func(t->tp, NULL);
        }
        $$ = ast->NewRoot(NODE_EXPR,OP_FUNC, NULL, $3);
        $$->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        $$->sym_ptr->tp = t->tp->operand;
        $$->sym_ptr->undefined = t->undefined;
        $$->sym_ptr->name = $1->sym_ptr->name;
    }
    | ID LPAREN RPAREN {
        symbol_entry *t;
        t = functions->lookup($1->sym_ptr->name);
        Node *c = NULL;
        if(t == NULL)
        {
            t = functions->install($1->sym_ptr->name);
            t->undefined = true;
            t->tp = typesys.int_type;
            t->tp = typesys.func(t->tp, NULL);
        }
        if($1->sym_ptr->name == "_sysy_starttime" || $1->sym_ptr->name == "_sysy_stoptime")
        {
            c = ast->NewRoot(NODE_EXPR, -1);
            c->sym_ptr = constants->find_constant(0);
        }
        $$ = ast->NewRoot(NODE_EXPR,OP_FUNC, NULL, c);
        $$->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        $$->sym_ptr->tp = t->tp->operand;
        $$->sym_ptr->undefined = t->undefined;
        $$->sym_ptr->name = $1->sym_ptr->name;
    }
    ;
UnaryExp
    :
    PrimaryExp {$$ = $1;}
    |
    ADD UnaryExp{$$ = $2;}
    |
    NOT UnaryExp {
        $$ = ast->NewRoot(NODE_EXPR,OP_NOT, NULL, $2);
    }
    |
    MINUS UnaryExp{
        $$ = ast->NewRoot(NODE_EXPR,OP_UMINUS, NULL, $2);
    }
    ;
MulExp
    :
    UnaryExp {$$ = $1;}
    |
    MulExp MUL UnaryExp
    {
        $$ = ast->NewRoot(NODE_EXPR,OP_MUL, NULL, $1, $3);
    }
    |
    MulExp DIV UnaryExp
    {
        $$ = ast->NewRoot(NODE_EXPR,OP_DIV, NULL, $1, $3);
    }
    |
    MulExp MOD UnaryExp
    {
        $$ = ast->NewRoot(NODE_EXPR,OP_MOD, NULL, $1, $3);
    }
    ;
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_ADD, NULL, $1, $3);
    }
    |
    AddExp MINUS MulExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_MINUS, NULL, $1, $3);
    }
    ;
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_L, NULL, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_G, NULL, $1, $3);
    }
    |
    RelExp LESSEQUAL AddExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_LE, NULL, $1, $3);
    }
    |
    RelExp GREATEREQUAL AddExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_GE, NULL, $1, $3);
    }
    ;
EqExp
    :
    RelExp {$$=$1;}
    |
    EqExp EQUAL RelExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_E, NULL, $1, $3);
    }
    |
    EqExp NOTEQUAL RelExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_NE, NULL, $1, $3);
    }
    ;
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_AND, NULL, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        $$ = ast->NewRoot(NODE_EXPR, OP_OR, NULL, $1, $3);
    }
    ;
ConstExp
    :
    AddExp {
        $$ = $1;
        }
    ;
FuncRParams
    :
    Exp {$$ = $1;}
    |
    FuncRParams COMMA Exp {
        $$ = $1;
        $$->addSibling($3);
    }
    ;
Type
    : INT {
        global_type = typesys.int_type;
    }
    | VOID {
        global_type = typesys.void_type;
    }
    ;
Decl
    :
    ConstDecl {$$=$1;}
    |
    VarDecl {$$=$1;}
    ;
ConstDecl
    :
    CONST Type {global_type = typesys.qual(global_type);} ConstDefList SEMICOLON {
        $$ = $4;
    }
    ;
ConstDefList
    :
    ConstDef{$$=$1;}
    |
    ConstDefList COMMA ConstDef
    {
        $$=$1;
        $$->addSibling($3);
    }
    ;
ConstDef
    :
    ID ASSIGN ConstInitVal {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;
        se->constant = Ast::get_constExpVal($3);
        if(se->scope > GLOBAL)
        {
            se->label = symbol_table::gen_label(1);
            ostringstream os;
            os << se->name << "." << se->label;
            symbol_entry *t = globals->install(os.str());
            t->tp = se->tp;
            t->constant = se->constant;
        }

        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    |
    ID ArrayList ASSIGN ConstInitVal {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;
        vector<int> s;
        Node *p = $2;
        while(p)
        {
            s.push_back(Ast::get_constExpVal(p));
            p = p->sibling;
        }
        while(!s.empty())
        {
            int dim = s.back();
            s.pop_back();
            se->tp = typesys.array(se->tp, dim);
        }
        se->array_value = new int[se->tp->size / se->tp->align];
        memset(se->array_value, 0, se->tp->size);
        $4->sym_ptr->tp = se->tp;
        $4->sym_ptr->array_value = se->array_value;
        Ast::set_initVal($4,0);
        if(se->scope > GLOBAL)
        {
            se->label = symbol_table::gen_label(1);
            ostringstream os;
            os << se->name << "." << se->label;
            symbol_entry *t = globals->install(os.str());
            t->tp = se->tp;
            t->array_value = se->array_value;
        }

        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    ;
ConstInitVal
    :
    ConstExp {$$=$1;}
    |
    LBRACE RBRACE {
        $$ = ast->NewRoot(NODE_EXPR,OP_INITLIST);
        $$->sym_ptr = new symbol_entry;
    }
    |
    LBRACE ConstInitValList RBRACE {
        $$ = ast->NewRoot(NODE_EXPR,OP_INITLIST, NULL, $2);
        $$->sym_ptr = new symbol_entry;
    }
    ;
ConstInitValList
    :
    ConstInitVal {
        if($1->kind_kind != OP_INITLIST)
            $1 = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), $1);
        $$=$1;
    }
    |
    ConstInitValList COMMA ConstInitVal {
        $$ = $1;
        if($3->kind_kind != OP_INITLIST)
            $3 = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), $3);
        $$->addSibling($3);
    }
    ;
ArrayList
    :
    LBRACKET ConstExp RBRACKET {$$ = $2;}
    |
    ArrayList LBRACKET ConstExp RBRACKET {
        $$ = $1;
        $$->addSibling($3);
    }
    ;
VarDecl
    :
    Type VarDefList SEMICOLON {
        $$ = $2;
    }
    ;
VarDefList
    :
    VarDefList COMMA VarDef {
        $$ = $1;
        $$->addSibling($3);
    }
    |
    VarDef{
        $$ = $1;
    }
    ;
VarDef
    :
    ID {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;
        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    |
    ID ArrayList {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = $2;
        while(p)
        {
            s.push_back(Ast::get_constExpVal(p));
            p = p->sibling;
        }
        while(!s.empty())
        {
            int dim = s.back();
            s.pop_back();
            se->tp = typesys.array(se->tp, dim);
        }
        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    |
    ID ASSIGN InitVal {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;
        if(se->scope == GLOBAL)
        {
            se->constant = Ast::get_constExpVal($3);
            $$ = ast->NewRoot(NODE_DECL,-1, se);
        }
        else 
            $$ = ast->NewRoot(NODE_DECL,-1, se, $3);
    }
    |
    ID ArrayList ASSIGN InitVal {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($1->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = $2;
        while(p)
        {
            s.push_back(Ast::get_constExpVal(p));
            p = p->sibling;
        }
        while(!s.empty())
        {
            int dim = s.back();
            s.pop_back();
            se->tp = typesys.array(se->tp, dim);
        }
        $4->sym_ptr->tp = se->tp;
        if(se->scope == GLOBAL)
        {
            se->array_value = new int[se->tp->size / se->tp->align];
            memset(se->array_value, 0, se->tp->size);
            $4->sym_ptr->tp = se->tp;
            $4->sym_ptr->array_value = se->array_value;
            Ast::set_initVal($4,0);
            $$ = ast->NewRoot(NODE_DECL,-1, se);
        }
        else
            $$ = ast->NewRoot(NODE_DECL,-1, se, $4);
    }
    ;
InitVal
    :
    Exp {$$=$1;}
    |
    LBRACE RBRACE {
        $$ = ast->NewRoot(NODE_EXPR,OP_INITLIST);
        $$->sym_ptr = new symbol_entry;
        }
    |
    LBRACE InitValList RBRACE {
        $$ = ast->NewRoot(NODE_EXPR,OP_INITLIST, NULL, $2);
        $$->sym_ptr = new symbol_entry;
    }
    ;
InitValList
    :
    InitVal {
        if($1->kind_kind != OP_INITLIST)
            $1 = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), $1);
        $$=$1;
    }
    |
    InitValList COMMA InitVal {
        $$ = $1;
        if($3->kind_kind != OP_INITLIST)
            $3 = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), $3);
        $$=$1;
        $$->addSibling($3);
    }
    ;
FuncDef
    :
    Type ID {
        symbol_entry *se;
        se = functions->install($2->sym_ptr->name);
        se->tp = typesys.func(global_type,NULL);
        symbol_table::enter_scope();}
    LPAREN OptFuncParams RPAREN
    BlockStmt
    {
        symbol_entry *t;
        t = functions->lookup($2->sym_ptr->name);
        t->undefined = false;
        int num_of_params = 0;
        Node *p = $5;
        while(p)
        {
            num_of_params++;
            p = p->sibling;
        }
        if(num_of_params > 0)
        {
            t->tp->func_params = new symbol_entry*[num_of_params + 1];
            p = $5;
            int i = 0;
            while(p)
            {
                t->tp->func_params[i] = p->sym_ptr;
                i++;
                p = p->sibling;
            }
            t->tp->func_params[num_of_params] = NULL;
        }
        $$ = ast->NewRoot(NODE_STMT, STMT_FUNC, t, $5, $7);
        symbol_table::leave_scope();
    }
    ;
OptFuncParams
    :
    FuncFParams {$$=$1;}
    | %empty {$$ = NULL;}
    ;
FuncFParams
    :
    FuncFParams COMMA FuncFParam {
        $$ = $1;
        Node *p = $1;
        while(p->sibling)
            p = p->sibling;
        p->sibling = $3;
    }
    |
    FuncFParam {$$ = $1;}
    ;
FuncFParam
    :
    Type ID {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($2->sym_ptr->name);
        se->tp = global_type;

        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    |
    Type ID FArrayList {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install($2->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = $3->sibling;
        while(p)
        {
            s.push_back(Ast::get_constExpVal(p));
            p = p->sibling;
        }
        while(!s.empty())
        {
            int dim = s.back();
            s.pop_back();
            se->tp = typesys.array(se->tp, dim);
        }
        se->tp = typesys.ptr(se->tp);

        $$ = ast->NewRoot(NODE_DECL,-1, se);
    }
    ;
FArrayList
    :
    LBRACKET RBRACKET
    {
        $$ = ast->NewRoot(NODE_EXPR, -1);
    }
    |
    FArrayList LBRACKET Exp RBRACKET {
        $$ = $1;
        $$->addSibling($3);
    }
    ;
%%

int yyerror(char const* message)
{
    std::cout<<message<<std::endl;
    return -1;
}
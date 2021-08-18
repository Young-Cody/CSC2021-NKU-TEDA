%option noyywrap
%{
    #include "common.h"
    #include "parser.h"
    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    extern Ast *ast;
%}

DECIMIAL [1-9][0-9]*
OCTAL 0[0-7]*
HEXADECIMAL 0(x|X)[0-9a-zA-Z]+
PUTF "putf"
STARTTIME "starttime"
STOPTIME "stoptime"
ID [[:alpha:]_][[:alpha:][:digit:]_]*
CONSTSTR \".*\"
CHAR \'\\?.?\'
EOL (\r\n|\n|\r)
WHITE [\t ]
LINECOMMENT \/\/[^\n]*
BLOCKCOMMENT_BEGIN "/*"
BLOCKCOMMENT_ELE .
BLOCKCOMMENT_LINE (\r\n|\n|\r)
BLOCKCOMMENT_END "*/"
%x BLOCKCOMMENT

%%

"int" return INT;
"void" return VOID;
"const" return CONST;

"if" return IF;
"while" return WHILE;
"else" return ELSE;
"return" return RETURN;
"continue" return CONTINUE;
"break" return BREAK;
"=" return ASSIGN;
"+" return ADD;
"-" return MINUS;
"*" return MUL;
"/" return DIV;
"%" return MOD;
"!" return NOT;
"||" return OR;
"&&" return AND;
"!=" return NOTEQUAL;
"==" return EQUAL;
"<" return LESS;
">" return GREATER;
"<=" return LESSEQUAL;
">=" return GREATEREQUAL;
"," return COMMA;
";" return SEMICOLON;
"(" return LPAREN;
")" return RPAREN;
"{" return LBRACE;
"}" return RBRACE;
"[" return LBRACKET;
"]" return RBRACKET;

{DECIMIAL} {
    Node *node = ast->NewRoot(NODE_EXPR, -1);
    node->sym_ptr = constants->find_constant(atoi(yytext));
    yylval = node;
    return INTEGER;
}
{OCTAL} {
    int val;
    sscanf(yytext, "%o", &val);
    Node *node = ast->NewRoot(NODE_EXPR, -1);
    node->sym_ptr = constants->find_constant(val);
    yylval = node;
    return INTEGER;
}
{HEXADECIMAL} {
    int val;
    sscanf(yytext, "%x", &val);
    Node *node = ast->NewRoot(NODE_EXPR, -1);
    node->sym_ptr = constants->find_constant(val);
    yylval = node;
    return INTEGER;
}
{PUTF} {
    Node *n = new Node;
    symbol_entry *e = new symbol_entry;
    e->name = "printf";
    n->sym_ptr = e;
    yylval = n;
    return ID;
}
{STARTTIME} {
    Node *n = new Node;
    symbol_entry *e = new symbol_entry;
    e->name = "_sysy_starttime";
    n->sym_ptr = e;
    yylval = n;
    return ID;
}
{STOPTIME} {
    Node *n = new Node;
    symbol_entry *e = new symbol_entry;
    e->name = "_sysy_stoptime";
    n->sym_ptr = e;
    yylval = n;
    return ID;
}
{ID} {
    Node *n = new Node;
    symbol_entry *e = new symbol_entry;
    e->name = yytext;
    n->sym_ptr = e;
    yylval = n;
    return ID;
}
{CONSTSTR} {
    static int strno = 0;
    symbol_entry *s = globals->lookup(yytext);
    if(s == NULL)
    {
        s = globals->install(yytext);
        s->str = 1;
        s->label = strno++;
    }
    Node *node = ast->NewRoot(NODE_EXPR, -1);
    node->sym_ptr = s;
    yylval = node;
    return CONSTSTR;
}
{EOL} yylineno++;
{WHITE}
{LINECOMMENT}
{BLOCKCOMMENT_BEGIN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{BLOCKCOMMENT_ELE} {}
<BLOCKCOMMENT>{BLOCKCOMMENT_LINE} {yylineno++;}
<BLOCKCOMMENT>{BLOCKCOMMENT_END} {BEGIN INITIAL;}
%%
/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

    #include "common.h"
    #include "symtab.h"
    #include <iostream>
    #include <sstream>
    #include <vector>
    extern Ast *ast;
    int yylex();
    int yyerror( char const * );
    type *global_type;

#line 82 "src/parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_INCLUDE_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    INTEGER = 259,
    CONSTSTR = 260,
    IF = 261,
    ELSE = 262,
    WHILE = 263,
    INT = 264,
    VOID = 265,
    LPAREN = 266,
    RPAREN = 267,
    LBRACE = 268,
    RBRACE = 269,
    SEMICOLON = 270,
    LBRACKET = 271,
    RBRACKET = 272,
    COMMA = 273,
    ADD = 274,
    ASSIGN = 275,
    EQUAL = 276,
    NOT = 277,
    MINUS = 278,
    MUL = 279,
    DIV = 280,
    MOD = 281,
    OR = 282,
    AND = 283,
    NOTEQUAL = 284,
    LESS = 285,
    GREATER = 286,
    LESSEQUAL = 287,
    GREATEREQUAL = 288,
    CONST = 289,
    RETURN = 290,
    CONTINUE = 291,
    BREAK = 292,
    THEN = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   252

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  103
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  178

#define YYUNDEFTOK  2
#define YYMAXUTOK   293


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    28,    28,    33,    34,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    51,    56,    61,    66,
      66,    70,    76,    79,    84,    89,    94,    99,   102,   108,
     112,   116,   121,   130,   131,   132,   133,   134,   150,   175,
     177,   179,   183,   189,   191,   196,   201,   208,   210,   215,
     222,   224,   229,   234,   239,   246,   248,   253,   260,   262,
     269,   271,   278,   284,   286,   292,   295,   301,   303,   307,
     307,   313,   315,   323,   343,   382,   384,   389,   396,   402,
     411,   413,   420,   426,   431,   437,   446,   469,   484,   520,
     522,   527,   534,   540,   550,   550,   587,   588,   592,   600,
     604,   614,   641,   646
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "INTEGER", "CONSTSTR", "IF",
  "ELSE", "WHILE", "INT", "VOID", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
  "SEMICOLON", "LBRACKET", "RBRACKET", "COMMA", "ADD", "ASSIGN", "EQUAL",
  "NOT", "MINUS", "MUL", "DIV", "MOD", "OR", "AND", "NOTEQUAL", "LESS",
  "GREATER", "LESSEQUAL", "GREATEREQUAL", "CONST", "RETURN", "CONTINUE",
  "BREAK", "THEN", "$accept", "program", "Stmts", "Stmt", "AssignStmt",
  "ExprStmt", "BlankStmt", "BlockStmt", "$@1", "IfStmt", "WhileStmt",
  "BreakStmt", "ContinueStmt", "ReturnStmt", "Exp", "Cond", "LVal",
  "PrimaryExp", "UnaryExp", "MulExp", "AddExp", "RelExp", "EqExp",
  "LAndExp", "LOrExp", "ConstExp", "FuncRParams", "Type", "Decl",
  "ConstDecl", "$@2", "ConstDefList", "ConstDef", "ConstInitVal",
  "ConstInitValList", "ArrayList", "VarDecl", "VarDefList", "VarDef",
  "InitVal", "InitValList", "FuncDef", "$@3", "OptFuncParams",
  "FuncFParams", "FuncFParam", "FArrayList", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

#define YYPACT_NINF (-136)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-95)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     154,    -7,  -136,  -136,    21,    29,  -136,  -136,   229,    20,
    -136,   229,   229,   229,    14,   181,    28,    30,    58,   154,
    -136,  -136,  -136,  -136,  -136,  -136,  -136,  -136,  -136,  -136,
      76,     0,  -136,  -136,    55,    23,    10,  -136,  -136,  -136,
    -136,   190,   229,   229,    86,    72,  -136,   154,  -136,  -136,
    -136,  -136,  -136,    82,  -136,  -136,  -136,  -136,  -136,   229,
     229,   229,   229,   229,   229,   229,     6,    99,  -136,  -136,
    -136,    13,    92,    23,   148,    -2,    77,    79,   101,  -136,
     118,   117,  -136,   108,   115,  -136,  -136,  -136,    55,    55,
     229,   203,    34,   123,  -136,   132,  -136,   229,   154,   229,
     229,   229,   229,   229,   229,   229,   229,   154,  -136,    37,
     100,  -136,  -136,  -136,    23,   119,    25,  -136,  -136,   229,
     203,    14,    39,  -136,  -136,   135,    23,    23,    23,    23,
     148,   148,    -2,    77,  -136,   216,    74,  -136,   117,  -136,
    -136,  -136,    85,   122,  -136,   140,   133,   128,  -136,   154,
      73,  -136,  -136,   216,  -136,  -136,   203,  -136,   131,   137,
      14,  -136,  -136,  -136,    93,  -136,  -136,   139,   145,  -136,
    -136,  -136,   216,  -136,   229,  -136,   151,  -136
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    31,    35,    36,     0,     0,    65,    66,     0,    19,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       3,     5,     6,     7,     8,     9,    10,    11,    12,    13,
       0,    34,    39,    43,    47,    29,     0,    14,    67,    68,
      15,     0,     0,     0,     0,    34,    21,     0,    40,    41,
      42,    69,    27,     0,    26,    25,     1,     4,    17,     0,
       0,     0,     0,     0,     0,     0,    85,     0,    84,    38,
      63,     0,     0,    50,    55,    58,    60,    30,     0,    33,
       0,     0,    28,     0,     0,    44,    45,    46,    48,    49,
       0,     0,    86,     0,    82,     0,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    20,     0,
       0,    71,    32,    16,    62,     0,     0,    89,    87,     0,
       0,    97,    85,    83,    64,    22,    51,    52,    53,    54,
      56,    57,    59,    61,    24,     0,     0,    70,     0,    80,
      90,    92,     0,     0,    88,     0,     0,    96,    99,     0,
       0,    75,    73,     0,    72,    91,     0,    81,   100,     0,
       0,    23,    76,    78,     0,    74,    93,     0,   101,    95,
      98,    77,     0,   102,     0,    79,     0,   103
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -136,  -136,   124,    -5,  -136,  -136,  -136,    11,  -136,  -136,
    -136,  -136,  -136,  -136,    -8,   129,     2,  -136,    -1,     9,
     -34,   -40,    69,    81,  -136,   -84,  -136,   -11,  -136,  -136,
    -136,  -136,    44,  -135,  -136,    66,  -136,  -136,    88,  -115,
    -136,  -136,  -136,  -136,  -136,    38,  -136
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    18,    19,    20,    21,    22,    23,    24,    47,    25,
      26,    27,    28,    29,    30,    72,    45,    32,    33,    34,
      35,    74,    75,    76,    77,   151,    71,    36,    37,    38,
      81,   110,   111,   152,   164,    92,    39,    67,    68,   118,
     142,    40,    93,   146,   147,   148,   168
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,   141,    31,    51,    41,   144,   115,    53,    73,    73,
      48,    49,    50,    66,    57,   163,    59,   -94,   165,   103,
      60,    31,    90,     6,     7,    96,    91,   104,     1,     2,
       3,    97,    42,    70,    46,   143,     8,   175,   116,   140,
      43,   166,    64,    54,    11,    55,    65,    12,    13,    31,
     119,    83,    84,    90,   120,    90,   114,   135,    56,    91,
      85,    86,    87,   130,   131,   126,   127,   128,   129,    73,
      73,    73,    73,    88,    89,    57,     1,     2,     3,    61,
      62,    63,    31,   117,     8,   114,   150,   162,    59,   124,
     119,    58,    11,   125,   153,    12,    13,    82,    79,   155,
      31,   114,   134,   156,    98,   105,   106,   171,   117,    31,
     145,   172,   117,   107,    94,   137,   114,    95,   138,   114,
     109,     1,     2,     3,     4,   112,     5,     6,     7,     8,
     113,     9,   108,    10,   121,   122,   139,    11,   114,   157,
      12,    13,   149,   158,   161,   159,   160,   167,   117,   145,
       9,    31,    14,    15,    16,    17,   173,     1,     2,     3,
       4,   174,     5,     6,     7,     8,   176,     9,   177,    10,
     169,    80,    78,    11,   132,   136,    12,    13,    99,   100,
     101,   102,   154,   123,     1,     2,     3,   133,    14,    15,
      16,    17,     8,     1,     2,     3,    52,     0,   170,     0,
      11,     8,    69,    12,    13,     0,     1,     2,     3,    11,
       0,     0,    12,    13,     8,     0,   116,     0,     0,     1,
       2,     3,    11,     0,     0,    12,    13,     8,     0,   150,
       0,     0,     1,     2,     3,    11,     0,     0,    12,    13,
       8,     0,     0,     0,     0,     0,     0,     0,    11,     0,
       0,    12,    13
};

static const yytype_int16 yycheck[] =
{
       8,   116,     0,    14,    11,   120,    90,    15,    42,    43,
      11,    12,    13,     3,    19,   150,    16,    11,   153,    21,
      20,    19,    16,     9,    10,    12,    20,    29,     3,     4,
       5,    18,    11,    41,    14,   119,    11,   172,    13,    14,
      11,   156,    19,    15,    19,    15,    23,    22,    23,    47,
      16,    59,    60,    16,    20,    16,    90,    20,     0,    20,
      61,    62,    63,   103,   104,    99,   100,   101,   102,   103,
     104,   105,   106,    64,    65,    80,     3,     4,     5,    24,
      25,    26,    80,    91,    11,   119,    13,    14,    16,    97,
      16,    15,    19,    98,    20,    22,    23,    15,    12,    14,
      98,   135,   107,    18,    12,    28,    27,    14,   116,   107,
     121,    18,   120,    12,    15,    15,   150,    18,    18,   153,
       3,     3,     4,     5,     6,    17,     8,     9,    10,    11,
      15,    13,    14,    15,    11,     3,    17,    19,   172,    17,
      22,    23,     7,     3,   149,    12,    18,    16,   156,   160,
      13,   149,    34,    35,    36,    37,    17,     3,     4,     5,
       6,    16,     8,     9,    10,    11,   174,    13,    17,    15,
     159,    47,    43,    19,   105,   109,    22,    23,    30,    31,
      32,    33,   138,    95,     3,     4,     5,   106,    34,    35,
      36,    37,    11,     3,     4,     5,    15,    -1,   160,    -1,
      19,    11,    12,    22,    23,    -1,     3,     4,     5,    19,
      -1,    -1,    22,    23,    11,    -1,    13,    -1,    -1,     3,
       4,     5,    19,    -1,    -1,    22,    23,    11,    -1,    13,
      -1,    -1,     3,     4,     5,    19,    -1,    -1,    22,    23,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    22,    23
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    13,
      15,    19,    22,    23,    34,    35,    36,    37,    40,    41,
      42,    43,    44,    45,    46,    48,    49,    50,    51,    52,
      53,    55,    56,    57,    58,    59,    66,    67,    68,    75,
      80,    11,    11,    11,    53,    55,    14,    47,    57,    57,
      57,    66,    15,    53,    15,    15,     0,    42,    15,    16,
      20,    24,    25,    26,    19,    23,     3,    76,    77,    12,
      53,    65,    54,    59,    60,    61,    62,    63,    54,    12,
      41,    69,    15,    53,    53,    57,    57,    57,    58,    58,
      16,    20,    74,    81,    15,    18,    12,    18,    12,    30,
      31,    32,    33,    21,    29,    28,    27,    12,    14,     3,
      70,    71,    17,    15,    59,    64,    13,    53,    78,    16,
      20,    11,     3,    77,    53,    42,    59,    59,    59,    59,
      60,    60,    61,    62,    42,    20,    74,    15,    18,    17,
      14,    78,    79,    64,    78,    66,    82,    83,    84,     7,
      13,    64,    72,    20,    71,    14,    18,    17,     3,    12,
      18,    42,    14,    72,    73,    72,    78,    16,    85,    46,
      84,    14,    18,    17,    16,    72,    53,    17
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    39,    40,    41,    41,    42,    42,    42,    42,    42,
      42,    42,    42,    42,    42,    42,    43,    44,    45,    47,
      46,    46,    48,    48,    49,    50,    51,    52,    52,    53,
      54,    55,    55,    56,    56,    56,    56,    56,    56,    57,
      57,    57,    57,    58,    58,    58,    58,    59,    59,    59,
      60,    60,    60,    60,    60,    61,    61,    61,    62,    62,
      63,    63,    64,    65,    65,    66,    66,    67,    67,    69,
      68,    70,    70,    71,    71,    72,    72,    72,    73,    73,
      74,    74,    75,    76,    76,    77,    77,    77,    77,    78,
      78,    78,    79,    79,    81,    80,    82,    82,    83,    83,
      84,    84,    85,    85
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     2,     1,     0,
       4,     2,     5,     7,     5,     2,     2,     2,     3,     1,
       1,     1,     4,     3,     1,     1,     1,     4,     3,     1,
       2,     2,     2,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     1,     3,     1,     1,     1,     1,     0,
       5,     1,     3,     3,     4,     1,     2,     3,     1,     3,
       3,     4,     3,     3,     1,     1,     2,     3,     4,     1,
       2,     3,     1,     3,     0,     7,     1,     0,     3,     1,
       2,     3,     2,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 28 "src/parser.y"
            {
        ast->NewRoot(NODE_PROG,-1,yyvsp[0]->sym_ptr, yyvsp[0]);
    }
#line 1490 "src/parser.cpp"
    break;

  case 3:
#line 33 "src/parser.y"
           {yyval=yyvsp[0];}
#line 1496 "src/parser.cpp"
    break;

  case 4:
#line 34 "src/parser.y"
                {yyval=yyvsp[-1];yyval->addSibling(yyvsp[0]);}
#line 1502 "src/parser.cpp"
    break;

  case 5:
#line 37 "src/parser.y"
                 {yyval=yyvsp[0];}
#line 1508 "src/parser.cpp"
    break;

  case 6:
#line 38 "src/parser.y"
               {yyval=yyvsp[0];}
#line 1514 "src/parser.cpp"
    break;

  case 7:
#line 39 "src/parser.y"
                {yyval=yyvsp[0];}
#line 1520 "src/parser.cpp"
    break;

  case 8:
#line 40 "src/parser.y"
                {yyval=yyvsp[0];}
#line 1526 "src/parser.cpp"
    break;

  case 9:
#line 41 "src/parser.y"
             {yyval=yyvsp[0];}
#line 1532 "src/parser.cpp"
    break;

  case 10:
#line 42 "src/parser.y"
                {yyval=yyvsp[0];}
#line 1538 "src/parser.cpp"
    break;

  case 11:
#line 43 "src/parser.y"
                {yyval=yyvsp[0];}
#line 1544 "src/parser.cpp"
    break;

  case 12:
#line 44 "src/parser.y"
                   {yyval=yyvsp[0];}
#line 1550 "src/parser.cpp"
    break;

  case 13:
#line 45 "src/parser.y"
                 {yyval=yyvsp[0];}
#line 1556 "src/parser.cpp"
    break;

  case 14:
#line 46 "src/parser.y"
           {yyval=yyvsp[0];}
#line 1562 "src/parser.cpp"
    break;

  case 15:
#line 47 "src/parser.y"
              {yyval=yyvsp[0];}
#line 1568 "src/parser.cpp"
    break;

  case 16:
#line 51 "src/parser.y"
                              {
        yyval = ast->NewRoot(NODE_STMT,STMT_ASSIGN, yyvsp[-3]->sym_ptr, yyvsp[-3], yyvsp[-1]);
    }
#line 1576 "src/parser.cpp"
    break;

  case 17:
#line 56 "src/parser.y"
                    {
        yyval = ast->NewRoot(NODE_STMT,STMT_EXPR, yyvsp[-1]->sym_ptr, yyvsp[-1]);
    }
#line 1584 "src/parser.cpp"
    break;

  case 18:
#line 61 "src/parser.y"
                {
        yyval = ast->NewRoot(NODE_STMT,STMT_BLANK);
    }
#line 1592 "src/parser.cpp"
    break;

  case 19:
#line 66 "src/parser.y"
             {symbol_table::enter_scope();}
#line 1598 "src/parser.cpp"
    break;

  case 20:
#line 66 "src/parser.y"
                                                         {
        yyval = ast->NewRoot(NODE_STMT,STMT_COMPOUND,NULL,yyvsp[-1]);
        symbol_table::leave_scope();
    }
#line 1607 "src/parser.cpp"
    break;

  case 21:
#line 71 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_STMT,STMT_COMPOUND);
    }
#line 1615 "src/parser.cpp"
    break;

  case 22:
#line 76 "src/parser.y"
                                            {
        yyval = ast->NewRoot(NODE_STMT,STMT_IF,yyvsp[-2]->sym_ptr,yyvsp[-2],yyvsp[0]);
    }
#line 1623 "src/parser.cpp"
    break;

  case 23:
#line 79 "src/parser.y"
                                           {
        yyval = ast->NewRoot(NODE_STMT,STMT_IF,yyvsp[-4]->sym_ptr,yyvsp[-4],yyvsp[-2],yyvsp[0]);
    }
#line 1631 "src/parser.cpp"
    break;

  case 24:
#line 84 "src/parser.y"
                                    {
        yyval = ast->NewRoot(NODE_STMT,STMT_WHILE,yyvsp[-2]->sym_ptr,yyvsp[-2],yyvsp[0]);
    }
#line 1639 "src/parser.cpp"
    break;

  case 25:
#line 89 "src/parser.y"
                      {
        yyval = ast->NewRoot(NODE_STMT,STMT_BREAK);
    }
#line 1647 "src/parser.cpp"
    break;

  case 26:
#line 94 "src/parser.y"
                         {
        yyval = ast->NewRoot(NODE_STMT,STMT_CONTINUE);
    }
#line 1655 "src/parser.cpp"
    break;

  case 27:
#line 99 "src/parser.y"
                       {
        yyval = ast->NewRoot(NODE_STMT,STMT_RETURN);
    }
#line 1663 "src/parser.cpp"
    break;

  case 28:
#line 102 "src/parser.y"
                          {
        yyval = ast->NewRoot(NODE_STMT,STMT_RETURN,yyvsp[-1]->sym_ptr, yyvsp[-1]);
    }
#line 1671 "src/parser.cpp"
    break;

  case 29:
#line 108 "src/parser.y"
           {yyval = yyvsp[0];}
#line 1677 "src/parser.cpp"
    break;

  case 30:
#line 112 "src/parser.y"
           {yyval = yyvsp[0];}
#line 1683 "src/parser.cpp"
    break;

  case 31:
#line 116 "src/parser.y"
       {
        symbol_entry *t;
        t = identifiers->lookup(yyvsp[0]->sym_ptr->name);
        yyval = ast->NewRoot(NODE_EXPR, -1, t);
        }
#line 1693 "src/parser.cpp"
    break;

  case 32:
#line 121 "src/parser.y"
                                 {
        yyval = ast->NewRoot(NODE_EXPR,OP_ARRAY, NULL, yyvsp[-3], yyvsp[-1]);
        yyval->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        yyval->sym_ptr->tp = yyvsp[-3]->sym_ptr->tp->operand;
        yyval->sym_ptr->array_value = yyvsp[-3]->sym_ptr->array_value;
    }
#line 1704 "src/parser.cpp"
    break;

  case 33:
#line 130 "src/parser.y"
                      {yyval = yyvsp[-1];}
#line 1710 "src/parser.cpp"
    break;

  case 34:
#line 131 "src/parser.y"
           {yyval = yyvsp[0];}
#line 1716 "src/parser.cpp"
    break;

  case 35:
#line 132 "src/parser.y"
              {yyval = yyvsp[0];}
#line 1722 "src/parser.cpp"
    break;

  case 36:
#line 133 "src/parser.y"
               {yyval = yyvsp[0];}
#line 1728 "src/parser.cpp"
    break;

  case 37:
#line 134 "src/parser.y"
                                   {
        symbol_entry *t;
        t = functions->lookup(yyvsp[-3]->sym_ptr->name);
        if(t == NULL)
        {
            t = functions->install(yyvsp[-3]->sym_ptr->name);
            t->undefined = true;
            t->tp = typesys.int_type;
            t->tp = typesys.func(t->tp, NULL);
        }
        yyval = ast->NewRoot(NODE_EXPR,OP_FUNC, NULL, yyvsp[-1]);
        yyval->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        yyval->sym_ptr->tp = t->tp->operand;
        yyval->sym_ptr->undefined = t->undefined;
        yyval->sym_ptr->name = yyvsp[-3]->sym_ptr->name;
    }
#line 1749 "src/parser.cpp"
    break;

  case 38:
#line 150 "src/parser.y"
                       {
        symbol_entry *t;
        t = functions->lookup(yyvsp[-2]->sym_ptr->name);
        Node *c = NULL;
        if(t == NULL)
        {
            t = functions->install(yyvsp[-2]->sym_ptr->name);
            t->undefined = true;
            t->tp = typesys.int_type;
            t->tp = typesys.func(t->tp, NULL);
        }
        if(yyvsp[-2]->sym_ptr->name == "_sysy_starttime" || yyvsp[-2]->sym_ptr->name == "_sysy_stoptime")
        {
            c = ast->NewRoot(NODE_EXPR, -1);
            c->sym_ptr = constants->find_constant(0);
        }
        yyval = ast->NewRoot(NODE_EXPR,OP_FUNC, NULL, c);
        yyval->sym_ptr = symbol_table::gen_temp(typesys.int_type);
        yyval->sym_ptr->tp = t->tp->operand;
        yyval->sym_ptr->undefined = t->undefined;
        yyval->sym_ptr->name = yyvsp[-2]->sym_ptr->name;
    }
#line 1776 "src/parser.cpp"
    break;

  case 39:
#line 175 "src/parser.y"
               {yyval = yyvsp[0];}
#line 1782 "src/parser.cpp"
    break;

  case 40:
#line 177 "src/parser.y"
                {yyval = yyvsp[0];}
#line 1788 "src/parser.cpp"
    break;

  case 41:
#line 179 "src/parser.y"
                 {
        yyval = ast->NewRoot(NODE_EXPR,OP_NOT, NULL, yyvsp[0]);
    }
#line 1796 "src/parser.cpp"
    break;

  case 42:
#line 183 "src/parser.y"
                  {
        yyval = ast->NewRoot(NODE_EXPR,OP_UMINUS, NULL, yyvsp[0]);
    }
#line 1804 "src/parser.cpp"
    break;

  case 43:
#line 189 "src/parser.y"
             {yyval = yyvsp[0];}
#line 1810 "src/parser.cpp"
    break;

  case 44:
#line 192 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR,OP_MUL, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1818 "src/parser.cpp"
    break;

  case 45:
#line 197 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR,OP_DIV, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1826 "src/parser.cpp"
    break;

  case 46:
#line 202 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR,OP_MOD, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1834 "src/parser.cpp"
    break;

  case 47:
#line 208 "src/parser.y"
           {yyval = yyvsp[0];}
#line 1840 "src/parser.cpp"
    break;

  case 48:
#line 211 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_ADD, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1848 "src/parser.cpp"
    break;

  case 49:
#line 216 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_MINUS, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1856 "src/parser.cpp"
    break;

  case 50:
#line 222 "src/parser.y"
           {yyval = yyvsp[0];}
#line 1862 "src/parser.cpp"
    break;

  case 51:
#line 225 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_L, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1870 "src/parser.cpp"
    break;

  case 52:
#line 230 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_G, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1878 "src/parser.cpp"
    break;

  case 53:
#line 235 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_LE, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1886 "src/parser.cpp"
    break;

  case 54:
#line 240 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_GE, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1894 "src/parser.cpp"
    break;

  case 55:
#line 246 "src/parser.y"
           {yyval=yyvsp[0];}
#line 1900 "src/parser.cpp"
    break;

  case 56:
#line 249 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_E, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1908 "src/parser.cpp"
    break;

  case 57:
#line 254 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_NE, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1916 "src/parser.cpp"
    break;

  case 58:
#line 260 "src/parser.y"
          {yyval = yyvsp[0];}
#line 1922 "src/parser.cpp"
    break;

  case 59:
#line 263 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_AND, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1930 "src/parser.cpp"
    break;

  case 60:
#line 269 "src/parser.y"
            {yyval = yyvsp[0];}
#line 1936 "src/parser.cpp"
    break;

  case 61:
#line 272 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, OP_OR, NULL, yyvsp[-2], yyvsp[0]);
    }
#line 1944 "src/parser.cpp"
    break;

  case 62:
#line 278 "src/parser.y"
           {
        yyval = yyvsp[0];
        }
#line 1952 "src/parser.cpp"
    break;

  case 63:
#line 284 "src/parser.y"
        {yyval = yyvsp[0];}
#line 1958 "src/parser.cpp"
    break;

  case 64:
#line 286 "src/parser.y"
                          {
        yyval = yyvsp[-2];
        yyval->addSibling(yyvsp[0]);
    }
#line 1967 "src/parser.cpp"
    break;

  case 65:
#line 292 "src/parser.y"
          {
        global_type = typesys.int_type;
    }
#line 1975 "src/parser.cpp"
    break;

  case 66:
#line 295 "src/parser.y"
           {
        global_type = typesys.void_type;
    }
#line 1983 "src/parser.cpp"
    break;

  case 67:
#line 301 "src/parser.y"
              {yyval=yyvsp[0];}
#line 1989 "src/parser.cpp"
    break;

  case 68:
#line 303 "src/parser.y"
            {yyval=yyvsp[0];}
#line 1995 "src/parser.cpp"
    break;

  case 69:
#line 307 "src/parser.y"
               {global_type = typesys.qual(global_type);}
#line 2001 "src/parser.cpp"
    break;

  case 70:
#line 307 "src/parser.y"
                                                                                 {
        yyval = yyvsp[-1];
    }
#line 2009 "src/parser.cpp"
    break;

  case 71:
#line 313 "src/parser.y"
            {yyval=yyvsp[0];}
#line 2015 "src/parser.cpp"
    break;

  case 72:
#line 316 "src/parser.y"
    {
        yyval=yyvsp[-2];
        yyval->addSibling(yyvsp[0]);
    }
#line 2024 "src/parser.cpp"
    break;

  case 73:
#line 323 "src/parser.y"
                           {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-2]->sym_ptr->name);
        se->tp = global_type;
        se->constant = Ast::get_constExpVal(yyvsp[0]);
        if(se->scope > GLOBAL)
        {
            se->label = symbol_table::gen_label(1);
            ostringstream os;
            os << se->name << "." << se->label;
            symbol_entry *t = globals->install(os.str());
            t->tp = se->tp;
            t->constant = se->constant;
        }

        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2048 "src/parser.cpp"
    break;

  case 74:
#line 343 "src/parser.y"
                                     {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-3]->sym_ptr->name);
        se->tp = global_type;
        vector<int> s;
        Node *p = yyvsp[-2];
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
        yyvsp[0]->sym_ptr->tp = se->tp;
        yyvsp[0]->sym_ptr->array_value = se->array_value;
        Ast::set_initVal(yyvsp[0],0);
        if(se->scope > GLOBAL)
        {
            se->label = symbol_table::gen_label(1);
            ostringstream os;
            os << se->name << "." << se->label;
            symbol_entry *t = globals->install(os.str());
            t->tp = se->tp;
            t->array_value = se->array_value;
        }

        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2089 "src/parser.cpp"
    break;

  case 75:
#line 382 "src/parser.y"
             {yyval=yyvsp[0];}
#line 2095 "src/parser.cpp"
    break;

  case 76:
#line 384 "src/parser.y"
                  {
        yyval = ast->NewRoot(NODE_EXPR,OP_INITLIST);
        yyval->sym_ptr = new symbol_entry;
    }
#line 2104 "src/parser.cpp"
    break;

  case 77:
#line 389 "src/parser.y"
                                   {
        yyval = ast->NewRoot(NODE_EXPR,OP_INITLIST, NULL, yyvsp[-1]);
        yyval->sym_ptr = new symbol_entry;
    }
#line 2113 "src/parser.cpp"
    break;

  case 78:
#line 396 "src/parser.y"
                 {
        if(yyvsp[0]->kind_kind != OP_INITLIST)
            yyvsp[0] = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), yyvsp[0]);
        yyval=yyvsp[0];
    }
#line 2123 "src/parser.cpp"
    break;

  case 79:
#line 402 "src/parser.y"
                                        {
        yyval = yyvsp[-2];
        if(yyvsp[0]->kind_kind != OP_INITLIST)
            yyvsp[0] = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), yyvsp[0]);
        yyval->addSibling(yyvsp[0]);
    }
#line 2134 "src/parser.cpp"
    break;

  case 80:
#line 411 "src/parser.y"
                               {yyval = yyvsp[-1];}
#line 2140 "src/parser.cpp"
    break;

  case 81:
#line 413 "src/parser.y"
                                         {
        yyval = yyvsp[-3];
        yyval->addSibling(yyvsp[-1]);
    }
#line 2149 "src/parser.cpp"
    break;

  case 82:
#line 420 "src/parser.y"
                              {
        yyval = yyvsp[-1];
    }
#line 2157 "src/parser.cpp"
    break;

  case 83:
#line 426 "src/parser.y"
                            {
        yyval = yyvsp[-2];
        yyval->addSibling(yyvsp[0]);
    }
#line 2166 "src/parser.cpp"
    break;

  case 84:
#line 431 "src/parser.y"
          {
        yyval = yyvsp[0];
    }
#line 2174 "src/parser.cpp"
    break;

  case 85:
#line 437 "src/parser.y"
       {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[0]->sym_ptr->name);
        se->tp = global_type;
        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2187 "src/parser.cpp"
    break;

  case 86:
#line 446 "src/parser.y"
                 {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-1]->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = yyvsp[0];
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
        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2214 "src/parser.cpp"
    break;

  case 87:
#line 469 "src/parser.y"
                      {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-2]->sym_ptr->name);
        se->tp = global_type;
        if(se->scope == GLOBAL)
        {
            se->constant = Ast::get_constExpVal(yyvsp[0]);
            yyval = ast->NewRoot(NODE_DECL,-1, se);
        }
        else 
            yyval = ast->NewRoot(NODE_DECL,-1, se, yyvsp[0]);
    }
#line 2233 "src/parser.cpp"
    break;

  case 88:
#line 484 "src/parser.y"
                                {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-3]->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = yyvsp[-2];
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
        yyvsp[0]->sym_ptr->tp = se->tp;
        if(se->scope == GLOBAL)
        {
            se->array_value = new int[se->tp->size / se->tp->align];
            memset(se->array_value, 0, se->tp->size);
            yyvsp[0]->sym_ptr->tp = se->tp;
            yyvsp[0]->sym_ptr->array_value = se->array_value;
            Ast::set_initVal(yyvsp[0],0);
            yyval = ast->NewRoot(NODE_DECL,-1, se);
        }
        else
            yyval = ast->NewRoot(NODE_DECL,-1, se, yyvsp[0]);
    }
#line 2271 "src/parser.cpp"
    break;

  case 89:
#line 520 "src/parser.y"
        {yyval=yyvsp[0];}
#line 2277 "src/parser.cpp"
    break;

  case 90:
#line 522 "src/parser.y"
                  {
        yyval = ast->NewRoot(NODE_EXPR,OP_INITLIST);
        yyval->sym_ptr = new symbol_entry;
        }
#line 2286 "src/parser.cpp"
    break;

  case 91:
#line 527 "src/parser.y"
                              {
        yyval = ast->NewRoot(NODE_EXPR,OP_INITLIST, NULL, yyvsp[-1]);
        yyval->sym_ptr = new symbol_entry;
    }
#line 2295 "src/parser.cpp"
    break;

  case 92:
#line 534 "src/parser.y"
            {
        if(yyvsp[0]->kind_kind != OP_INITLIST)
            yyvsp[0] = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), yyvsp[0]);
        yyval=yyvsp[0];
    }
#line 2305 "src/parser.cpp"
    break;

  case 93:
#line 540 "src/parser.y"
                              {
        yyval = yyvsp[-2];
        if(yyvsp[0]->kind_kind != OP_INITLIST)
            yyvsp[0] = ast->NewRoot(NODE_EXPR,-1, symbol_table::gen_temp(typesys.int_type), yyvsp[0]);
        yyval=yyvsp[-2];
        yyval->addSibling(yyvsp[0]);
    }
#line 2317 "src/parser.cpp"
    break;

  case 94:
#line 550 "src/parser.y"
            {
        symbol_entry *se;
        se = functions->install(yyvsp[0]->sym_ptr->name);
        se->tp = typesys.func(global_type,NULL);
        symbol_table::enter_scope();}
#line 2327 "src/parser.cpp"
    break;

  case 95:
#line 557 "src/parser.y"
    {
        symbol_entry *t;
        t = functions->lookup(yyvsp[-5]->sym_ptr->name);
        t->undefined = false;
        int num_of_params = 0;
        Node *p = yyvsp[-2];
        while(p)
        {
            num_of_params++;
            p = p->sibling;
        }
        if(num_of_params > 0)
        {
            t->tp->func_params = new symbol_entry*[num_of_params + 1];
            p = yyvsp[-2];
            int i = 0;
            while(p)
            {
                t->tp->func_params[i] = p->sym_ptr;
                i++;
                p = p->sibling;
            }
            t->tp->func_params[num_of_params] = NULL;
        }
        yyval = ast->NewRoot(NODE_STMT, STMT_FUNC, t, yyvsp[-2], yyvsp[0]);
        symbol_table::leave_scope();
    }
#line 2359 "src/parser.cpp"
    break;

  case 96:
#line 587 "src/parser.y"
                {yyval=yyvsp[0];}
#line 2365 "src/parser.cpp"
    break;

  case 97:
#line 588 "src/parser.y"
             {yyval = NULL;}
#line 2371 "src/parser.cpp"
    break;

  case 98:
#line 592 "src/parser.y"
                                 {
        yyval = yyvsp[-2];
        Node *p = yyvsp[-2];
        while(p->sibling)
            p = p->sibling;
        p->sibling = yyvsp[0];
    }
#line 2383 "src/parser.cpp"
    break;

  case 99:
#line 600 "src/parser.y"
               {yyval = yyvsp[0];}
#line 2389 "src/parser.cpp"
    break;

  case 100:
#line 604 "src/parser.y"
            {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[0]->sym_ptr->name);
        se->tp = global_type;

        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2403 "src/parser.cpp"
    break;

  case 101:
#line 614 "src/parser.y"
                       {
        symbol_entry *se;
        if(symbol_table::level > identifiers->table_level)
            identifiers = identifiers->new_table(symbol_table::level);
        se = identifiers->install(yyvsp[-1]->sym_ptr->name);
        se->tp = global_type;

        vector<int> s;
        Node *p = yyvsp[0]->sibling;
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

        yyval = ast->NewRoot(NODE_DECL,-1, se);
    }
#line 2432 "src/parser.cpp"
    break;

  case 102:
#line 642 "src/parser.y"
    {
        yyval = ast->NewRoot(NODE_EXPR, -1);
    }
#line 2440 "src/parser.cpp"
    break;

  case 103:
#line 646 "src/parser.y"
                                     {
        yyval = yyvsp[-3];
        yyval->addSibling(yyvsp[-1]);
    }
#line 2449 "src/parser.cpp"
    break;


#line 2453 "src/parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 651 "src/parser.y"


int yyerror(char const* message)
{
    std::cout<<message<<std::endl;
    return -1;
}

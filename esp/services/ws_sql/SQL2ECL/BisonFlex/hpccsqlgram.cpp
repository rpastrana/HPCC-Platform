/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1





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

#include "hpccsqlgram.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOKEN_ROOT = 3,                 /* TOKEN_ROOT  */
  YYSYMBOL_TOKEN_SELECT_STATEMENT = 4,     /* TOKEN_SELECT_STATEMENT  */
  YYSYMBOL_TOKEN_CALL_STATEMENT = 5,       /* TOKEN_CALL_STATEMENT  */
  YYSYMBOL_TOKEN_CREATE_LOAD_TABLE_STATEMENT = 6, /* TOKEN_CREATE_LOAD_TABLE_STATEMENT  */
  YYSYMBOL_TOKEN_CREATE_INDEX_STATEMENT = 7, /* TOKEN_CREATE_INDEX_STATEMENT  */
  YYSYMBOL_TOKEN_CREATE_TABLE = 8,         /* TOKEN_CREATE_TABLE  */
  YYSYMBOL_TOKEN_DONOT_OVERWRITE = 9,      /* TOKEN_DONOT_OVERWRITE  */
  YYSYMBOL_TOKEN_OVERWRITE = 10,           /* TOKEN_OVERWRITE  */
  YYSYMBOL_TOKEN_LOAD_TABLE = 11,          /* TOKEN_LOAD_TABLE  */
  YYSYMBOL_TOKEN_FROM_LIST = 12,           /* TOKEN_FROM_LIST  */
  YYSYMBOL_TOKEN_FROM_TABLE = 13,          /* TOKEN_FROM_TABLE  */
  YYSYMBOL_TOKEN_PROC_NAME = 14,           /* TOKEN_PROC_NAME  */
  YYSYMBOL_TOKEN_PROC_PARAMS = 15,         /* TOKEN_PROC_PARAMS  */
  YYSYMBOL_TOKEN_ALIAS = 16,               /* TOKEN_ALIAS  */
  YYSYMBOL_TOKEN_INNER_JOIN = 17,          /* TOKEN_INNER_JOIN  */
  YYSYMBOL_TOKEN_OUTTER_JOIN = 18,         /* TOKEN_OUTTER_JOIN  */
  YYSYMBOL_TOKEN_INDEX_HINT = 19,          /* TOKEN_INDEX_HINT  */
  YYSYMBOL_TOKEN_AVOID_INDEX = 20,         /* TOKEN_AVOID_INDEX  */
  YYSYMBOL_TOKEN_COLUMN = 21,              /* TOKEN_COLUMN  */
  YYSYMBOL_TOKEN_LISTEXP = 22,             /* TOKEN_LISTEXP  */
  YYSYMBOL_TOKEN_FUNCEXP = 23,             /* TOKEN_FUNCEXP  */
  YYSYMBOL_TOKEN_PARAMPLACEHOLDER = 24,    /* TOKEN_PARAMPLACEHOLDER  */
  YYSYMBOL_TOKEN_COLUMNWILDCARD = 25,      /* TOKEN_COLUMNWILDCARD  */
  YYSYMBOL_TOKEN_TABLE_SCHEMA = 26,        /* TOKEN_TABLE_SCHEMA  */
  YYSYMBOL_TOKEN_COLUMN_DEF_LIST = 27,     /* TOKEN_COLUMN_DEF_LIST  */
  YYSYMBOL_TOKEN_COLUMN_DEF = 28,          /* TOKEN_COLUMN_DEF  */
  YYSYMBOL_TOKEN_LANDING_ZONE = 29,        /* TOKEN_LANDING_ZONE  */
  YYSYMBOL_TOKEN_VARIABLE_FILE = 30,       /* TOKEN_VARIABLE_FILE  */
  YYSYMBOL_TOKEN_VAR_SEPERATOR = 31,       /* TOKEN_VAR_SEPERATOR  */
  YYSYMBOL_TOKEN_VAR_TERMINATOR = 32,      /* TOKEN_VAR_TERMINATOR  */
  YYSYMBOL_TOKEN_VAR_ENCLOSED = 33,        /* TOKEN_VAR_ENCLOSED  */
  YYSYMBOL_TOKEN_VAR_ESCAPED = 34,         /* TOKEN_VAR_ESCAPED  */
  YYSYMBOL_ADD_SYM = 35,                   /* ADD_SYM  */
  YYSYMBOL_ALL = 36,                       /* ALL  */
  YYSYMBOL_ANY = 37,                       /* ANY  */
  YYSYMBOL_AS_SYM = 38,                    /* AS_SYM  */
  YYSYMBOL_ASC = 39,                       /* ASC  */
  YYSYMBOL_ASCII_SYM = 40,                 /* ASCII_SYM  */
  YYSYMBOL_AT_SYM = 41,                    /* AT_SYM  */
  YYSYMBOL_AVG = 42,                       /* AVG  */
  YYSYMBOL_BETWEEN = 43,                   /* BETWEEN  */
  YYSYMBOL_BINARY_SYM = 44,                /* BINARY_SYM  */
  YYSYMBOL_BIT_SYM = 45,                   /* BIT_SYM  */
  YYSYMBOL_BOOL_SYM = 46,                  /* BOOL_SYM  */
  YYSYMBOL_BOOLEAN_SYM = 47,               /* BOOLEAN_SYM  */
  YYSYMBOL_BY_SYM = 48,                    /* BY_SYM  */
  YYSYMBOL_CALL_SYM = 49,                  /* CALL_SYM  */
  YYSYMBOL_CREATE_SYM = 50,                /* CREATE_SYM  */
  YYSYMBOL_COLUMN_SYM = 51,                /* COLUMN_SYM  */
  YYSYMBOL_COMMENT_SYM = 52,               /* COMMENT_SYM  */
  YYSYMBOL_CONTAINS_SYM = 53,              /* CONTAINS_SYM  */
  YYSYMBOL_COUNT = 54,                     /* COUNT  */
  YYSYMBOL_DATA_SYM = 55,                  /* DATA_SYM  */
  YYSYMBOL_DESC = 56,                      /* DESC  */
  YYSYMBOL_DISTINCT = 57,                  /* DISTINCT  */
  YYSYMBOL_EXISTS_SYM = 58,                /* EXISTS_SYM  */
  YYSYMBOL_FALSE_SYM = 59,                 /* FALSE_SYM  */
  YYSYMBOL_FOR_SYM = 60,                   /* FOR_SYM  */
  YYSYMBOL_FROM = 61,                      /* FROM  */
  YYSYMBOL_GROUP_SYM = 62,                 /* GROUP_SYM  */
  YYSYMBOL_HAVING = 63,                    /* HAVING  */
  YYSYMBOL_IF_SYM = 64,                    /* IF_SYM  */
  YYSYMBOL_IN_SYM = 65,                    /* IN_SYM  */
  YYSYMBOL_INDEX_SYM = 66,                 /* INDEX_SYM  */
  YYSYMBOL_INFILE_SYM = 67,                /* INFILE_SYM  */
  YYSYMBOL_INTO_SYM = 68,                  /* INTO_SYM  */
  YYSYMBOL_IS_SYM = 69,                    /* IS_SYM  */
  YYSYMBOL_JOIN_SYM = 70,                  /* JOIN_SYM  */
  YYSYMBOL_LEFT = 71,                      /* LEFT  */
  YYSYMBOL_LIKE = 72,                      /* LIKE  */
  YYSYMBOL_LIMIT = 73,                     /* LIMIT  */
  YYSYMBOL_LOAD_SYM = 74,                  /* LOAD_SYM  */
  YYSYMBOL_LOWER = 75,                     /* LOWER  */
  YYSYMBOL_MAX_SYM = 76,                   /* MAX_SYM  */
  YYSYMBOL_MIN_SYM = 77,                   /* MIN_SYM  */
  YYSYMBOL_MOD = 78,                       /* MOD  */
  YYSYMBOL_NOT_SYM = 79,                   /* NOT_SYM  */
  YYSYMBOL_NULL_SYM = 80,                  /* NULL_SYM  */
  YYSYMBOL_OFFSET_SYM = 81,                /* OFFSET_SYM  */
  YYSYMBOL_ON = 82,                        /* ON  */
  YYSYMBOL_OR_SYM = 83,                    /* OR_SYM  */
  YYSYMBOL_ORDER_SYM = 84,                 /* ORDER_SYM  */
  YYSYMBOL_OUT_SYM = 85,                   /* OUT_SYM  */
  YYSYMBOL_OUTER = 86,                     /* OUTER  */
  YYSYMBOL_POWER = 87,                     /* POWER  */
  YYSYMBOL_REPLACE_SYM = 88,               /* REPLACE_SYM  */
  YYSYMBOL_RIGHT = 89,                     /* RIGHT  */
  YYSYMBOL_SELECT = 90,                    /* SELECT  */
  YYSYMBOL_SUM = 91,                       /* SUM  */
  YYSYMBOL_TABLE_SYM = 92,                 /* TABLE_SYM  */
  YYSYMBOL_TRUE_SYM = 93,                  /* TRUE_SYM  */
  YYSYMBOL_UNION = 94,                     /* UNION  */
  YYSYMBOL_UPPER = 95,                     /* UPPER  */
  YYSYMBOL_WHERE = 96,                     /* WHERE  */
  YYSYMBOL_AND_SYM = 97,                   /* AND_SYM  */
  YYSYMBOL_EQ_SYM = 98,                    /* EQ_SYM  */
  YYSYMBOL_NE = 99,                        /* NE  */
  YYSYMBOL_LTH = 100,                      /* LTH  */
  YYSYMBOL_GTH = 101,                      /* GTH  */
  YYSYMBOL_LE = 102,                       /* LE  */
  YYSYMBOL_GE = 103,                       /* GE  */
  YYSYMBOL_PLUS = 104,                     /* PLUS  */
  YYSYMBOL_MINUS = 105,                    /* MINUS  */
  YYSYMBOL_ASTERISK = 106,                 /* ASTERISK  */
  YYSYMBOL_DIVIDE = 107,                   /* DIVIDE  */
  YYSYMBOL_MOD_SYM = 108,                  /* MOD_SYM  */
  YYSYMBOL_POWER_OP = 109,                 /* POWER_OP  */
  YYSYMBOL_LPAREN = 110,                   /* LPAREN  */
  YYSYMBOL_RPAREN = 111,                   /* RPAREN  */
  YYSYMBOL_LBRACK = 112,                   /* LBRACK  */
  YYSYMBOL_RBRACK = 113,                   /* RBRACK  */
  YYSYMBOL_LCURLY = 114,                   /* LCURLY  */
  YYSYMBOL_RCURLY = 115,                   /* RCURLY  */
  YYSYMBOL_COMMA = 116,                    /* COMMA  */
  YYSYMBOL_SEMI = 117,                     /* SEMI  */
  YYSYMBOL_COLON = 118,                    /* COLON  */
  YYSYMBOL_DOT = 119,                      /* DOT  */
  YYSYMBOL_QUESTION = 120,                 /* QUESTION  */
  YYSYMBOL_DOLLAR = 121,                   /* DOLLAR  */
  YYSYMBOL_DQUOTE = 122,                   /* DQUOTE  */
  YYSYMBOL_VERTBAR = 123,                  /* VERTBAR  */
  YYSYMBOL_BITAND = 124,                   /* BITAND  */
  YYSYMBOL_NEGATION = 125,                 /* NEGATION  */
  YYSYMBOL_STRING_LITERAL = 126,           /* STRING_LITERAL  */
  YYSYMBOL_ID = 127,                       /* ID  */
  YYSYMBOL_QUOTED_ID = 128,                /* QUOTED_ID  */
  YYSYMBOL_HEX_DIGIT = 129,                /* HEX_DIGIT  */
  YYSYMBOL_INTEGER_NUM = 130,              /* INTEGER_NUM  */
  YYSYMBOL_DECIMAL_NUM = 131,              /* DECIMAL_NUM  */
  YYSYMBOL_YYACCEPT = 132,                 /* $accept  */
  YYSYMBOL_root_statement = 133,           /* root_statement  */
  YYSYMBOL_select_statement = 134,         /* select_statement  */
  YYSYMBOL_select_portion = 135,           /* select_portion  */
  YYSYMBOL_from_portion = 136,             /* from_portion  */
  YYSYMBOL_where_clause = 137,             /* where_clause  */
  YYSYMBOL_groupby_clause = 138,           /* groupby_clause  */
  YYSYMBOL_having_clause = 139,            /* having_clause  */
  YYSYMBOL_orderby_clause = 140,           /* orderby_clause  */
  YYSYMBOL_limit_clause = 141,             /* limit_clause  */
  YYSYMBOL_select_list = 142,              /* select_list  */
  YYSYMBOL_select_item = 143,              /* select_item  */
  YYSYMBOL_table_references = 144,         /* table_references  */
  YYSYMBOL_table_reference = 145,          /* table_reference  */
  YYSYMBOL_column_spec = 146,              /* column_spec  */
  YYSYMBOL_expression = 147,               /* expression  */
  YYSYMBOL_literal_value = 148,            /* literal_value  */
  YYSYMBOL_call_statement = 149,           /* call_statement  */
  YYSYMBOL_create_load_table_statement = 150 /* create_load_table_statement  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Second part of user prologue.  */
#line 30 "hpccsqlgram.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration to avoid circular dependency
struct HPCCSQLTreeWalker;

// Define scanner type for reentrant parser
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef struct ASTNode {
    int nodeType;
    char* value;
    struct ASTNode** children;
    int childCount;
    int childCapacity;
} ASTNode;

ASTNode* createASTNode(int type, const char* value);
ASTNode* addChild(ASTNode* parent, ASTNode* child);
void freeASTNode(ASTNode* node);

// Function to scan string (will be implemented in lexer)
void hpccsql_scan_string(const char* str, yyscan_t scanner);

// Forward declarations
void yyerror(yyscan_t scanner, struct HPCCSQLTreeWalker* context, const char* msg);
int hpccsqllex(YYSTYPE* yylval, yyscan_t yyscanner);


#line 288 "hpccsqlgram.cpp"


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

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
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
typedef yytype_int8 yy_state_t;

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
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

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
#define YYFINAL  25
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   147

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  132
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  58
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  92

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   386


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   130,   130,   134,   138,   145,   156,   164,   165,   169,
     170,   174,   175,   179,   180,   184,   185,   189,   190,   191,
     192,   196,   197,   198,   202,   203,   207,   208,   209,   213,
     214,   218,   219,   223,   224,   225,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   251,   252,   253,   254,   255,   256,   260,   266
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_ROOT",
  "TOKEN_SELECT_STATEMENT", "TOKEN_CALL_STATEMENT",
  "TOKEN_CREATE_LOAD_TABLE_STATEMENT", "TOKEN_CREATE_INDEX_STATEMENT",
  "TOKEN_CREATE_TABLE", "TOKEN_DONOT_OVERWRITE", "TOKEN_OVERWRITE",
  "TOKEN_LOAD_TABLE", "TOKEN_FROM_LIST", "TOKEN_FROM_TABLE",
  "TOKEN_PROC_NAME", "TOKEN_PROC_PARAMS", "TOKEN_ALIAS",
  "TOKEN_INNER_JOIN", "TOKEN_OUTTER_JOIN", "TOKEN_INDEX_HINT",
  "TOKEN_AVOID_INDEX", "TOKEN_COLUMN", "TOKEN_LISTEXP", "TOKEN_FUNCEXP",
  "TOKEN_PARAMPLACEHOLDER", "TOKEN_COLUMNWILDCARD", "TOKEN_TABLE_SCHEMA",
  "TOKEN_COLUMN_DEF_LIST", "TOKEN_COLUMN_DEF", "TOKEN_LANDING_ZONE",
  "TOKEN_VARIABLE_FILE", "TOKEN_VAR_SEPERATOR", "TOKEN_VAR_TERMINATOR",
  "TOKEN_VAR_ENCLOSED", "TOKEN_VAR_ESCAPED", "ADD_SYM", "ALL", "ANY",
  "AS_SYM", "ASC", "ASCII_SYM", "AT_SYM", "AVG", "BETWEEN", "BINARY_SYM",
  "BIT_SYM", "BOOL_SYM", "BOOLEAN_SYM", "BY_SYM", "CALL_SYM", "CREATE_SYM",
  "COLUMN_SYM", "COMMENT_SYM", "CONTAINS_SYM", "COUNT", "DATA_SYM", "DESC",
  "DISTINCT", "EXISTS_SYM", "FALSE_SYM", "FOR_SYM", "FROM", "GROUP_SYM",
  "HAVING", "IF_SYM", "IN_SYM", "INDEX_SYM", "INFILE_SYM", "INTO_SYM",
  "IS_SYM", "JOIN_SYM", "LEFT", "LIKE", "LIMIT", "LOAD_SYM", "LOWER",
  "MAX_SYM", "MIN_SYM", "MOD", "NOT_SYM", "NULL_SYM", "OFFSET_SYM", "ON",
  "OR_SYM", "ORDER_SYM", "OUT_SYM", "OUTER", "POWER", "REPLACE_SYM",
  "RIGHT", "SELECT", "SUM", "TABLE_SYM", "TRUE_SYM", "UNION", "UPPER",
  "WHERE", "AND_SYM", "EQ_SYM", "NE", "LTH", "GTH", "LE", "GE", "PLUS",
  "MINUS", "ASTERISK", "DIVIDE", "MOD_SYM", "POWER_OP", "LPAREN", "RPAREN",
  "LBRACK", "RBRACK", "LCURLY", "RCURLY", "COMMA", "SEMI", "COLON", "DOT",
  "QUESTION", "DOLLAR", "DQUOTE", "VERTBAR", "BITAND", "NEGATION",
  "STRING_LITERAL", "ID", "QUOTED_ID", "HEX_DIGIT", "INTEGER_NUM",
  "DECIMAL_NUM", "$accept", "root_statement", "select_statement",
  "select_portion", "from_portion", "where_clause", "groupby_clause",
  "having_clause", "orderby_clause", "limit_clause", "select_list",
  "select_item", "table_references", "table_reference", "column_spec",
  "expression", "literal_value", "call_statement",
  "create_load_table_statement", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-7)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -45,  -120,   -79,   -56,    19,  -121,     2,  -121,  -121,   -92,
    -107,   -49,  -121,  -121,  -121,  -121,  -121,   -98,  -121,  -121,
    -121,   -94,  -121,  -121,  -121,  -121,  -116,   -73,   -86,  -121,
     -94,  -100,   -49,  -121,  -121,   -88,  -121,   -42,   -32,  -121,
    -121,  -121,  -116,   -42,  -121,    18,  -121,   -16,   -29,  -121,
      -7,   -42,   -42,   -42,   -42,   -42,   -42,   -42,   -42,   -42,
     -42,   -42,   -42,  -112,   -42,   -51,  -121,    18,    18,    18,
      18,    18,    18,    18,    18,    18,    18,    18,    18,  -121,
      18,   -12,   -34,  -112,   -90,  -121,   -30,   -40,  -121,  -121,
     -87,  -121
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     2,    10,     3,     4,     0,
       0,     0,    55,    56,    54,    28,    51,    33,    34,    52,
      53,     7,    24,    26,    27,     1,     0,    12,     0,    58,
       8,     0,     0,    31,    32,     9,    29,     0,    14,    57,
      35,    25,     0,     0,    36,    11,    37,     0,    16,    30,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    50,    48,    49,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    13,
      15,     0,    23,     0,     0,     5,    17,    21,    18,    19,
       0,    22
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
      31,    14,  -121,     5,    -3,    83,     3,  -121,  -121
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,    27,    38,    48,    65,    82,    85,
      21,    22,    35,    36,    44,    45,    46,     7,     8
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      23,    11,    -6,    12,     1,     2,    24,     9,    23,    88,
      12,    33,    34,    10,    24,    17,    18,    12,    28,    25,
      29,    31,    32,    37,    13,    39,    89,    40,    42,    23,
      47,    13,    63,    81,    64,    24,    83,    14,    13,    84,
      87,    90,    30,    91,    14,     3,    41,    49,     0,     0,
      15,    14,     0,     0,     0,     0,     0,    15,     0,     0,
      79,     0,     0,    26,     0,     0,     0,     0,    43,     0,
      16,    17,    18,     0,    19,    20,    51,    16,    17,    18,
      86,    19,    20,     0,    16,    17,    18,     0,    19,    20,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    51,     0,     0,    66,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    50,     0,     0,     0,
       0,     0,     0,     0,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,     0,    80
};

static const yytype_int16 yycheck[] =
{
       3,    57,     0,    59,    49,    50,     3,   127,    11,    39,
      59,   127,   128,    92,    11,   127,   128,    59,   110,     0,
     127,   119,   116,    96,    80,   111,    56,   127,   116,    32,
      62,    80,    48,    84,    63,    32,    48,    93,    80,    73,
     130,    81,    11,   130,    93,    90,    32,    42,    -1,    -1,
     106,    93,    -1,    -1,    -1,    -1,    -1,   106,    -1,    -1,
      63,    -1,    -1,    61,    -1,    -1,    -1,    -1,   110,    -1,
     126,   127,   128,    -1,   130,   131,    83,   126,   127,   128,
      83,   130,   131,    -1,   126,   127,   128,    -1,   130,   131,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,    83,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    64
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    49,    50,    90,   133,   134,   135,   149,   150,   127,
      92,    57,    59,    80,    93,   106,   126,   127,   128,   130,
     131,   142,   143,   146,   148,     0,    61,   136,   110,   127,
     142,   119,   116,   127,   128,   144,   145,    96,   137,   111,
     127,   143,   116,   110,   146,   147,   148,    62,   138,   145,
     147,    83,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,    48,    63,   139,   111,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   146,
     147,    84,   140,    48,    73,   141,   146,   130,    39,    56,
      81,   130
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   132,   133,   133,   133,   134,   134,   135,   135,   136,
     136,   137,   137,   138,   138,   139,   139,   140,   140,   140,
     140,   141,   141,   141,   142,   142,   143,   143,   143,   144,
     144,   145,   145,   146,   146,   146,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   148,   148,   148,   148,   148,   148,   149,   150
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     7,     1,     2,     3,     2,
       0,     2,     0,     3,     0,     2,     0,     3,     4,     4,
       0,     2,     4,     0,     1,     3,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     4,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
        yyerror (scanner, context, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner, context); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, HPCCSQLTreeWalker* context)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (scanner);
  YY_USE (context);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, HPCCSQLTreeWalker* context)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner, context);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, yyscan_t scanner, HPCCSQLTreeWalker* context)
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
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner, context);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, context); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, yyscan_t scanner, HPCCSQLTreeWalker* context)
{
  YY_USE (yyvaluep);
  YY_USE (scanner);
  YY_USE (context);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t scanner, HPCCSQLTreeWalker* context)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
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
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  case 2: /* root_statement: select_statement  */
#line 130 "hpccsqlgram.y"
                        { 
        (yyval.node) = createASTNode(TOKEN_SELECT_STATEMENT, NULL); 
        addChild((yyval.node), (yyvsp[0].node)); 
    }
#line 1357 "hpccsqlgram.cpp"
    break;

  case 3: /* root_statement: call_statement  */
#line 134 "hpccsqlgram.y"
                        { 
        (yyval.node) = createASTNode(TOKEN_CALL_STATEMENT, NULL); 
        addChild((yyval.node), (yyvsp[0].node)); 
    }
#line 1366 "hpccsqlgram.cpp"
    break;

  case 4: /* root_statement: create_load_table_statement  */
#line 138 "hpccsqlgram.y"
                                { 
        (yyval.node) = createASTNode(TOKEN_CREATE_LOAD_TABLE_STATEMENT, NULL); 
        addChild((yyval.node), (yyvsp[0].node)); 
    }
#line 1375 "hpccsqlgram.cpp"
    break;

  case 5: /* select_statement: select_portion from_portion where_clause groupby_clause having_clause orderby_clause limit_clause  */
#line 146 "hpccsqlgram.y"
    {
        (yyval.node) = createASTNode(TOKEN_SELECT_STATEMENT, NULL);
        if ((yyvsp[-6].node)) addChild((yyval.node), (yyvsp[-6].node));
        if ((yyvsp[-5].node)) addChild((yyval.node), (yyvsp[-5].node));
        if ((yyvsp[-4].node)) addChild((yyval.node), (yyvsp[-4].node));
        if ((yyvsp[-3].node)) addChild((yyval.node), (yyvsp[-3].node));
        if ((yyvsp[-2].node)) addChild((yyval.node), (yyvsp[-2].node));
        if ((yyvsp[-1].node)) addChild((yyval.node), (yyvsp[-1].node));
        if ((yyvsp[0].node)) addChild((yyval.node), (yyvsp[0].node));
    }
#line 1390 "hpccsqlgram.cpp"
    break;

  case 6: /* select_statement: select_portion  */
#line 157 "hpccsqlgram.y"
    {
        (yyval.node) = createASTNode(TOKEN_SELECT_STATEMENT, NULL);
        addChild((yyval.node), (yyvsp[0].node));
    }
#line 1399 "hpccsqlgram.cpp"
    break;

  case 7: /* select_portion: SELECT select_list  */
#line 164 "hpccsqlgram.y"
                            { (yyval.node) = createASTNode(SELECT, "SELECT"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1405 "hpccsqlgram.cpp"
    break;

  case 8: /* select_portion: SELECT DISTINCT select_list  */
#line 165 "hpccsqlgram.y"
                                { (yyval.node) = createASTNode(SELECT, "SELECT DISTINCT"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1411 "hpccsqlgram.cpp"
    break;

  case 9: /* from_portion: FROM table_references  */
#line 169 "hpccsqlgram.y"
                           { (yyval.node) = createASTNode(FROM, "FROM"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1417 "hpccsqlgram.cpp"
    break;

  case 10: /* from_portion: %empty  */
#line 170 "hpccsqlgram.y"
                            { (yyval.node) = NULL; }
#line 1423 "hpccsqlgram.cpp"
    break;

  case 11: /* where_clause: WHERE expression  */
#line 174 "hpccsqlgram.y"
                            { (yyval.node) = createASTNode(WHERE, "WHERE"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1429 "hpccsqlgram.cpp"
    break;

  case 12: /* where_clause: %empty  */
#line 175 "hpccsqlgram.y"
                            { (yyval.node) = NULL; }
#line 1435 "hpccsqlgram.cpp"
    break;

  case 13: /* groupby_clause: GROUP_SYM BY_SYM column_spec  */
#line 179 "hpccsqlgram.y"
                                    { (yyval.node) = createASTNode(GROUP_SYM, "GROUP BY"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1441 "hpccsqlgram.cpp"
    break;

  case 14: /* groupby_clause: %empty  */
#line 180 "hpccsqlgram.y"
                                    { (yyval.node) = NULL; }
#line 1447 "hpccsqlgram.cpp"
    break;

  case 15: /* having_clause: HAVING expression  */
#line 184 "hpccsqlgram.y"
                            { (yyval.node) = createASTNode(HAVING, "HAVING"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1453 "hpccsqlgram.cpp"
    break;

  case 16: /* having_clause: %empty  */
#line 185 "hpccsqlgram.y"
                            { (yyval.node) = NULL; }
#line 1459 "hpccsqlgram.cpp"
    break;

  case 17: /* orderby_clause: ORDER_SYM BY_SYM column_spec  */
#line 189 "hpccsqlgram.y"
                                    { (yyval.node) = createASTNode(ORDER_SYM, "ORDER BY"); addChild((yyval.node), (yyvsp[0].node)); }
#line 1465 "hpccsqlgram.cpp"
    break;

  case 18: /* orderby_clause: ORDER_SYM BY_SYM column_spec ASC  */
#line 190 "hpccsqlgram.y"
                                     { (yyval.node) = createASTNode(ORDER_SYM, "ORDER BY ASC"); addChild((yyval.node), (yyvsp[-1].node)); }
#line 1471 "hpccsqlgram.cpp"
    break;

  case 19: /* orderby_clause: ORDER_SYM BY_SYM column_spec DESC  */
#line 191 "hpccsqlgram.y"
                                      { (yyval.node) = createASTNode(ORDER_SYM, "ORDER BY DESC"); addChild((yyval.node), (yyvsp[-1].node)); }
#line 1477 "hpccsqlgram.cpp"
    break;

  case 20: /* orderby_clause: %empty  */
#line 192 "hpccsqlgram.y"
                                     { (yyval.node) = NULL; }
#line 1483 "hpccsqlgram.cpp"
    break;

  case 21: /* limit_clause: LIMIT INTEGER_NUM  */
#line 196 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(LIMIT, "LIMIT"); ASTNode* num = createASTNode(INTEGER_NUM, NULL); addChild((yyval.node), num); }
#line 1489 "hpccsqlgram.cpp"
    break;

  case 22: /* limit_clause: LIMIT INTEGER_NUM OFFSET_SYM INTEGER_NUM  */
#line 197 "hpccsqlgram.y"
                                             { (yyval.node) = createASTNode(LIMIT, "LIMIT OFFSET"); }
#line 1495 "hpccsqlgram.cpp"
    break;

  case 23: /* limit_clause: %empty  */
#line 198 "hpccsqlgram.y"
                                   { (yyval.node) = NULL; }
#line 1501 "hpccsqlgram.cpp"
    break;

  case 24: /* select_list: select_item  */
#line 202 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_LISTEXP, NULL); addChild((yyval.node), (yyvsp[0].node)); }
#line 1507 "hpccsqlgram.cpp"
    break;

  case 25: /* select_list: select_list COMMA select_item  */
#line 203 "hpccsqlgram.y"
                                   { addChild((yyvsp[-2].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-2].node); }
#line 1513 "hpccsqlgram.cpp"
    break;

  case 26: /* select_item: column_spec  */
#line 207 "hpccsqlgram.y"
                                   { (yyval.node) = (yyvsp[0].node); }
#line 1519 "hpccsqlgram.cpp"
    break;

  case 27: /* select_item: literal_value  */
#line 208 "hpccsqlgram.y"
                                   { (yyval.node) = (yyvsp[0].node); }
#line 1525 "hpccsqlgram.cpp"
    break;

  case 28: /* select_item: ASTERISK  */
#line 209 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_COLUMNWILDCARD, "*"); }
#line 1531 "hpccsqlgram.cpp"
    break;

  case 29: /* table_references: table_reference  */
#line 213 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_FROM_LIST, NULL); addChild((yyval.node), (yyvsp[0].node)); }
#line 1537 "hpccsqlgram.cpp"
    break;

  case 30: /* table_references: table_references COMMA table_reference  */
#line 214 "hpccsqlgram.y"
                                           { addChild((yyvsp[-2].node), (yyvsp[0].node)); (yyval.node) = (yyvsp[-2].node); }
#line 1543 "hpccsqlgram.cpp"
    break;

  case 31: /* table_reference: ID  */
#line 218 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_FROM_TABLE, (yyvsp[0].strval)); }
#line 1549 "hpccsqlgram.cpp"
    break;

  case 32: /* table_reference: QUOTED_ID  */
#line 219 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_FROM_TABLE, (yyvsp[0].strval)); }
#line 1555 "hpccsqlgram.cpp"
    break;

  case 33: /* column_spec: ID  */
#line 223 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_COLUMN, (yyvsp[0].strval)); }
#line 1561 "hpccsqlgram.cpp"
    break;

  case 34: /* column_spec: QUOTED_ID  */
#line 224 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_COLUMN, (yyvsp[0].strval)); }
#line 1567 "hpccsqlgram.cpp"
    break;

  case 35: /* column_spec: ID DOT ID  */
#line 225 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_COLUMN, NULL); 
                                     ASTNode* table = createASTNode(ID, (yyvsp[-2].strval));
                                     ASTNode* col = createASTNode(ID, (yyvsp[0].strval));
                                     addChild((yyval.node), col);
                                     addChild((yyval.node), table); }
#line 1577 "hpccsqlgram.cpp"
    break;

  case 36: /* expression: column_spec  */
#line 233 "hpccsqlgram.y"
                                   { (yyval.node) = (yyvsp[0].node); }
#line 1583 "hpccsqlgram.cpp"
    break;

  case 37: /* expression: literal_value  */
#line 234 "hpccsqlgram.y"
                                   { (yyval.node) = (yyvsp[0].node); }
#line 1589 "hpccsqlgram.cpp"
    break;

  case 38: /* expression: expression EQ_SYM expression  */
#line 235 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(EQ_SYM, "="); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1595 "hpccsqlgram.cpp"
    break;

  case 39: /* expression: expression NE expression  */
#line 236 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(NE, "!="); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1601 "hpccsqlgram.cpp"
    break;

  case 40: /* expression: expression LTH expression  */
#line 237 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(LTH, "<"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1607 "hpccsqlgram.cpp"
    break;

  case 41: /* expression: expression GTH expression  */
#line 238 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(GTH, ">"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1613 "hpccsqlgram.cpp"
    break;

  case 42: /* expression: expression LE expression  */
#line 239 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(LE, "<="); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1619 "hpccsqlgram.cpp"
    break;

  case 43: /* expression: expression GE expression  */
#line 240 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(GE, ">="); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1625 "hpccsqlgram.cpp"
    break;

  case 44: /* expression: expression PLUS expression  */
#line 241 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(PLUS, "+"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1631 "hpccsqlgram.cpp"
    break;

  case 45: /* expression: expression MINUS expression  */
#line 242 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(MINUS, "-"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1637 "hpccsqlgram.cpp"
    break;

  case 46: /* expression: expression ASTERISK expression  */
#line 243 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(ASTERISK, "*"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1643 "hpccsqlgram.cpp"
    break;

  case 47: /* expression: expression DIVIDE expression  */
#line 244 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(DIVIDE, "/"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1649 "hpccsqlgram.cpp"
    break;

  case 48: /* expression: expression OR_SYM expression  */
#line 245 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(OR_SYM, "OR"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1655 "hpccsqlgram.cpp"
    break;

  case 49: /* expression: expression AND_SYM expression  */
#line 246 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(AND_SYM, "AND"); addChild((yyval.node), (yyvsp[-2].node)); addChild((yyval.node), (yyvsp[0].node)); }
#line 1661 "hpccsqlgram.cpp"
    break;

  case 50: /* expression: LPAREN expression RPAREN  */
#line 247 "hpccsqlgram.y"
                                   { (yyval.node) = (yyvsp[-1].node); }
#line 1667 "hpccsqlgram.cpp"
    break;

  case 51: /* literal_value: STRING_LITERAL  */
#line 251 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(STRING_LITERAL, (yyvsp[0].strval)); }
#line 1673 "hpccsqlgram.cpp"
    break;

  case 52: /* literal_value: INTEGER_NUM  */
#line 252 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(INTEGER_NUM, NULL); }
#line 1679 "hpccsqlgram.cpp"
    break;

  case 53: /* literal_value: DECIMAL_NUM  */
#line 253 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(DECIMAL_NUM, NULL); }
#line 1685 "hpccsqlgram.cpp"
    break;

  case 54: /* literal_value: TRUE_SYM  */
#line 254 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TRUE_SYM, "TRUE"); }
#line 1691 "hpccsqlgram.cpp"
    break;

  case 55: /* literal_value: FALSE_SYM  */
#line 255 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(FALSE_SYM, "FALSE"); }
#line 1697 "hpccsqlgram.cpp"
    break;

  case 56: /* literal_value: NULL_SYM  */
#line 256 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(NULL_SYM, "NULL"); }
#line 1703 "hpccsqlgram.cpp"
    break;

  case 57: /* call_statement: CALL_SYM ID LPAREN RPAREN  */
#line 260 "hpccsqlgram.y"
                                   { (yyval.node) = createASTNode(TOKEN_CALL_STATEMENT, NULL);
                                     ASTNode* proc = createASTNode(TOKEN_PROC_NAME, (yyvsp[-2].strval));
                                     addChild((yyval.node), proc); }
#line 1711 "hpccsqlgram.cpp"
    break;

  case 58: /* create_load_table_statement: CREATE_SYM TABLE_SYM ID  */
#line 266 "hpccsqlgram.y"
                                    { (yyval.node) = createASTNode(TOKEN_CREATE_LOAD_TABLE_STATEMENT, NULL);
                                     ASTNode* table = createASTNode(TOKEN_CREATE_TABLE, (yyvsp[0].strval));
                                     addChild((yyval.node), table); }
#line 1719 "hpccsqlgram.cpp"
    break;


#line 1723 "hpccsqlgram.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, context, YY_("syntax error"));
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
                      yytoken, &yylval, scanner, context);
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner, context);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, context, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner, context);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner, context);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 271 "hpccsqlgram.y"


void yyerror(yyscan_t scanner, HPCCSQLTreeWalker* context, const char* msg) {
    fprintf(stderr, "SQL Parse Error: %s\n", msg);
}

ASTNode* createASTNode(int type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->nodeType = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->childCount = 0;
    node->childCapacity = 0;
    return node;
}

ASTNode* addChild(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return parent;
    
    if (parent->childCount >= parent->childCapacity) {
        parent->childCapacity = parent->childCapacity == 0 ? 4 : parent->childCapacity * 2;
        parent->children = (ASTNode**)realloc(parent->children, 
                                              parent->childCapacity * sizeof(ASTNode*));
    }
    
    parent->children[parent->childCount] = child;
    parent->childCount++;
    return parent;
}

void freeASTNode(ASTNode* node) {
    if (!node) return;
    
    for (int i = 0; i < node->childCount; i++) {
        freeASTNode(node->children[i]);
    }
    
    free(node->children);
    free(node->value);
    free(node);
}

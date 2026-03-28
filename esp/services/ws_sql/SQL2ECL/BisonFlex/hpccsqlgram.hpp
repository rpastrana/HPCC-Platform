/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HPCCSQLGRAM_HPP_INCLUDED
# define YY_YY_HPCCSQLGRAM_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_ROOT = 258,              /* TOKEN_ROOT  */
    TOKEN_SELECT_STATEMENT = 259,  /* TOKEN_SELECT_STATEMENT  */
    TOKEN_CALL_STATEMENT = 260,    /* TOKEN_CALL_STATEMENT  */
    TOKEN_CREATE_LOAD_TABLE_STATEMENT = 261, /* TOKEN_CREATE_LOAD_TABLE_STATEMENT  */
    TOKEN_CREATE_INDEX_STATEMENT = 262, /* TOKEN_CREATE_INDEX_STATEMENT  */
    TOKEN_CREATE_TABLE = 263,      /* TOKEN_CREATE_TABLE  */
    TOKEN_DONOT_OVERWRITE = 264,   /* TOKEN_DONOT_OVERWRITE  */
    TOKEN_OVERWRITE = 265,         /* TOKEN_OVERWRITE  */
    TOKEN_LOAD_TABLE = 266,        /* TOKEN_LOAD_TABLE  */
    TOKEN_FROM_LIST = 267,         /* TOKEN_FROM_LIST  */
    TOKEN_FROM_TABLE = 268,        /* TOKEN_FROM_TABLE  */
    TOKEN_PROC_NAME = 269,         /* TOKEN_PROC_NAME  */
    TOKEN_PROC_PARAMS = 270,       /* TOKEN_PROC_PARAMS  */
    TOKEN_ALIAS = 271,             /* TOKEN_ALIAS  */
    TOKEN_INNER_JOIN = 272,        /* TOKEN_INNER_JOIN  */
    TOKEN_OUTTER_JOIN = 273,       /* TOKEN_OUTTER_JOIN  */
    TOKEN_INDEX_HINT = 274,        /* TOKEN_INDEX_HINT  */
    TOKEN_AVOID_INDEX = 275,       /* TOKEN_AVOID_INDEX  */
    TOKEN_COLUMN = 276,            /* TOKEN_COLUMN  */
    TOKEN_LISTEXP = 277,           /* TOKEN_LISTEXP  */
    TOKEN_FUNCEXP = 278,           /* TOKEN_FUNCEXP  */
    TOKEN_PARAMPLACEHOLDER = 279,  /* TOKEN_PARAMPLACEHOLDER  */
    TOKEN_COLUMNWILDCARD = 280,    /* TOKEN_COLUMNWILDCARD  */
    TOKEN_TABLE_SCHEMA = 281,      /* TOKEN_TABLE_SCHEMA  */
    TOKEN_COLUMN_DEF_LIST = 282,   /* TOKEN_COLUMN_DEF_LIST  */
    TOKEN_COLUMN_DEF = 283,        /* TOKEN_COLUMN_DEF  */
    TOKEN_LANDING_ZONE = 284,      /* TOKEN_LANDING_ZONE  */
    TOKEN_VARIABLE_FILE = 285,     /* TOKEN_VARIABLE_FILE  */
    TOKEN_VAR_SEPERATOR = 286,     /* TOKEN_VAR_SEPERATOR  */
    TOKEN_VAR_TERMINATOR = 287,    /* TOKEN_VAR_TERMINATOR  */
    TOKEN_VAR_ENCLOSED = 288,      /* TOKEN_VAR_ENCLOSED  */
    TOKEN_VAR_ESCAPED = 289,       /* TOKEN_VAR_ESCAPED  */
    ADD_SYM = 290,                 /* ADD_SYM  */
    ALL = 291,                     /* ALL  */
    ANY = 292,                     /* ANY  */
    AS_SYM = 293,                  /* AS_SYM  */
    ASC = 294,                     /* ASC  */
    ASCII_SYM = 295,               /* ASCII_SYM  */
    AT_SYM = 296,                  /* AT_SYM  */
    AVG = 297,                     /* AVG  */
    BETWEEN = 298,                 /* BETWEEN  */
    BINARY_SYM = 299,              /* BINARY_SYM  */
    BIT_SYM = 300,                 /* BIT_SYM  */
    BOOL_SYM = 301,                /* BOOL_SYM  */
    BOOLEAN_SYM = 302,             /* BOOLEAN_SYM  */
    BY_SYM = 303,                  /* BY_SYM  */
    CALL_SYM = 304,                /* CALL_SYM  */
    CREATE_SYM = 305,              /* CREATE_SYM  */
    COLUMN_SYM = 306,              /* COLUMN_SYM  */
    COMMENT_SYM = 307,             /* COMMENT_SYM  */
    CONTAINS_SYM = 308,            /* CONTAINS_SYM  */
    COUNT = 309,                   /* COUNT  */
    DATA_SYM = 310,                /* DATA_SYM  */
    DESC = 311,                    /* DESC  */
    DISTINCT = 312,                /* DISTINCT  */
    EXISTS_SYM = 313,              /* EXISTS_SYM  */
    FALSE_SYM = 314,               /* FALSE_SYM  */
    FOR_SYM = 315,                 /* FOR_SYM  */
    FROM = 316,                    /* FROM  */
    GROUP_SYM = 317,               /* GROUP_SYM  */
    HAVING = 318,                  /* HAVING  */
    IF_SYM = 319,                  /* IF_SYM  */
    IN_SYM = 320,                  /* IN_SYM  */
    INDEX_SYM = 321,               /* INDEX_SYM  */
    INFILE_SYM = 322,              /* INFILE_SYM  */
    INTO_SYM = 323,                /* INTO_SYM  */
    IS_SYM = 324,                  /* IS_SYM  */
    JOIN_SYM = 325,                /* JOIN_SYM  */
    LEFT = 326,                    /* LEFT  */
    LIKE = 327,                    /* LIKE  */
    LIMIT = 328,                   /* LIMIT  */
    LOAD_SYM = 329,                /* LOAD_SYM  */
    LOWER = 330,                   /* LOWER  */
    MAX_SYM = 331,                 /* MAX_SYM  */
    MIN_SYM = 332,                 /* MIN_SYM  */
    MOD = 333,                     /* MOD  */
    NOT_SYM = 334,                 /* NOT_SYM  */
    NULL_SYM = 335,                /* NULL_SYM  */
    OFFSET_SYM = 336,              /* OFFSET_SYM  */
    ON = 337,                      /* ON  */
    OR_SYM = 338,                  /* OR_SYM  */
    ORDER_SYM = 339,               /* ORDER_SYM  */
    OUT_SYM = 340,                 /* OUT_SYM  */
    OUTER = 341,                   /* OUTER  */
    POWER = 342,                   /* POWER  */
    REPLACE_SYM = 343,             /* REPLACE_SYM  */
    RIGHT = 344,                   /* RIGHT  */
    SELECT = 345,                  /* SELECT  */
    SUM = 346,                     /* SUM  */
    TABLE_SYM = 347,               /* TABLE_SYM  */
    TRUE_SYM = 348,                /* TRUE_SYM  */
    UNION = 349,                   /* UNION  */
    UPPER = 350,                   /* UPPER  */
    WHERE = 351,                   /* WHERE  */
    AND_SYM = 352,                 /* AND_SYM  */
    EQ_SYM = 353,                  /* EQ_SYM  */
    NE = 354,                      /* NE  */
    LTH = 355,                     /* LTH  */
    GTH = 356,                     /* GTH  */
    LE = 357,                      /* LE  */
    GE = 358,                      /* GE  */
    PLUS = 359,                    /* PLUS  */
    MINUS = 360,                   /* MINUS  */
    ASTERISK = 361,                /* ASTERISK  */
    DIVIDE = 362,                  /* DIVIDE  */
    MOD_SYM = 363,                 /* MOD_SYM  */
    POWER_OP = 364,                /* POWER_OP  */
    LPAREN = 365,                  /* LPAREN  */
    RPAREN = 366,                  /* RPAREN  */
    LBRACK = 367,                  /* LBRACK  */
    RBRACK = 368,                  /* RBRACK  */
    LCURLY = 369,                  /* LCURLY  */
    RCURLY = 370,                  /* RCURLY  */
    COMMA = 371,                   /* COMMA  */
    SEMI = 372,                    /* SEMI  */
    COLON = 373,                   /* COLON  */
    DOT = 374,                     /* DOT  */
    QUESTION = 375,                /* QUESTION  */
    DOLLAR = 376,                  /* DOLLAR  */
    DQUOTE = 377,                  /* DQUOTE  */
    VERTBAR = 378,                 /* VERTBAR  */
    BITAND = 379,                  /* BITAND  */
    NEGATION = 380,                /* NEGATION  */
    STRING_LITERAL = 381,          /* STRING_LITERAL  */
    ID = 382,                      /* ID  */
    QUOTED_ID = 383,               /* QUOTED_ID  */
    HEX_DIGIT = 384,               /* HEX_DIGIT  */
    INTEGER_NUM = 385,             /* INTEGER_NUM  */
    DECIMAL_NUM = 386              /* DECIMAL_NUM  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 23 "hpccsqlgram.y"

    char* strval;
    int intval;
    float floatval;
    struct ASTNode* node;

#line 202 "hpccsqlgram.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (yyscan_t scanner, HPCCSQLTreeWalker* context);


#endif /* !YY_YY_HPCCSQLGRAM_HPP_INCLUDED  */

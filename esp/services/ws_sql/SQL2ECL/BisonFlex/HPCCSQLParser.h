/*##############################################################################

HPCC SYSTEMS software Copyright (C) 2025 HPCC Systems.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
############################################################################## */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token definitions
typedef enum {
    TOKEN_ROOT = 1000,
    TOKEN_SELECT_STATEMENT,
    TOKEN_CALL_STATEMENT,
    TOKEN_CREATE_LOAD_TABLE_STATEMENT,
    TOKEN_CREATE_INDEX_STATEMENT,
    TOKEN_CREATE_TABLE,
    TOKEN_DONOT_OVERWRITE,
    TOKEN_OVERWRITE,
    TOKEN_LOAD_TABLE,
    TOKEN_FROM_LIST,
    TOKEN_FROM_TABLE,
    TOKEN_PROC_NAME,
    TOKEN_PROC_PARAMS,
    TOKEN_ALIAS,
    TOKEN_INNER_JOIN,
    TOKEN_OUTTER_JOIN,
    TOKEN_INDEX_HINT,
    TOKEN_AVOID_INDEX,
    TOKEN_COLUMN,
    TOKEN_LISTEXP,
    TOKEN_FUNCEXP,
    TOKEN_PARAMPLACEHOLDER,
    TOKEN_COLUMNWILDCARD,
    TOKEN_TABLE_SCHEMA,
    TOKEN_COLUMN_DEF_LIST,
    TOKEN_COLUMN_DEF,
    TOKEN_LANDING_ZONE,
    TOKEN_VARIABLE_FILE,
    TOKEN_VAR_SEPERATOR,
    TOKEN_VAR_TERMINATOR,
    TOKEN_VAR_ENCLOSED,
    TOKEN_VAR_ESCAPED
} HPCCSQLTokenType;

// Keywords
enum {
    ADD_SYM = 2000, ALL, ANY, AS_SYM, ASC, ASCII_SYM, AT_SYM, AVG,
    BETWEEN, BINARY_SYM, BIT_SYM, BOOL_SYM, BOOLEAN_SYM, BY_SYM,
    CALL_SYM, CREATE_SYM, COLUMN_SYM, COMMENT_SYM, CONTAINS_SYM, COUNT,
    DATA_SYM, DESC, DISTINCT, EXISTS_SYM, FALSE_SYM, FOR_SYM, FROM,
    GROUP_SYM, HAVING, IF_SYM, IN_SYM, INDEX_SYM, INFILE_SYM, INTO_SYM, IS_SYM,
    JOIN_SYM, LEFT, LIKE, LIMIT, LOAD_SYM, LOWER, MAX_SYM, MIN_SYM, MOD,
    NOT_SYM, NULL_SYM, OFFSET_SYM, ON, OR_SYM, ORDER_SYM, OUT_SYM, OUTER,
    POWER, REPLACE_SYM, RIGHT, SELECT, SUM, TABLE_SYM, TRUE_SYM, UNION, UPPER, WHERE,
    AND_SYM
};

// Operators
enum {
    EQ_SYM = 3000, NE, LTH, GTH, LE, GE, PLUS, MINUS, ASTERISK, DIVIDE, MOD_SYM, POWER_OP,
    LPAREN, RPAREN, LBRACK, RBRACK, LCURLY, RCURLY, COMMA, SEMI, COLON, DOT,
    QUESTION, DOLLAR, DQUOTE, VERTBAR, BITAND, NEGATION
};

// Literals
enum {
    STRING_LITERAL = 4000, ID, QUOTED_ID, HEX_DIGIT, INTEGER_NUM, DECIMAL_NUM, ABSOLUTE_FILE_ID
};

// AST Node structure
typedef struct ASTNode {
    int nodeType;
    char* value;
    struct ASTNode** children;
    int childCount;
    int childCapacity;
} ASTNode;

// Parser union type
typedef union HPCCSQLSTYPE {
    char* strval;
    int intval;
    float floatval;
    struct ASTNode* node;
} HPCCSQLSTYPE;

// Forward declarations
struct HPCCSQLTreeWalker;
typedef void* yyscan_t;

// Function declarations
ASTNode* createASTNode(int type, const char* value);
ASTNode* addChild(ASTNode* parent, ASTNode* child);
void freeASTNode(ASTNode* node);

// Lexer/Parser functions
int hpccsqllex(HPCCSQLSTYPE* yylval, yyscan_t yyscanner);
int hpccsqllex_init(yyscan_t* scanner);
int hpccsqllex_destroy(yyscan_t scanner);
void hpccsql_scan_string(const char* str, yyscan_t scanner);

int hpccsqlparse(yyscan_t scanner, struct HPCCSQLTreeWalker* context);

// Error handling
void yyerror(yyscan_t scanner, struct HPCCSQLTreeWalker* context, const char* msg);

// Compatibility types for ANTLR3 replacement
typedef ASTNode* pHPCCSQLBASE_TREE;

// Compatibility macros
#define HPCCSQL_TOKEN_EOF -1

// Additional function declarations for TreeWalker
class HPCCSQLTreeWalker;
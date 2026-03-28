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

%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {HPCCSQLTreeWalker* context}

%union {
    char* strval;
    int intval;
    float floatval;
    struct ASTNode* node;
}

%{
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

%}

%token TOKEN_ROOT
%token TOKEN_SELECT_STATEMENT
%token TOKEN_CALL_STATEMENT
%token TOKEN_CREATE_LOAD_TABLE_STATEMENT
%token TOKEN_CREATE_INDEX_STATEMENT
%token TOKEN_CREATE_TABLE
%token TOKEN_DONOT_OVERWRITE
%token TOKEN_OVERWRITE
%token TOKEN_LOAD_TABLE
%token TOKEN_FROM_LIST
%token TOKEN_FROM_TABLE
%token TOKEN_PROC_NAME
%token TOKEN_PROC_PARAMS
%token TOKEN_ALIAS
%token TOKEN_INNER_JOIN
%token TOKEN_OUTTER_JOIN
%token TOKEN_INDEX_HINT
%token TOKEN_AVOID_INDEX
%token TOKEN_COLUMN
%token TOKEN_LISTEXP
%token TOKEN_FUNCEXP
%token TOKEN_PARAMPLACEHOLDER
%token TOKEN_COLUMNWILDCARD
%token TOKEN_TABLE_SCHEMA
%token TOKEN_COLUMN_DEF_LIST
%token TOKEN_COLUMN_DEF
%token TOKEN_LANDING_ZONE
%token TOKEN_VARIABLE_FILE
%token TOKEN_VAR_SEPERATOR
%token TOKEN_VAR_TERMINATOR
%token TOKEN_VAR_ENCLOSED
%token TOKEN_VAR_ESCAPED

/* Keyword tokens */
%token ADD_SYM ALL ANY AS_SYM ASC ASCII_SYM AT_SYM AVG
%token BETWEEN BINARY_SYM BIT_SYM BOOL_SYM BOOLEAN_SYM BY_SYM
%token CALL_SYM CREATE_SYM COLUMN_SYM COMMENT_SYM CONTAINS_SYM COUNT
%token DATA_SYM DESC DISTINCT EXISTS_SYM FALSE_SYM FOR_SYM FROM
%token GROUP_SYM HAVING IF_SYM IN_SYM INDEX_SYM INFILE_SYM INTO_SYM IS_SYM
%token JOIN_SYM LEFT LIKE LIMIT LOAD_SYM LOWER MAX_SYM MIN_SYM MOD
%token NOT_SYM NULL_SYM OFFSET_SYM ON OR_SYM ORDER_SYM OUT_SYM OUTER
%token POWER REPLACE_SYM RIGHT SELECT SUM TABLE_SYM TRUE_SYM UNION UPPER WHERE
%token AND_SYM

/* Operators */
%token EQ_SYM NE LTH GTH LE GE PLUS MINUS ASTERISK DIVIDE MOD_SYM POWER_OP
%token LPAREN RPAREN LBRACK RBRACK LCURLY RCURLY COMMA SEMI COLON DOT
%token QUESTION DOLLAR DQUOTE VERTBAR BITAND NEGATION

/* Literals and identifiers */
%token <strval> STRING_LITERAL ID QUOTED_ID HEX_DIGIT
%token <intval> INTEGER_NUM 
%token <floatval> DECIMAL_NUM

/* Non-terminals with types */
%type <node> root_statement select_statement call_statement create_load_table_statement
%type <node> expression select_list select_item table_references table_reference
%type <node> column_spec literal_value where_clause orderby_clause limit_clause
%type <node> from_portion select_portion groupby_clause having_clause

%start root_statement

%%

root_statement:
    select_statement    { 
        $$ = createASTNode(TOKEN_SELECT_STATEMENT, NULL); 
        addChild($$, $1); 
    }
  | call_statement      { 
        $$ = createASTNode(TOKEN_CALL_STATEMENT, NULL); 
        addChild($$, $1); 
    }
  | create_load_table_statement { 
        $$ = createASTNode(TOKEN_CREATE_LOAD_TABLE_STATEMENT, NULL); 
        addChild($$, $1); 
    }
  ;

select_statement:
    select_portion from_portion where_clause groupby_clause having_clause orderby_clause limit_clause
    {
        $$ = createASTNode(TOKEN_SELECT_STATEMENT, NULL);
        if ($1) addChild($$, $1);
        if ($2) addChild($$, $2);
        if ($3) addChild($$, $3);
        if ($4) addChild($$, $4);
        if ($5) addChild($$, $5);
        if ($6) addChild($$, $6);
        if ($7) addChild($$, $7);
    }
  | select_portion
    {
        $$ = createASTNode(TOKEN_SELECT_STATEMENT, NULL);
        addChild($$, $1);
    }
  ;

select_portion:
    SELECT select_list      { $$ = createASTNode(SELECT, "SELECT"); addChild($$, $2); }
  | SELECT DISTINCT select_list { $$ = createASTNode(SELECT, "SELECT DISTINCT"); addChild($$, $3); }
  ;

from_portion:
    FROM table_references  { $$ = createASTNode(FROM, "FROM"); addChild($$, $2); }
  | /* empty */             { $$ = NULL; }
  ;

where_clause:
    WHERE expression        { $$ = createASTNode(WHERE, "WHERE"); addChild($$, $2); }
  | /* empty */             { $$ = NULL; }
  ;

groupby_clause:
    GROUP_SYM BY_SYM column_spec    { $$ = createASTNode(GROUP_SYM, "GROUP BY"); addChild($$, $3); }
  | /* empty */                     { $$ = NULL; }
  ;

having_clause:
    HAVING expression       { $$ = createASTNode(HAVING, "HAVING"); addChild($$, $2); }
  | /* empty */             { $$ = NULL; }
  ;

orderby_clause:
    ORDER_SYM BY_SYM column_spec    { $$ = createASTNode(ORDER_SYM, "ORDER BY"); addChild($$, $3); }
  | ORDER_SYM BY_SYM column_spec ASC { $$ = createASTNode(ORDER_SYM, "ORDER BY ASC"); addChild($$, $3); }
  | ORDER_SYM BY_SYM column_spec DESC { $$ = createASTNode(ORDER_SYM, "ORDER BY DESC"); addChild($$, $3); }
  | /* empty */                      { $$ = NULL; }
  ;

limit_clause:
    LIMIT INTEGER_NUM              { $$ = createASTNode(LIMIT, "LIMIT"); ASTNode* num = createASTNode(INTEGER_NUM, NULL); addChild($$, num); }
  | LIMIT INTEGER_NUM OFFSET_SYM INTEGER_NUM { $$ = createASTNode(LIMIT, "LIMIT OFFSET"); }
  | /* empty */                    { $$ = NULL; }
  ;

select_list:
    select_item                    { $$ = createASTNode(TOKEN_LISTEXP, NULL); addChild($$, $1); }
  | select_list COMMA select_item  { addChild($1, $3); $$ = $1; }
  ;

select_item:
    column_spec                    { $$ = $1; }
  | literal_value                  { $$ = $1; }
  | ASTERISK                       { $$ = createASTNode(TOKEN_COLUMNWILDCARD, "*"); }
  ;

table_references:
    table_reference                { $$ = createASTNode(TOKEN_FROM_LIST, NULL); addChild($$, $1); }
  | table_references COMMA table_reference { addChild($1, $3); $$ = $1; }
  ;

table_reference:
    ID                             { $$ = createASTNode(TOKEN_FROM_TABLE, $1); }
  | QUOTED_ID                      { $$ = createASTNode(TOKEN_FROM_TABLE, $1); }
  ;

column_spec:
    ID                             { $$ = createASTNode(TOKEN_COLUMN, $1); }
  | QUOTED_ID                      { $$ = createASTNode(TOKEN_COLUMN, $1); }
  | ID DOT ID                      { $$ = createASTNode(TOKEN_COLUMN, NULL); 
                                     ASTNode* table = createASTNode(ID, $1);
                                     ASTNode* col = createASTNode(ID, $3);
                                     addChild($$, col);
                                     addChild($$, table); }
  ;

expression:
    column_spec                    { $$ = $1; }
  | literal_value                  { $$ = $1; }
  | expression EQ_SYM expression   { $$ = createASTNode(EQ_SYM, "="); addChild($$, $1); addChild($$, $3); }
  | expression NE expression       { $$ = createASTNode(NE, "!="); addChild($$, $1); addChild($$, $3); }
  | expression LTH expression      { $$ = createASTNode(LTH, "<"); addChild($$, $1); addChild($$, $3); }
  | expression GTH expression      { $$ = createASTNode(GTH, ">"); addChild($$, $1); addChild($$, $3); }
  | expression LE expression       { $$ = createASTNode(LE, "<="); addChild($$, $1); addChild($$, $3); }
  | expression GE expression       { $$ = createASTNode(GE, ">="); addChild($$, $1); addChild($$, $3); }
  | expression PLUS expression     { $$ = createASTNode(PLUS, "+"); addChild($$, $1); addChild($$, $3); }
  | expression MINUS expression    { $$ = createASTNode(MINUS, "-"); addChild($$, $1); addChild($$, $3); }
  | expression ASTERISK expression { $$ = createASTNode(ASTERISK, "*"); addChild($$, $1); addChild($$, $3); }
  | expression DIVIDE expression   { $$ = createASTNode(DIVIDE, "/"); addChild($$, $1); addChild($$, $3); }
  | expression OR_SYM expression   { $$ = createASTNode(OR_SYM, "OR"); addChild($$, $1); addChild($$, $3); }
  | expression AND_SYM expression  { $$ = createASTNode(AND_SYM, "AND"); addChild($$, $1); addChild($$, $3); }
  | LPAREN expression RPAREN       { $$ = $2; }
  ;

literal_value:
    STRING_LITERAL                 { $$ = createASTNode(STRING_LITERAL, $1); }
  | INTEGER_NUM                    { $$ = createASTNode(INTEGER_NUM, NULL); }
  | DECIMAL_NUM                    { $$ = createASTNode(DECIMAL_NUM, NULL); }
  | TRUE_SYM                       { $$ = createASTNode(TRUE_SYM, "TRUE"); }
  | FALSE_SYM                      { $$ = createASTNode(FALSE_SYM, "FALSE"); }
  | NULL_SYM                       { $$ = createASTNode(NULL_SYM, "NULL"); }
  ;

call_statement:
    CALL_SYM ID LPAREN RPAREN      { $$ = createASTNode(TOKEN_CALL_STATEMENT, NULL);
                                     ASTNode* proc = createASTNode(TOKEN_PROC_NAME, $2);
                                     addChild($$, proc); }
  ;

create_load_table_statement:
    CREATE_SYM TABLE_SYM ID         { $$ = createASTNode(TOKEN_CREATE_LOAD_TABLE_STATEMENT, NULL);
                                     ASTNode* table = createASTNode(TOKEN_CREATE_TABLE, $3);
                                     addChild($$, table); }
  ;

%%

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
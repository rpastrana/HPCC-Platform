/*##############################################################################

HPCC SYSTEMS software Copyright (C) 2014 HPCC Systems.

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

%option 8bit never-interactive
%option prefix="hpccsql"
%option nounistd
%option reentrant
%option noyywrap
%option case-insensitive

%{
// Include the generated parser header for token definitions
// This will be available after Bison generates it
#include "hpccsqlgram.hpp"

#define YY_DECL int hpccsqllex(YYSTYPE * yylval, yyscan_t yyscanner)

%}

/* Character fragments - case insensitive */
A   [aA]
B   [bB]
C   [cC]
D   [dD]
E   [eE]
F   [fF]
G   [gG]
H   [hH]
I   [iI]
J   [jJ]
K   [kK]
L   [lL]
M   [mM]
N   [nN]
O   [oO]
P   [pP]
Q   [qQ]
R   [rR]
S   [sS]
T   [tT]
U   [uU]
V   [vV]
W   [wW]
X   [xX]
Y   [yY]
Z   [zZ]

LETTER          [a-zA-Z_]
DIGIT           [0-9]
HEX_DIGIT       [0-9a-fA-F]

%%

    /* Comments */
"//".*                              { /* single line comment */ }
"/*"([^*]|"*"+[^*/])*"*"+"/"        { /* multi-line comment */ }

    /* Keywords (case insensitive) */
{A}{D}{D}                           { return ADD_SYM; }
{A}{L}{L}                           { return ALL; }
{A}{N}{D}                           { return AND_SYM; }
{A}{N}{Y}                           { return ANY; }
{A}{S}                              { return AS_SYM; }
{A}{S}{C}                           { return ASC; }
{A}{S}{C}{I}{I}                     { return ASCII_SYM; }
{A}{T}                              { return AT_SYM; }
{A}{V}{G}                           { return AVG; }
{B}{E}{T}{W}{E}{E}{N}               { return BETWEEN; }
{B}{I}{N}{A}{R}{Y}                  { return BINARY_SYM; }
{B}{I}{T}                           { return BIT_SYM; }
{B}{O}{O}{L}                        { return BOOL_SYM; }
{B}{O}{O}{L}{E}{A}{N}               { return BOOLEAN_SYM; }
{B}{Y}                              { return BY_SYM; }
{C}{A}{L}{L}                        { return CALL_SYM; }
{C}{R}{E}{A}{T}{E}                  { return CREATE_SYM; }
{C}{O}{L}{U}{M}{N}                  { return COLUMN_SYM; }
{C}{O}{M}{M}{E}{N}{T}               { return COMMENT_SYM; }
{C}{O}{N}{T}{A}{I}{N}{S}            { return CONTAINS_SYM; }
{C}{O}{U}{N}{T}                     { return COUNT; }
{D}{A}{T}{A}                        { return DATA_SYM; }
{D}{E}{S}{C}                        { return DESC; }
{D}{I}{S}{T}{I}{N}{C}{T}            { return DISTINCT; }
{E}{X}{I}{S}{T}{S}                  { return EXISTS_SYM; }
{F}{A}{L}{S}{E}                     { return FALSE_SYM; }
{F}{O}{R}                           { return FOR_SYM; }
{F}{R}{O}{M}                        { return FROM; }
{G}{R}{O}{U}{P}                     { return GROUP_SYM; }
{H}{A}{V}{I}{N}{G}                  { return HAVING; }
{I}{F}                              { return IF_SYM; }
{I}{N}                              { return IN_SYM; }
{I}{N}{D}{E}{X}                     { return INDEX_SYM; }
{I}{N}{F}{I}{L}{E}                  { return INFILE_SYM; }
{I}{N}{T}{O}                        { return INTO_SYM; }
{I}{S}                              { return IS_SYM; }
{J}{O}{I}{N}                        { return JOIN_SYM; }
{L}{E}{F}{T}                        { return LEFT; }
{L}{I}{K}{E}                        { return LIKE; }
{L}{I}{M}{I}{T}                     { return LIMIT; }
{L}{O}{A}{D}                        { return LOAD_SYM; }
{L}{O}{W}{E}{R}|{L}{C}{A}{S}{E}     { return LOWER; }
{M}{A}{X}                           { return MAX_SYM; }
{M}{I}{N}                           { return MIN_SYM; }
{M}{O}{D}                           { return MOD; }
{N}{O}{T}|{N}{O}{T}                 { return NOT_SYM; }
{N}{U}{L}{L}                        { return NULL_SYM; }
{O}{F}{F}{S}{E}{T}                  { return OFFSET_SYM; }
{O}{N}                              { return ON; }
{O}{R}                              { return OR_SYM; }
{O}{R}{D}{E}{R}                     { return ORDER_SYM; }
{O}{U}{T}                           { return OUT_SYM; }
{O}{U}{T}{E}{R}                     { return OUTER; }
{P}{O}{W}{E}{R}                     { return POWER; }
{R}{E}{P}{L}{A}{C}{E}               { return REPLACE_SYM; }
{R}{I}{G}{H}{T}                     { return RIGHT; }
{S}{E}{L}{E}{C}{T}                  { return SELECT; }
{S}{U}{M}                           { return SUM; }
{T}{A}{B}{L}{E}                     { return TABLE_SYM; }
{T}{R}{U}{E}                        { return TRUE_SYM; }
{U}{N}{I}{O}{N}                     { return UNION; }
{U}{P}{P}{E}{R}                     { return UPPER; }
{W}{H}{E}{R}{E}                     { return WHERE; }

    /* Operators and punctuation */
"="                                 { return EQ_SYM; }
"!="                                { return NE; }
"<>"                                { return NE; }
"<"                                 { return LTH; }
">"                                 { return GTH; }
"<="                                { return LE; }
">="                                { return GE; }
"+"                                 { return PLUS; }
"-"                                 { return MINUS; }
"*"                                 { return ASTERISK; }
"/"                                 { return DIVIDE; }
"%"                                 { return MOD_SYM; }
"^"                                 { return POWER_OP; }
"("                                 { return LPAREN; }
")"                                 { return RPAREN; }
"["                                 { return LBRACK; }
"]"                                 { return RBRACK; }
"{"                                 { return LCURLY; }
"}"                                 { return RCURLY; }
","                                 { return COMMA; }
";"                                 { return SEMI; }
":"                                 { return COLON; }
"."                                 { return DOT; }
"?"                                 { return QUESTION; }
"$"                                 { return DOLLAR; }
"\""                                { return DQUOTE; }
"|"                                 { return VERTBAR; }
"&"                                 { return BITAND; }
"~"                                 { return NEGATION; }

    /* String literals */
\'([^'\\]|\\.|'')*\'                { yylval->strval = strdup(yytext); return STRING_LITERAL; }
\"([^"\\]|\\.|"")*\"                { yylval->strval = strdup(yytext); return STRING_LITERAL; }

    /* Numeric literals */
{DIGIT}+                            { yylval->intval = atoi(yytext); return INTEGER_NUM; }
{DIGIT}+\.{DIGIT}+                  { yylval->floatval = atof(yytext); return DECIMAL_NUM; }
0[xX]{HEX_DIGIT}+                   { yylval->strval = strdup(yytext); return HEX_DIGIT; }

    /* Identifiers */
{LETTER}({LETTER}|{DIGIT})*         { yylval->strval = strdup(yytext); return ID; }
`([^`\\]|\\.)*`                     { yylval->strval = strdup(yytext); return QUOTED_ID; }

    /* Whitespace */
[ \t\n\r]+                          { /* skip whitespace */ }

    /* Default rule for unmatched characters */
.                                   { return yytext[0]; }

%%

/* User code section */
YY_BUFFER_STATE hpcc_scan_string_buffer(const char* str, yyscan_t scanner) {
    return hpccsql_scan_string(str, scanner);
}
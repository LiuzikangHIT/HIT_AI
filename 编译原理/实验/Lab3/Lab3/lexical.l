%option yylineno

%{
    #include <stdio.h>
    #include <string.h>

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    
    #include "tree.h"
    #define YYSTYPE struct MTnode*
    #include "syntax.tab.h"

    int ErrA = 0;
%}

int 0|[1-9][0-9]*
float 0|([1-9][0-9]*)\.[0-9]*
id [a-zA-Z_][a-zA-Z0-9_]*
relop >|<|>=|<=|==|!=

%%
";" {
    yylval = newNode("SEMI\0");
    return SEMI;
}

"," {
    yylval = newNode("COMMA\0");
    return COMMA;
}

"=" {
    yylval = newNode("ASSIGNOP\0");
    return ASSIGNOP;
}

{relop} {
    yylval = newNode("RELOP\0");
    strcpy(yylval->val.msg, yytext);
    return RELOP;
}

"+" {
    yylval = newNode("PLUS\0");
    return PLUS;
}

"-" {
    yylval = newNode("MINUS\0");
    return MINUS;
}

"*" {
    yylval = newNode("STAR\0");
    return STAR;
}

"/" {
    yylval = newNode("DIV\0");
    return DIV;
}

"&&" {
    yylval = newNode("AND\0");
    return AND;
}

"||" {
    yylval = newNode("OR\0");
    return OR;
}


"." {
    yylval = newNode("DOT\0");
    return DOT;
}

"!" {
    yylval = newNode("NOT\0");
    return NOT;
}

"int"|"float" {
    yylval = newNode("TYPE\0");
    strcpy(yylval->val.msg, yytext);
    return TYPE;
}

"(" {
    yylval = newNode("LP\0");
    return LP;
}

")" {
    yylval = newNode("RP\0");
    return RP;
}

"[" {
    yylval = newNode("LB\0");
    return LB;
}

"]" {
    yylval = newNode("RB\0");
    return RB;
}

"{" {
    yylval = newNode("LC\0");
    return LC;
}

"}" {
    yylval = newNode("RC\0");
    return RC;
}

"struct" {
    yylval = newNode("STRUCT\0");
    return STRUCT;
}

"return" {
    yylval = newNode("RETURN\0");
    return RETURN;
}

"if" {
    yylval = newNode("IF\0");
    return IF;
}

"else" {
    yylval = newNode("ELSE\0");
    return ELSE;
}

"while" {
    yylval = newNode("WHILE\0");
    return WHILE;
}

" " { }

\n { yycolumn = 1; }

{int} {
    yylval = newNode("INT\0");
    yylval->val.val_i = atoi(yytext);
    return INT;
}

{float} {
    yylval = newNode("FLOAT\0");
    yylval->val.val_f = atof(yytext);
    return FLOAT;
}

{id} {
    yylval = newNode("ID\0");
    strcpy(yylval->val.msg, yytext);
    return ID;
}

. {
    ErrA = 1;
    fprintf(stderr, "Error type A at Line %d: %s.\n", yylineno, yytext);
}

%%
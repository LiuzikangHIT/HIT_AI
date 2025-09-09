%locations

%{
    #include <stdio.h>
    #include "lex.yy.c"
    #include "tree.h"
    #define YYSTYPE struct MTnode*

    extern int yylineno;
    extern char* yytext;
    extern int yylex();

    int ErrB = 0;
    int lastErrLineno = 0;  // 上一个错误的行号

    void printError(char* msg);
    void yyerror(const char* s);
%}

%token SEMI COMMA
%token TYPE STRUCT
%token LC RC LB RB LP RP
%token ID INT FLOAT
%token RETURN
%token IF ELSE WHILE
%token ASSIGNOP AND OR RELOP PLUS MINUS STAR DIV NOT DOT
%type Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP     // 右结合 赋值
%left OR            // 左结合 或
%left AND           // 左结合 与
%left RELOP         // 左结合 关系运算
%left PLUS MINUS    // 左结合 加减
%left STAR DIV      // 左结合 乘除
%right NOT          // 右结合 非
%left DOT LP LB RP RB      // 左结合 点和括号
%nonassoc LOWER_THAN_ELSE   // 不可结合 比else优先级低的操作符
%nonassoc ELSE              // 不可结合 else操作符

%%
Program : ExtDefList {
        if($1->val.val_i == 0) {
            $$ = createNode("Program\0", $1->line); // 创建新节点，添加名称和行号
            $$->child[$$->num_child++] = $1;        // 添加子节点
        } else {
            $$ = createNode("Program\0", @1.first_line);
            $$->child[$$->num_child++] = $1;
        }
    }
    ;

ExtDefList : ExtDef ExtDefList {
        $$ = createNode("ExtDefList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | {
        $$ = NULL;  // 空节点
    }
    ;

ExtDef : Specifier ExtDecList SEMI {
        $$ = createNode("ExtDef\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Specifier SEMI {
        $$ = createNode("ExtDef\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | Specifier FunDec CompSt {
        $$ = createNode("ExtDef\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Specifier error {
        ErrB = 1;   // 表示遇到语法错误
        if (lastErrLineno != yylineno) {
            printError("Missing \";\"");   // 输出错误信息
        }
    }
    ;

ExtDecList : VarDec {
        $$ = createNode("ExtDecList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | VarDec COMMA ExtDecList {
        $$ = createNode("ExtDecList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | VarDec error ExtDecList {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("text");
        }
    }
    ;

Specifier : TYPE {
        $$ = createNode("Specifier\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | StructSpecifier {
        $$ = createNode("Specifier\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    ;

StructSpecifier : STRUCT OptTag LC DefList RC {
        $$ = createNode("StructSpecifier\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
        $$->child[$$->num_child++] = $5;
    }
    | STRUCT Tag {
        $$ = createNode("StructSpecifier\0", @1.first_line);
        $$->line = @1.first_line;
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    ;

OptTag : ID {
        $$ = createNode("OptTag\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | {
        $$ = NULL;
    }
    ;

Tag : ID {
        $$ = createNode("Tag\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    ;

VarDec : ID {
        $$ = createNode("VarDec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | VarDec LB INT RB {
        $$ = createNode("VarDec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
    }
    | VarDec LB error RB {
        ErrB = 1;
        char msg[32];   // 错误信息缓冲区
        sprintf(msg, "Syntax error, near \"%c\"", yytext[0]);
        if (lastErrLineno != yylineno) {
            printError(msg);
        }
    }
    ;

FunDec : ID LP VarList RP {
        $$ = createNode("FunDec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
    }
    | ID LP RP {
        $$ = createNode("FunDec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | ID LP error RP {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Syntax error between ()");
        }
    }
    ;

VarList : ParamDec COMMA VarList {
        $$ = createNode("VarList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | ParamDec {
        $$ = createNode("VarList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    ;

ParamDec : Specifier VarDec {
        $$ = createNode("ParamDec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    ;

CompSt : LC DefList StmtList RC {
        $$ = createNode("CompSt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
    }
    ;

StmtList : Stmt StmtList {
        $$ = createNode("StmtList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | {
        $$ = NULL;
    }
    ;

Stmt : Exp SEMI {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | CompSt {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | RETURN Exp SEMI {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | IF LP Exp RP Stmt {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
        $$->child[$$->num_child++] = $5;
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
        $$->child[$$->num_child++] = $5;
        $$->child[$$->num_child++] = $6;
        $$->child[$$->num_child++] = $7;
    }
    | WHILE LP Exp RP Stmt {
        $$ = createNode("Stmt\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
        $$->child[$$->num_child++] = $5;
    }
    | Exp error {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Missing \";\"");
        }
    }
    ;

DefList : Def DefList {
        $$ = createNode("DefList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | {
        $$ = NULL;
    }
    ;

Def : Specifier DecList SEMI {
        $$ = createNode("Def\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Specifier error SEMI {
        ErrB = 1;
        char msg[32];
        sprintf(msg, "Syntax error, near \"%c\"", yytext[0]);
        if (lastErrLineno != yylineno) {
            printError(msg);
        }
    }
    | Specifier DecList error {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Missing \";\"");
        }
    }
    ;

DecList : Dec {
        $$ = createNode("DecList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | Dec COMMA DecList {
        $$ = createNode("DecList\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    ;

Dec : VarDec {
        $$ = createNode("Dec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | VarDec ASSIGNOP Exp {
        $$ = createNode("Dec\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    ;

Exp : Exp ASSIGNOP Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp AND Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp OR Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp RELOP Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp PLUS Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp MINUS Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp STAR Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp DIV Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | LP Exp RP {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | MINUS Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | NOT Exp {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
    }
    | ID LP Args RP {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
    }
    | ID LP RP {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp LB Exp RB {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
        $$->child[$$->num_child++] = $4;
    }
    | Exp DOT ID {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | ID {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | INT {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | FLOAT {
        $$ = createNode("Exp\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    | Exp LB error RB {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Syntax error between \"[]\"");
        }
    }
    | Exp ASSIGNOP error {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Syntax error in Exp");
        }
    } | LP error RP {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Syntax error in Exp");
        }
    }
    | ID LP error RP {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Syntax error in Exp");
        }
    }
    | Exp LB Exp error RB {
        ErrB = 1;
        if (lastErrLineno != yylineno) {
            printError("Missing ']'");
        }
    }
    ;

Args : Exp COMMA Args {
        $$ = createNode("Args\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
        $$->child[$$->num_child++] = $2;
        $$->child[$$->num_child++] = $3;
    }
    | Exp {
        $$ = createNode("Args\0", @1.first_line);
        $$->child[$$->num_child++] = $1;
    }
    ;

%%
/* 输出错误信息 */
void printError(char* msg) {
    fprintf(stderr, "Error type B at Line %d: %s.\n", yylineno, msg);
    lastErrLineno = yylineno;
}

/* 避免额外打印错误信息 */
void yyerror(const char* s) {}
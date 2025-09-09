#include <stdio.h>
#include "semantic.h"
#include "syntax.tab.h"
extern FILE* yyin;
extern int yylineno;
extern int ErrA;
extern int ErrB;
extern pNode root;
extern void yyrestart(FILE *input_file);
extern int yylex();
extern int yyparse(void);
extern void printTree(pNode root, int depth);


int main(int argc, char** argv) { 
  if (argc <= 1) 
    return 1;

  FILE* f = fopen(argv[1], "r");
  if (!f) {
    perror(argv[1]);
    return 1;
  }

  yylineno = 1;
  yyrestart(f);

  yyparse();
  fclose(f);

  if (!ErrA && !ErrB) {
    // printTree(root, 0);
    table = initTable();
    traverseTree(root);
    // deleteTable(table);
  }
  
  return 0;
}
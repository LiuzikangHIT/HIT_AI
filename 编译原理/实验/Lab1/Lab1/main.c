#include <stdio.h>
extern FILE* yyin;
extern int yylineno;
extern int ErrA;
extern int ErrB;
extern struct MTnode* root;
extern void yyrestart(FILE *input_file);
extern int yylex();
extern int yyparse(void);
extern void printTree(struct MTnode* root, int depth);


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

  if (ErrA == 0 && ErrB == 0)
    printTree(root, 0);

  return 0;
}
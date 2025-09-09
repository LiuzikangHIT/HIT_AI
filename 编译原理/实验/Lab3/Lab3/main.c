#include <stdio.h>
#include "inter.h"
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

  FILE* fr = fopen(argv[1], "r");
  if (!fr) {
      perror(argv[1]);
      return 1;
  }

  FILE* fw = fopen(argv[2], "wt+");
  if (!fw) {
      perror(argv[2]);
      return 1;
  }

  yylineno = 1;
  yyrestart(fr);

  yyparse();

  if (!ErrA && !ErrB) {
    char *pHead = "head";
    pNode head = newNode(pHead);
    head->child[0] = root;

    // printTree(root, 0);
    table = initTable();
    traverseTree(root);

    interCodeList = newInterCodeList();
    genInterCodes(root);
    if (!interError) {
        //printInterCode(NULL, interCodeList);
        printInterCode(fw, interCodeList);
    }
    // deleteInterCodeList(interCodeList);
    deleteTable(table);
  }

  fclose(fr);
  fclose(fw);
  
  return 0;
}
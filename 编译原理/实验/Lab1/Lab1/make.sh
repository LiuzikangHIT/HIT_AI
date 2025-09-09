flex lexical.l
bison -d syntax.y
gcc main.c outputTree.c syntax.tab.c -lfl -ly -o parser
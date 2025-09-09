flex lexical.l
bison -d syntax.y
gcc main.c tree.c inter.c semantic.c syntax.tab.c -lfl -ly -o parser -w
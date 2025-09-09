#ifndef OUTPUTTREE_H
#define OUTPUTTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 10
#define MAX_CHILD_NUM 10

// 语法树节点结构体
struct MTnode {
    char *name;     // 单元名称
    int isEnd;      // 是否为终结符
    int line;       // 行号
    int num_child;  // 子节点数量
    union {         // 具体值
        char msg[MAX_LEN];
        int value_i;
        float value_f;
    } data;
    struct MTnode* child[MAX_CHILD_NUM];    // 子节点数组
};

// 全局根节点声明
extern struct MTnode* root;

// 函数声明
struct MTnode* newNode(char *name);
struct MTnode* createNode(char *name, int line);
void printNode(struct MTnode* node);
void printTree(struct MTnode* root, int depth);

#endif // OUTPUTTREE_H
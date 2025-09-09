#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "enum.h"

#define MAX_LEN 16
#define MAX_CHILD_NUM 16

// 语法树节点结构体
typedef struct MTnode {
    char* name;     // 单元名称
    int line;       // 行号
    int isEnd;      // 是否为终结符
    union {         // 具体值
        char msg[MAX_LEN];
        int val_i;
        float val_f;
    } val;

    int num_child;  // 子节点数量
    struct MTnode* child[MAX_CHILD_NUM];    // 子节点数组
} Node;

typedef Node* pNode;

// 全局根节点声明
extern pNode root;

// 函数声明
pNode newNode(char* name);
pNode createNode(char* name, int line);
void printNode(pNode node);
void printTree(pNode root, int depth);

static inline char* copyString(char* src) {
    if (src == NULL) return NULL;
    int length = strlen(src) + 1;
    char* p = (char*)malloc(sizeof(char) * length);
    assert(p != NULL);
    strncpy(p, src, length);
    return p;
}

#endif // TREE_H
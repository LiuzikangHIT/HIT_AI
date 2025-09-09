#include "tree.h"


pNode newNode(char* name) {
    pNode node = (pNode)malloc(sizeof(Node));
    node->name = strdup(name);  // 动态分配并复制字符串
    node->isEnd = 1;
    node->line = 0;
    node->num_child = 0;
    for (int i = 0; i < MAX_CHILD_NUM; i++) {
        node->child[i] = NULL;
    }
    return node;
}

pNode root = NULL;

pNode createNode(char* name, int line) {
    pNode node = (pNode)malloc(sizeof(Node));
    node->name = strdup(name);  // 动态分配并复制字符串
    node->isEnd = 0;
    node->line = line;
    node->val.val_i = 1;
    node->num_child = 0;
    for (int i = 0; i < MAX_CHILD_NUM; i++) {
        node->child[i] = NULL;
    }
    root = node;
    return node;
}

void printNode(pNode node) {
    /* 词法单元 */
    if (node->isEnd == 1) {
        if (!strcmp(node->name, "ID")) {
            printf("ID: %s\n", node->val.msg);
        } else if (!strcmp(node->name, "TYPE")) {
            printf("TYPE: %s\n", node->val.msg);
        } else if (!strcmp(node->name, "INT")) {
            printf("INT: %u\n", node->val.val_i);
        } else if (!strcmp(node->name, "FLOAT")) {
            printf("FLOAT: %f\n", node->val.val_f);
        } else {
            printf("%s\n", node->name);
        }
    }
    /* 语法单元 */
    else {
        printf("%s (%d)\n", node->name, node->line);
    }
}

void printTree(pNode root, int depth) {
    if (root == NULL) return;  // 安全检查

    if (root->isEnd == 1 || root->val.val_i) {
        for (int i = 0; i < depth; i++) printf("  ");
        printNode(root);
    }

    // 递归打印子节点
    for (int i = 0; i < root->num_child; i++) {
        printTree(root->child[i], depth + 1);  // 注意 depth + 1 实现缩进
    }
}
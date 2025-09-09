#include "outputTree.h"

struct MTnode* newNode(char *name) {
    struct MTnode *node = (struct MTnode*)malloc(sizeof(struct MTnode));
    node->name = strdup(name);  // 动态分配并复制字符串
    node->isEnd = 1;
    node->line = 0;
    node->num_child = 0;
    return node;
}

struct MTnode* root = NULL;

struct MTnode* createNode(char *name, int line) {
    struct MTnode *node = (struct MTnode*)malloc(sizeof(struct MTnode));
    node->name = strdup(name);  // 动态分配并复制字符串
    node->isEnd = 0;
    node->line = line;
    node->data.value_i = 1;
    node->num_child = 0;
    root = node;
    return node;
}

void printNode(struct MTnode* node) {
    /* 词法单元 */
    if (node->isEnd == 1) {
        if (!strcmp(node->name, "ID")) {
            printf("ID: %s\n", node->data.msg);
        } else if (!strcmp(node->name, "TYPE")) {
            printf("TYPE: %s\n", node->data.msg);
        } else if (!strcmp(node->name, "INT")) {
            printf("INT: %u\n", node->data.value_i);
        } else if (!strcmp(node->name, "FLOAT")) {
            printf("FLOAT: %f\n", node->data.value_f);
        } else {
            printf("%s\n", node->name);
        }
    }
    /* 语法单元 */
    else {
        printf("%s (%d)\n", node->name, node->line);
    }
}

void printTree(struct MTnode* root, int depth) {
    if (root == NULL) return;  // 安全检查

    if (root->isEnd == 1 || root->data.value_i) {
        for (int i = 0; i < depth; i++) printf("  ");
        printNode(root);
    }

    // 递归打印子节点
    for (int i = 0; i < root->num_child; i++) {
        printTree(root->child[i], depth + 1);  // 注意 depth + 1 实现缩进
    }
}
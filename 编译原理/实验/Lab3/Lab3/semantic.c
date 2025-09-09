#include <stdarg.h>
#include "semantic.h"


pTable table;

/* Type functions */
pType newType(Kind kind, ...) {
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = kind;
    va_list args;
    assert(kind == BASIC || kind == ARRAY || 
        kind == FUNCTION);
    switch (kind) {
        case BASIC:
            va_start(args, kind);
            p->u.basic = va_arg(args, BasicType);
            break;
        case ARRAY:
            va_start(args, kind);
            p->u.array.elem = va_arg(args, pType);
            p->u.array.size = va_arg(args, int);
            break;
        case FUNCTION:
            va_start(args, kind);
            p->u.function.argc = va_arg(args, int);
            p->u.function.argv = va_arg(args, pFieldList);
            p->u.function.returnType = va_arg(args, pType);
            break;
    }
    va_end(args);
    return p;
}

pType copyType(pType src) {
    if (src == NULL) return NULL;
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = src->kind;
    assert(p->kind == BASIC || p->kind == ARRAY || 
        p->kind == FUNCTION);
    switch (p->kind) {
        case BASIC:
            p->u.basic = src->u.basic;
            break;
        case ARRAY:
            p->u.array.elem = copyType(src->u.array.elem);
            p->u.array.size = src->u.array.size;
            break;
    }

    return p;
}

void deleteType(pType type) {
    assert(type != NULL);
    assert(type->kind == BASIC || type->kind == ARRAY || 
        type->kind == FUNCTION);
    pFieldList temp = NULL;
    switch (type->kind) {
        case BASIC:
            break;
        case ARRAY:
            deleteType(type->u.array.elem);
            type->u.array.elem = NULL;
            break;
        case FUNCTION:
            deleteType(type->u.function.returnType);
            type->u.function.returnType = NULL;
            temp = type->u.function.argv;
            while (temp) {
                pFieldList tDelete = temp;
                temp = temp->next;
                deleteFieldList(tDelete);
            }
            type->u.function.argv = NULL;
            break;
    }
    free(type);
}

bool checkType(pType type1, pType type2) {
    if (type1 == NULL || type2 == NULL)
        return true;
    else if (type1->kind == FUNCTION || type2->kind == FUNCTION)
        return false;
    else if (type1->kind != type2->kind)
        return false;
    else {
        assert(type1->kind == BASIC || type1->kind == ARRAY);
        switch (type1->kind) {
            case BASIC:
                return type1->u.basic == type2->u.basic;
            case ARRAY:
                return checkType(type1->u.array.elem, type2->u.array.elem);
        }
    }
}

void printType(pType type) {
    if (type == NULL)
        printf("type is NULL.\n");
    else {
        printf("type kind: %d\n", type->kind);
        switch (type->kind) {
            case BASIC:
                printf("type basic: %d\n", type->u.basic);
                break;
            case ARRAY:
                printf("array size: %d\n", type->u.array.size);
                printType(type->u.array.elem);
                break;
            case FUNCTION:
                printf("function argc is %d\n", type->u.function.argc);
                printf("function args:\n");
                printFieldList(type->u.function.argv);
                printf("function return type:\n");
                printType(type->u.function.returnType);
                break;
        }
    }
}

/* FieldList functions */
pFieldList newFieldList(char* newName, pType newType) {
    pFieldList p = (pFieldList)malloc(sizeof(FieldList));
    assert(p != NULL);
    p->name = copyString(newName);
    p->type = newType;
    p->next = NULL;
    return p;
}

pFieldList copyFieldList(pFieldList src) {
    assert(src != NULL);
    pFieldList head = NULL, cur = NULL;
    pFieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->next;
        } else {
            cur->next = newFieldList(temp->name, copyType(temp->type));
            cur = cur->next;
            temp = temp->next;
        }
    }
    return head;
}

void deleteFieldList(pFieldList fieldList) {
    assert(fieldList != NULL);
    if (fieldList->name) {
        free(fieldList->name);
        fieldList->name = NULL;
    }
    if (fieldList->type) deleteType(fieldList->type);
    fieldList->type = NULL;
    free(fieldList);
}

void setFieldListName(pFieldList p, char* newName) {
    assert(p != NULL && newName != NULL);
    if (p->name != NULL) {
        free(p->name);
    }
    p->name = copyString(newName);
}

void printFieldList(pFieldList fieldList) {
    if (fieldList == NULL)
        printf("fieldList is NULL\n");
    else {
        printf("fieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->next);
    }
}

/* tableItem functions */
pItem newItem(int symbolDepth, pFieldList pfield) {
    pItem p = (pItem)malloc(sizeof(TableItem));
    assert(p != NULL);
    p->symbolDepth = symbolDepth;
    p->field = pfield;
    p->nextHash = NULL;
    p->nextSymbol = NULL;
    return p;
}

void deleteItem(pItem item) {
    assert(item != NULL);
    if (item->field != NULL) deleteFieldList(item->field);
    free(item);
}

/* Hash functions */
pHash newHash() {
    pHash p = (pHash)malloc(sizeof(HashTable));
    assert(p != NULL);
    p->hashArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->hashArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->hashArray[i] = NULL;
    }
    return p;
}

void deleteHash(pHash hash) {
    assert(hash != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem temp = hash->hashArray[i];
        while (temp) {
            pItem tdelete = temp;
            temp = temp->nextHash;
            deleteItem(tdelete);
        }
        hash->hashArray[i] = NULL;
    }
    free(hash->hashArray);
    hash->hashArray = NULL;
    free(hash);
}

pItem getHashHead(pHash hash, int index) {
    assert(hash != NULL);
    return hash->hashArray[index];
}

void setHashHead(pHash hash, int index, pItem newVal) {
    assert(hash != NULL);
    hash->hashArray[index] = newVal;
}

/* Table functions */
pTable initTable() {
    pTable table = (pTable)malloc(sizeof(Table));
    assert(table != NULL);
    table->hash = newHash();
    table->stack = newStack();
    table->unNamedStructNum = 0;

    // 添加read和write函数
    pItem readFun = newItem(
        0, newFieldList(copyString("read"),
                        newType(FUNCTION, 0, NULL, newType(BASIC, INT_TYPE))));

    pItem writeFun = newItem(
        0, newFieldList(copyString("write"),
                        newType(FUNCTION, 1,
                                newFieldList("arg1", newType(BASIC, INT_TYPE)),
                                newType(BASIC, INT_TYPE))));

    addTableItem(table, readFun);
    addTableItem(table, writeFun);

    return table;
};

void deleteTable(pTable table) {
    deleteHash(table->hash);
    table->hash = NULL;
    deleteStack(table->stack);
    table->stack = NULL;
    free(table);
};

pItem searchTableItem(pTable table, char* name) {
    unsigned hashCode = getHashCode(name);
    pItem temp = getHashHead(table->hash, hashCode);
    if (temp == NULL) return NULL;
    while (temp) {
        if (!strcmp(temp->field->name, name)) return temp;
        temp = temp->nextHash;
    }
    return NULL;
}

// Return false -> no confliction, true -> has confliction
bool checkTableItemConflict(pTable table, pItem item) {
    pItem temp = searchTableItem(table, item->field->name);
    if (temp == NULL) return false;
    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            if (temp->symbolDepth == table->stack->curStackDepth) return true;
        }
        temp = temp->nextHash;
    }
    return false;
}

void addTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    pHash hash = table->hash;
    pStack stack = table->stack;

    item->nextSymbol = stack == NULL ? NULL : stack->stackArray[stack->curStackDepth];
    stack->stackArray[stack->curStackDepth] = item;

    item->nextHash = getHashHead(hash, hashCode);
    setHashHead(hash, hashCode, item);
}

void deleteTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    if (item == getHashHead(table->hash, hashCode))
        setHashHead(table->hash, hashCode, item->nextHash);
    else {
        pItem cur = getHashHead(table->hash, hashCode);
        pItem last = cur;
        while (cur != item) {
            last = cur;
            cur = cur->nextHash;
        }
        last->nextHash = cur->nextHash;
    }
    deleteItem(item);
}

void clearCurDepthStackList(pTable table) {
    assert(table != NULL);
    pStack stack = table->stack;
    pItem temp = stack == NULL ? NULL : stack->stackArray[stack->curStackDepth];
    while (temp) {
        pItem tDelete = temp;
        temp = temp->nextSymbol;
        deleteTableItem(table, tDelete);
    }
    stack->stackArray[stack->curStackDepth] = NULL;
    stack->curStackDepth--;
}

/* Stack functions */
pStack newStack() {
    pStack p = (pStack)malloc(sizeof(Stack));
    assert(p != NULL);
    p->stackArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->stackArray[i] = NULL;
    }
    p->curStackDepth = 0;
    return p;
}

void deleteStack(pStack stack) {
    assert(stack != NULL);
    free(stack->stackArray);
    stack->stackArray = NULL;
    stack->curStackDepth = 0;
    free(stack);
}

/* Global function */
void traverseTree(pNode node) {
    // printTree(node, 0);
    if (node == NULL) 
        return;
    else if (!strcmp(node->name, "ExtDef")) ExtDef(node);
    for (int i = 0; i < node->num_child; i++) {
        traverseTree(node->child[i]);
    }
    // printf("%s\n", node->name);
}

/* Generate symbol table functions */
void ExtDef(pNode node) {
    assert(node != NULL);
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    pType specifierType = Specifier(node->child[0]);
    // printTree(node, 0);
    char* secondName = node->child[1]->name;

    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList")) {
        ExtDecList(node, specifierType);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(secondName, "FunDec")) {
        FunDec(node->child[1], specifierType);
        CompSt(node->child[2], specifierType);
    }
    // ExtDef -> Specifier SEMI
    // this situation has no meaning or is struct define(have been processe inSpecifier())
    if (specifierType) deleteType(specifierType);
}

void ExtDecList(pNode node, pType specifier) {
    assert(node != NULL);
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    pNode ini = node->child[3], temp = node->child[1];
    while (temp) {
        pItem item = VarDec(temp->child[0], specifier);
        if (checkTableItemConflict(table, item)) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            pError(REDEF_VAR, temp->line, msg);
            deleteItem(item);
        } else {
            addTableItem(table, item);
        }
        if (temp->child[1]) {
            // temp = temp->next->next->child;
            temp = ini->child[0];
            ini = ini->child[2];
        } else {
            break;
        }
    }
}

pType Specifier(pNode node) {
    assert(node != NULL);
    // Specifier -> TYPE
    //            | StructSpecifier

    pNode t = node->child[0];
    // Specifier -> TYPE
    if (!strcmp(t->name, "TYPE")) {
        if (!strcmp(t->val.msg, "float")) {
            return newType(BASIC, FLOAT_TYPE);
        } else {
            return newType(BASIC, INT_TYPE);
        }
    }
    // Specifier -> StructSpecifier
    else {
        return NULL;
    }
}

pItem VarDec(pNode node, pType specifier) {
    assert(node != NULL);
    // VarDec -> ID
    //         | VarDec LB INT RB
    pNode id = node;
    // get ID
    while (id->child[0]) id = id->child[0];
    pItem p = newItem(table->stack->curStackDepth, newFieldList(id->val.msg, NULL));

    // VarDec -> ID
    if (!strcmp(node->child[0]->name, "ID")) {
        // printf("copy type tp %s.\n", node->child[0]->val.msg);
        p->field->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else {
        pNode varDec = node;
        pType temp = specifier;
        // printf("VarDec -> VarDec LB INT RB.\n");
        while (varDec->child[1]) {
            p->field->type = newType(ARRAY, copyType(temp), atoi(varDec->child[2]->val.msg));
            temp = p->field->type;
            varDec = varDec->child[0];
        }
    }
    return p;
}

void FunDec(pNode node, pType returnType) {
    assert(node != NULL);
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    pItem p =
        newItem(table->stack->curStackDepth,
                newFieldList(node->child[0]->val.msg,
                            newType(FUNCTION, 0, NULL, copyType(returnType))));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child[2]->name, "VarList")) {
        VarList(node->child[2], p);
    }

    // FunDec -> ID LP RP don't need process

    addTableItem(table, p);
}

void VarList(pNode node, pItem func) {
    assert(node != NULL);
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    table->stack->curStackDepth++;
    int argc = 0;
    pNode temp = node;
    pFieldList cur = NULL;

    // VarList -> ParamDec
    pFieldList paramDec = ParamDec(temp->child[0]);
    func->field->type->u.function.argv = copyFieldList(paramDec);
    cur = func->field->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->num_child > 1) {
        temp = temp->child[2];
        paramDec = ParamDec(temp->child[0]);
        if (paramDec) {
            cur->next = copyFieldList(paramDec);
            cur = cur->next;
            argc++;
        }
    }
    func->field->type->u.function.argc = argc;

    table->stack->curStackDepth--;
}

pFieldList ParamDec(pNode node) {
    assert(node != NULL);
    // ParamDec -> Specifier VarDec
    pType specifierType = Specifier(node->child[0]);
    pItem p = VarDec(node->child[1], specifierType);
    if (specifierType) deleteType(specifierType);
    if (checkTableItemConflict(table, p)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        pError(REDEF_VAR, node->line, msg);
        deleteItem(p);
        return NULL;
    } else {
        addTableItem(table, p);
        return p->field;
    }
}

void CompSt(pNode node, pType returnType) {
    assert(node != NULL);
    // printTree(node, 0);
    // CompSt -> LC DefList StmtList RC
    table->stack->curStackDepth++;

    if (node->child[1] != NULL && !strcmp(node->child[1]->name, "DefList")) {
        DefList(node->child[1]);
    }
    if (node->child[2] != NULL && !strcmp(node->child[2]->name, "StmtList")) {
        StmtList(node->child[2], returnType);
    }
    // clearCurDepthStackList(table);
}

void StmtList(pNode node, pType returnType) {
    // assert(node != NULL);
    // StmtList -> Stmt StmtList
    //           | e
    while (node) {
        // printTree(node, 0);
        Stmt(node->child[0], returnType);
        node = node->child[1];
    }
}

void Stmt(pNode node, pType returnType) {
    assert(node != NULL);
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    pType expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child[0]->name, "Exp")) expType = Exp(node->child[0]);

    // Stmt -> CompSt
    else if (!strcmp(node->child[0]->name, "CompSt"))
        CompSt(node->child[0], returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child[0]->name, "RETURN")) {
        expType = Exp(node->child[1]);
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child[0]->name, "IF")) {
        // printTree(node, 0);
        pNode stmt = node->child[4];
        expType = Exp(node->child[2]);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (node->child[5] != NULL) Stmt(node->child[6], returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child[0]->name, "WHILE")) {
        expType = Exp(node->child[2]);
        Stmt(node->child[4], returnType);
    }
    
    if (expType) deleteType(expType);
}

void DefList(pNode node) {
    // assert(node != NULL);
    // DefList -> Def DefList
    //          | e
    while (node) {
        // printTree(node, 0);
        Def(node->child[0]);
        node = node->child[1];
    }
}

void Def(pNode node) {
    assert(node != NULL);
    // printTree(node, 0);
    // Def -> Specifier DecList SEMI
    pType dectype = Specifier(node->child[0]);
    DecList(node->child[1], dectype);

    if (dectype) deleteType(dectype);
}

void DecList(pNode node, pType specifier) {
    assert(node != NULL);
    // DecList -> Dec
    //          | Dec COMMA DecList
    pNode temp = node;
    while (temp) {
        Dec(temp->child[0], specifier);
        if (temp->child[1])
            temp = temp->child[2];
        else
            break;
    }
}

void Dec(pNode node, pType specifier) {
    assert(node != NULL);
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child[1] == NULL) {
        // 判断返回的item有无冲突，无冲突放入表中，有冲突报错delete
        pItem decitem = VarDec(node->child[0], specifier);
        if (checkTableItemConflict(table, decitem)) {
            // 出现冲突，报错
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", decitem->field->name);
            pError(REDEF_VAR, node->line, msg);
            deleteItem(decitem);
        } else {
            addTableItem(table, decitem);
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        // 判断赋值类型是否相符
        // 如果成功，注册该符号
        pItem decitem = VarDec(node->child[0], specifier);
        pType exptype = Exp(node->child[2]);
        if (checkTableItemConflict(table, decitem)) {
            // 出现冲突，报错
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".",
                    decitem->field->name);
            pError(REDEF_VAR, node->line, msg);
            deleteItem(decitem);
        }
        if (!checkType(decitem->field->type, exptype)) {
            // 类型不相符
            // 报错
            pError(TYPE_MISMATCH_ASSIGN, node->line,
                    "Type mismatchedfor assignment.");
            deleteItem(decitem);
        }
        if (decitem->field->type && decitem->field->type->kind == ARRAY) {
            // 报错，对非basic类型赋值
            pError(TYPE_MISMATCH_ASSIGN, node->line,
                    "Illegal initialize variable.");
            deleteItem(decitem);
        } else {
            addTableItem(table, decitem);
        }
        // exp不出意外应该返回一个无用的type，删除
        if (exptype) deleteType(exptype);
    }
}

pType Exp(pNode node) {
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    pNode t = node;
    // exp will only check if the cal is right
    // 二值运算
    if (!strcmp(t->child[0]->name, "Exp")) {
        // printTree(t, 0);
        // 基本数学运算符
        if (strcmp(t->child[1]->name, "LB") && strcmp(t->child[1]->name, "DOT")) {
            pType p1 = Exp(t->child[0]);
            pType p2 = Exp(t->child[2]);
            pType returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->child[1]->name, "ASSIGNOP")) {
                // 检查左值
                pNode tchild = t->child[0];

                if (!strcmp(tchild->child[0]->name, "FLOAT") ||
                    !strcmp(tchild->child[0]->name, "INT")) {
                    // 报错，左值
                    pError(LEFT_VAR_ASSIGN, t->child[0]->line,
                           "The left-hand side of an assignment must be "
                           "avariable.");

                } else if (!strcmp(tchild->child[0]->name, "ID") ||
                           !strcmp(tchild->child[1]->name, "LB") ||
                           !strcmp(tchild->child[1]->name, "DOT")) {
                    if (!checkType(p1, p2)) {
                        // 报错，类型不匹配
                        pError(TYPE_MISMATCH_ASSIGN, t->child[0]->line,
                               "Type mismatched for assignment.");
                    } else
                        returnType = copyType(p1);
                } else {
                    // 报错，左值
                    pError(LEFT_VAR_ASSIGN, t->child[0]->line,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }

            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY)) {
                    // 报错，数组、结构体运算
                    pError(TYPE_MISMATCH_OP, t->child[0]->line,
                           "Type mismatched for operands.");
                } else if (!checkType(p1, p2)) {
                    // 报错，类型不匹配
                    pError(TYPE_MISMATCH_OP, t->child[0]->line,
                           "Type mismatched for operands.");
                } else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1) deleteType(p1);
            if (p2) deleteType(p2);
            return returnType;
        }
        // 数组访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->child[1]->name, "LB")) {
                // 数组
                pType p1 = Exp(t->child[0]);
                pType p2 = Exp(t->child[2]);
                pType returnType = NULL;

                if (!p1) {
                    // 第一个 exp 为 null，上层报错，这里不用再管
                } else if (p1 && p1->kind != ARRAY) {
                    // 报错，非数组使用[]运算符
                    char msg[100] = {0};
                    // printTree(t, 0);
                    sprintf(msg, "\"%s\" is not an array.", t->child[0]->child[0]->val.msg);
                    pError(NOT_A_ARRAY, t->child[0]->line, msg);
                } else if (!p2 || p2->kind != BASIC ||
                           p2->u.basic != INT_TYPE) {
                    // 报错，不用 int 索引访问数组
                    char msg[100] = {0};
                    if (t->child[2]->child[0]->val.val_f > 1e-9) {
                        sprintf(msg, "\"%g\" is not an integer.", t->child[2]->child[0]->val.val_f);
                    } else {
                        sprintf(msg, "\"%s\" is not an integer.", t->child[2]->child[0]->val.msg);
                    }
                    pError(NOT_A_INT, t->child[0]->line, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) deleteType(p1);
                if (p2) deleteType(p2);
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->child[0]->name, "MINUS") || !strcmp(t->child[0]->name, "NOT")) {
        pType p1 = Exp(t->child[1]);
        pType returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            // 报错，数组、结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, t->child[0]->line,
                   "TYPE_MISMATCH_OP");
        } else {
            returnType = copyType(p1);
        }
        if (p1) deleteType(p1);
        return returnType;
    } else if (!strcmp(t->child[0]->name, "LP")) {
        return Exp(t->child[1]);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->child[0]->name, "ID") && t->child[1]) {
        pItem funcInfo = searchTableItem(table, t->child[0]->val.msg);

        // Exp -> ID LP Args RP
        if (!strcmp(t->child[2]->name, "Args")) {
            Args(t->child[2], funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else {
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->child[0]->name, "ID")) {
        pItem tp = searchTableItem(table, t->child[0]->val.msg);
        if (tp == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->child[0]->val.msg);
            pError(UNDEF_VAR, t->child[0]->line, msg);
            return NULL;
        } else {
            // good
            return copyType(tp->field->type);
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->child[0]->name, "FLOAT")) {
            return newType(BASIC, FLOAT_TYPE);
        }
        // Exp -> INT
        else {
            return newType(BASIC, INT_TYPE);
        }
    }
}

void Args(pNode node, pItem funcInfo) {
    assert(node != NULL);
    // Args -> Exp COMMA Args
    //       | Exp
    pNode temp = node;
    pFieldList arg = funcInfo->field->type->u.function.argv;
    while (temp) {
        pType realType = Exp(temp->child[0]);
        if (realType) deleteType(realType);

        arg = arg->next;
        if (temp->child[1]) {
            temp = temp->child[2];
        } else {
            break;
        }
    }
}

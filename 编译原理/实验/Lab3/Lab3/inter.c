#include <stdarg.h>
#include "inter.h"


bool interError = false;
pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...) {
    pOperand p = (pOperand)malloc(sizeof(Operand));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind >= 0 && kind < 6);
    va_start(vaList, kind);
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = va_arg(vaList, int);
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            p->u.name = va_arg(vaList, char*);
            break;
    }

    // p->isAddr = false;

    return p;
}

void setOperand(pOperand p, int kind, void* val) {
    assert(p != NULL);
    assert(kind >= 0 && kind < 6);
    p->kind = kind;
    switch (kind) {
        case OP_CONSTANT:
            p->u.value = (int)val;
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            if (p->u.name) free(p->u.name);
            p->u.name = (char*)val;
            break;
    }
}

void deleteOperand(pOperand p) {
    if (p == NULL) return;
    assert(p->kind >= 0 && p->kind < 6);
    switch (p->kind) {
        case OP_CONSTANT:
            break;
        case OP_VARIABLE:
        case OP_ADDRESS:
        case OP_LABEL:
        case OP_FUNCTION:
        case OP_RELOP:
            if (p->u.name) {
                free(p->u.name);
                p->u.name = NULL;
            }
            break;
    }
    free(p);
}

void printOp(FILE* fp, pOperand op) {
    assert(op != NULL);
    if (fp == NULL) {
        printf("Error: Fail to open file.");
    } else {
        switch (op->kind) {
            case OP_CONSTANT:
                fprintf(fp, "#%d", op->u.value);
                break;
            case OP_VARIABLE:
            case OP_ADDRESS:
            case OP_LABEL:
            case OP_FUNCTION:
            case OP_RELOP:
                fprintf(fp, "%s", op->u.name);
                break;
        }
    }
}

// InterCode func
pInterCode newInterCode(int kind, ...) {
    pInterCode p = (pInterCode)malloc(sizeof(InterCode));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind >= 0 && kind < 19);
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(vaList, kind);
            p->u.oneOp.op = va_arg(vaList, pOperand);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(vaList, kind);
            p->u.assign.left = va_arg(vaList, pOperand);
            p->u.assign.right = va_arg(vaList, pOperand);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(vaList, kind);
            p->u.binOp.result = va_arg(vaList, pOperand);
            p->u.binOp.op1 = va_arg(vaList, pOperand);
            p->u.binOp.op2 = va_arg(vaList, pOperand);
            break;
        case IR_DEC:
            va_start(vaList, kind);
            p->u.dec.op = va_arg(vaList, pOperand);
            p->u.dec.size = va_arg(vaList, int);
            break;
        case IR_IF_GOTO:
            va_start(vaList, kind);
            p->u.ifGoto.x = va_arg(vaList, pOperand);
            p->u.ifGoto.relop = va_arg(vaList, pOperand);
            p->u.ifGoto.y = va_arg(vaList, pOperand);
            p->u.ifGoto.z = va_arg(vaList, pOperand);
    }
    return p;
}

void deleteInterCode(pInterCode p) {
    assert(p != NULL);
    assert(p->kind >= 0 && p->kind < 19);
    switch (p->kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            deleteOperand(p->u.oneOp.op);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            deleteOperand(p->u.assign.left);
            deleteOperand(p->u.assign.right);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            deleteOperand(p->u.binOp.result);
            deleteOperand(p->u.binOp.op1);
            deleteOperand(p->u.binOp.op2);
            break;
        case IR_DEC:
            deleteOperand(p->u.dec.op);
            break;
        case IR_IF_GOTO:
            deleteOperand(p->u.ifGoto.x);
            deleteOperand(p->u.ifGoto.relop);
            deleteOperand(p->u.ifGoto.y);
            deleteOperand(p->u.ifGoto.z);
    }
    free(p);
}

void printInterCode(FILE* fp, pInterCodeList interCodeList) {
    for (pInterCodes cur = interCodeList->head; cur != NULL; cur = cur->next) {
        assert(cur->code->kind >= 0 && cur->code->kind < 19);
        if (fp == NULL) {
            printf("Error: Fail to open file.");
        } else {
            switch (cur->code->kind) {
                case IR_LABEL:
                    fprintf(fp, "LABEL ");
                    printOp(fp, cur->code->u.oneOp.op);
                    fprintf(fp, " :");
                    break;
                case IR_FUNCTION:
                    fprintf(fp, "FUNCTION ");
                    printOp(fp, cur->code->u.oneOp.op);
                    fprintf(fp, " :");
                    break;
                case IR_ASSIGN:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_ADD:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " + ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_SUB:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " - ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_MUL:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " * ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_DIV:
                    printOp(fp, cur->code->u.binOp.result);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.binOp.op1);
                    fprintf(fp, " / ");
                    printOp(fp, cur->code->u.binOp.op2);
                    break;
                case IR_GET_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := &");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_READ_ADDR:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := *");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_WRITE_ADDR:
                    fprintf(fp, "*");
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_GOTO:
                    fprintf(fp, "GOTO ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_IF_GOTO:
                    fprintf(fp, "IF ");
                    printOp(fp, cur->code->u.ifGoto.x);
                    fprintf(fp, " ");
                    printOp(fp, cur->code->u.ifGoto.relop);
                    fprintf(fp, " ");
                    printOp(fp, cur->code->u.ifGoto.y);
                    fprintf(fp, " GOTO ");
                    printOp(fp, cur->code->u.ifGoto.z);
                    break;
                case IR_RETURN:
                    fprintf(fp, "RETURN ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_DEC:
                    fprintf(fp, "DEC ");
                    printOp(fp, cur->code->u.dec.op);
                    fprintf(fp, " ");
                    fprintf(fp, "%d", cur->code->u.dec.size);
                    break;
                case IR_ARG:
                    fprintf(fp, "ARG ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_CALL:
                    printOp(fp, cur->code->u.assign.left);
                    fprintf(fp, " := CALL ");
                    printOp(fp, cur->code->u.assign.right);
                    break;
                case IR_PARAM:
                    fprintf(fp, "PARAM ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_READ:
                    fprintf(fp, "READ ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
                case IR_WRITE:
                    fprintf(fp, "WRITE ");
                    printOp(fp, cur->code->u.oneOp.op);
                    break;
            }
            fprintf(fp, "\n");
        }
    }
}

// InterCodes func
pInterCodes newInterCodes(pInterCode code) {
    pInterCodes p = (pInterCodes)malloc(sizeof(InterCodes));
    assert(p != NULL);
    p->code = code;
    p->prev = NULL;
    p->next = NULL;
}

void deleteInterCodes(pInterCodes p) {
    assert(p != NULL);
    deleteInterCode(p->code);
    free(p);
}

// Arg and ArgList func
pArg newArg(pOperand op) {
    pArg p = (pArg)malloc(sizeof(Arg));
    assert(p != NULL);
    p->op = op;
    p->next = NULL;
}

pArgList newArgList() {
    pArgList p = (pArgList)malloc(sizeof(ArgList));
    assert(p != NULL);
    p->head = NULL;
    p->cur = NULL;
}

void deleteArg(pArg p) {
    assert(p != NULL);
    deleteOperand(p->op);
    free(p);
}

void deleteArgList(pArgList p) {
    assert(p != NULL);
    pArg q = p->head;
    while (q) {
        pArg temp = q;
        q = q->next;
        deleteArg(temp);
    }
    free(p);
}

void addArg(pArgList argList, pArg arg) {
    if (argList->head == NULL) {
        argList->head = arg;
        argList->cur = arg;
    } else {
        argList->cur->next = arg;
        argList->cur = arg;
    }
}

// InterCodeList func
pInterCodeList newInterCodeList() {
    pInterCodeList p = (pInterCodeList)malloc(sizeof(InterCodeList));
    p->head = NULL;
    p->cur = NULL;
    p->lastArrayName = NULL;
    p->tempVarNum = 1;
    p->labelNum = 1;
}

void deleteInterCodeList(pInterCodeList p) {
    assert(p != NULL);
    pInterCodes q = p->head;
    while (q) {
        pInterCodes temp = q;
        q = q->next;
        deleteInterCodes(temp);
    }
    free(p);
}

void addInterCode(pInterCodeList interCodeList, pInterCodes newCode) {
    if (interCodeList->head == NULL) {
        interCodeList->head = newCode;
        interCodeList->cur = newCode;
    } else {
        interCodeList->cur->next = newCode;
        newCode->prev = interCodeList->cur;
        interCodeList->cur = newCode;
    }
}

// traverse func
pOperand newTemp() {
    // printf("newTemp() tempVal:%d\n", interCodeList->tempVarNum);
    char tName[10] = {0};
    sprintf(tName, "t%d", interCodeList->tempVarNum);
    interCodeList->tempVarNum++;
    pOperand temp = newOperand(OP_VARIABLE, copyString(tName));
    return temp;
}

pOperand newLabel() {
    char lName[10] = {0};
    sprintf(lName, "label%d", interCodeList->labelNum);
    interCodeList->labelNum++;
    pOperand temp = newOperand(OP_LABEL, copyString(lName));
    return temp;
}

int getSize(pType type) {
    if (type == NULL)
        return 0;
    else if (type->kind == BASIC)
        return 4;
    else if (type->kind == ARRAY)
        return type->u.array.size * getSize(type->u.array.elem);
    return 0;
}

void genInterCodes(pNode node) {
    // printTree(node, 0);
    if (node->child[0] == NULL) return;
    if (!strcmp(node->child[0]->name, "ExtDefList"))
        translateExtDefList(node->child[0]);
    else {
        genInterCodes(node->child[0]);
        genInterCodes(node->child[1]);
    }
}

void genInterCode(int kind, ...) {
    va_list vaList;
    pOperand temp = NULL;
    pOperand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    pInterCodes newCode = NULL;
    assert(kind >= 0 && kind < 19);
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(vaList, kind);
            op1 = va_arg(vaList, pOperand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, op1));
            addInterCode(interCodeList, newCode);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(vaList, kind);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (kind == IR_ASSIGN &&
                (op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS)) {
                if (op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
                    genInterCode(IR_WRITE_ADDR, op1, op2);
                else if (op2->kind == OP_ADDRESS && op1->kind != OP_ADDRESS)
                    genInterCode(IR_READ_ADDR, op1, op2);
                else {
                    temp = newTemp();
                    genInterCode(IR_READ_ADDR, temp, op2);
                    genInterCode(IR_WRITE_ADDR, op1, temp);
                }
            } else {
                newCode = newInterCodes(newInterCode(kind, op1, op2));
                addInterCode(interCodeList, newCode);
            }
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(vaList, kind);
            result = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            if (op2->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op2);
                op2 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, result, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
        case IR_DEC:
            va_start(vaList, kind);
            op1 = va_arg(vaList, pOperand);
            size = va_arg(vaList, int);
            newCode = newInterCodes(newInterCode(kind, op1, size));
            addInterCode(interCodeList, newCode);
            break;
        case IR_IF_GOTO:
            va_start(vaList, kind);
            result = va_arg(vaList, pOperand);
            relop = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            newCode =
                newInterCodes(newInterCode(kind, result, relop, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
    }
}
void translateExtDefList(pNode node) {
    // ExtDefList -> ExtDef ExtDefList
    //             | e
    while (node) {
        translateExtDef(node->child[0]);
        node = node->child[1];
    }
}

void translateExtDef(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt

    // 因为没有全局变量使用，ExtDecList不涉及中间代码生成，类型声明也不涉及，
    // 所以只需要处理FunDec和CompSt
    if (!strcmp(node->child[1]->name, "FunDec")) {
        translateFunDec(node->child[1]);
        translateCompSt(node->child[2]);
    }
}

void translateFunDec(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    genInterCode(IR_FUNCTION,
                 newOperand(OP_FUNCTION, copyString(node->child[0]->val.msg)));

    pItem funcItem = searchTableItem(table, node->child[0]->val.msg);
    pFieldList temp = funcItem->field->type->u.function.argv;
    while (temp) {
        genInterCode(IR_PARAM, newOperand(OP_VARIABLE, copyString(temp->name)));
        temp = temp->next;
    }
}

void translateCompSt(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // CompSt -> LC DefList StmtList RC
    if (node->child[1] != NULL && !strcmp(node->child[1]->name, "DefList")) {
        translateDefList(node->child[1]);
    }
    if (node->child[2] != NULL && !strcmp(node->child[2]->name, "StmtList")) {
        translateStmtList(node->child[2]);
    }
}

void translateDefList(pNode node) {
    if (interError) return;
    // DefList -> Def DefList
    //          | e
    while (node) {
        translateDef(node->child[0]);
        node = node->child[1];
    }
}

void translateDef(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Def -> Specifier DecList SEMI
    translateDecList(node->child[1]);
}

void translateDecList(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // DecList -> Dec
    //          | Dec COMMA DecList
    pNode temp = node;
    while (temp) {
        translateDec(temp->child[0]);
        if (temp->child[1])
            temp = temp->child[2];
        else
            break;
    }
}

void translateDec(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child[1] == NULL) {
        translateVarDec(node->child[0], NULL);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        pOperand t1 = newTemp();
        translateVarDec(node->child[0], t1);
        pOperand t2 = newTemp();
        translateExp(node->child[2], t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}

void translateVarDec(pNode node, pOperand place) {
    assert(node != NULL);
    if (interError) return;
    // VarDec -> ID
    //         | VarDec LB INT RB

    if (!strcmp(node->child[0]->name, "ID")) {
        pItem temp = searchTableItem(table, node->child[0]->val.msg);
        pType type = temp->field->type;
        if (type->kind == BASIC) {
            if (place) {
                interCodeList->tempVarNum--;
                setOperand(place, OP_VARIABLE,
                           (void*)copyString(temp->field->name));
            }
        } else if (type->kind == ARRAY) {
            // 不需要完成高维数组情况
            if (type->u.array.elem->kind == ARRAY) {
                interError = true;
                printf(
                    "Cannot translate: Code containsvariables of "
                    "multi-dimensional array type or parameters of array "
                    "type.\n");
                return;
            } else {
                genInterCode(
                    IR_DEC,
                    newOperand(OP_VARIABLE, copyString(temp->field->name)),
                    getSize(type));
            }
        }
    } else {
        translateVarDec(node->child[0], place);
    }
}

void translateStmtList(pNode node) {
    if (interError) return;
    // StmtList -> Stmt StmtList
    //           | e
    while (node) {
        translateStmt(node->child[0]);
        node = node->child[1];
    }
}

void translateStmt(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    // Stmt -> Exp SEMI

    if (!strcmp(node->child[0]->name, "Exp")) {
        translateExp(node->child[0], NULL);
    }

    // Stmt -> CompSt
    else if (!strcmp(node->child[0]->name, "CompSt")) {
        translateCompSt(node->child[0]);
    }

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child[0]->name, "RETURN")) {
        pOperand t1 = newTemp();
        translateExp(node->child[1], t1);
        genInterCode(IR_RETURN, t1);
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child[0]->name, "IF")) {
        pNode exp = node->child[2];
        pNode stmt = node->child[4];
        pOperand label1 = newLabel();
        pOperand label2 = newLabel();

        translateCond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        translateStmt(stmt);
        if (node->child[5] == NULL) {
            genInterCode(IR_LABEL, label2);
        }
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        else {
            pOperand label3 = newLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            translateStmt(node->child[6]);
            genInterCode(IR_LABEL, label3);
        }

    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child[0]->name, "WHILE")) {
        pOperand label1 = newLabel();
        pOperand label2 = newLabel();
        pOperand label3 = newLabel();

        genInterCode(IR_LABEL, label1);
        translateCond(node->child[2], label2, label3);
        genInterCode(IR_LABEL, label2);
        translateStmt(node->child[4]);
        genInterCode(IR_GOTO, label1);
        genInterCode(IR_LABEL, label3);
    }
}

void translateExp(pNode node, pOperand place) {
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp

    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT

    // Exp -> LP Exp RP
    if (!strcmp(node->child[0]->name, "LP"))
        translateExp(node->child[1], place);

    else if (!strcmp(node->child[0]->name, "Exp") ||
             !strcmp(node->child[0]->name, "NOT")) {
        // 条件表达式 和 基本表达式
        if (strcmp(node->child[1]->name, "LB") &&
            strcmp(node->child[1]->name, "DOT")) {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
            if (!strcmp(node->child[1]->name, "AND") ||
                !strcmp(node->child[1]->name, "OR") ||
                !strcmp(node->child[1]->name, "RELOP") ||
                !strcmp(node->child[0]->name, "NOT")) {
                pOperand label1 = newLabel();
                pOperand label2 = newLabel();
                pOperand true_num = newOperand(OP_CONSTANT, 1);
                pOperand false_num = newOperand(OP_CONSTANT, 0);
                genInterCode(IR_ASSIGN, place, false_num);
                translateCond(node, label1, label2);
                genInterCode(IR_LABEL, label1);
                genInterCode(IR_ASSIGN, place, true_num);
            } else {
                // Exp -> Exp ASSIGNOP Exp
                if (!strcmp(node->child[1]->name, "ASSIGNOP")) {
                    pOperand t2 = newTemp();
                    translateExp(node->child[2], t2);
                    pOperand t1 = newTemp();
                    translateExp(node->child[0], t1);
                    genInterCode(IR_ASSIGN, t1, t2);
                } else {
                    pOperand t1 = newTemp();
                    translateExp(node->child[0], t1);
                    pOperand t2 = newTemp();
                    translateExp(node->child[2], t2);
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->child[1]->name, "PLUS")) {
                        genInterCode(IR_ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->child[1]->name, "MINUS")) {
                        genInterCode(IR_SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->child[1]->name, "STAR")) {
                        genInterCode(IR_MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->child[1]->name, "DIV")) {
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }

        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->child[1]->name, "LB")) {
                //数组
                if (node->child[0]->child[1] &&
                    !strcmp(node->child[0]->child[1]->name, "LB")) {
                    //多维数组，报错
                    interError = true;
                    printf(
                        "Cannot translate: Code containsvariables of "
                        "multi-dimensional array type or parameters of array type.\n");
                    return;
                } else {
                    pOperand idx = newTemp();
                    translateExp(node->child[2], idx);
                    pOperand base = newTemp();
                    translateExp(node->child[0], base);

                    pOperand width;
                    pOperand offset = newTemp();
                    pOperand target;
                    // 根据假设，Exp 只会展开为 ID，需要对 ID 取址
                    target = newTemp();
                    genInterCode(IR_GET_ADDR, target, base);

                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                }
            }
            // Exp -> Exp DOT ID
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    else if (!strcmp(node->child[0]->name, "MINUS")) {
        pOperand t1 = newTemp();
        translateExp(node->child[1], t1);
        pOperand zero = newOperand(OP_CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }
    // Exp -> NOT Exp

    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->child[0]->name, "ID") && node->child[1]) {
        pOperand funcTemp =
            newOperand(OP_FUNCTION, copyString(node->child[0]->val.msg));
        // Exp -> ID LP Args RP
        if (!strcmp(node->child[2]->name, "Args")) {
            pArgList argList = newArgList();
            translateArgs(node->child[2], argList);
            if (!strcmp(node->child[0]->val.msg, "write")) {
                genInterCode(IR_WRITE, argList->head->op);
            } else {
                pArg argTemp = argList->head;
                while (argTemp) {
                    genInterCode(IR_ARG, argTemp->op);
                    argTemp = argTemp->next;
                }
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
        // Exp -> ID LP RP
        else {
            if (!strcmp(node->child[0]->val.msg, "read")) {
                genInterCode(IR_READ, place);
            } else {
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
    }
    // Exp -> ID
    else if (!strcmp(node->child[0]->name, "ID")) {
        interCodeList->tempVarNum--;
        // 非结构体参数情况都当做变量处理
        setOperand(place, OP_VARIABLE, (void*)copyString(node->child[0]->val.msg));
    }
    // Exp -> INT（无浮点数情况）
    else {
        interCodeList->tempVarNum--;
        setOperand(place, OP_CONSTANT, (void*)node->child[0]->val.val_i);
    }
}

void translateCond(pNode node, pOperand labelTrue, pOperand labelFalse) {
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | NOT Exp

    // Exp -> NOT Exp
    if (!strcmp(node->child[0]->name, "NOT")) {
        translateCond(node->child[1], labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
    else if (!strcmp(node->child[1]->name, "RELOP")) {
        pOperand t1 = newTemp();
        pOperand t2 = newTemp();
        translateExp(node->child[0], t1);
        translateExp(node->child[2], t2);

        pOperand relop =
            newOperand(OP_RELOP, copyString(node->child[1]->val.msg));

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
    // Exp -> Exp AND Exp
    else if (!strcmp(node->child[1]->name, "AND")) {
        pOperand label1 = newLabel();
        translateCond(node->child[0], label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        translateCond(node->child[2], labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->child[1]->name, "OR")) {
        pOperand label1 = newLabel();
        translateCond(node->child[0], labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        translateCond(node->child[2], labelTrue, labelFalse);
    }
    // other cases
    else {
        pOperand t1 = newTemp();
        translateExp(node, t1);
        pOperand t2 = newOperand(OP_CONSTANT, 0);
        pOperand relop = newOperand(OP_RELOP, copyString("!="));

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
}

void translateArgs(pNode node, pArgList argList) {
    assert(node != NULL);
    assert(argList != NULL);
    if (interError) return;
    // Args -> Exp COMMA Args
    //       | Exp

    // Args -> Exp
    pArg temp = newArg(newTemp());
    translateExp(node->child[0], temp->op);

    if (temp->op->kind == OP_VARIABLE) {
        pItem item = searchTableItem(table, temp->op->u.name);
        if (item && item->field->type->kind == ARRAY) {
            interError = true;
            printf(
                "Cannot translate: Code containsvariables of "
                "multi-dimensional array type or parameters of array "
                "type.\n");
            return;
        }
    }
    addArg(argList, temp);

    // Args -> Exp COMMA Args
    if (node->child[1] != NULL) {
        translateArgs(node->child[2], argList);
    }
}

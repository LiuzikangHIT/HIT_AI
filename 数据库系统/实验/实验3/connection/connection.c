#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "extmem.h"


#define BLOCK_SIZE 64
#define BUFFER_SIZE 520
#define NUM_R_TUPLES 112
#define NUM_S_TUPLES 224
#define HASH_TABLE_SIZE 16
#define RAND_SEED 23

typedef struct {
    int A;
    int B;
} Tuple_R;

typedef struct {
    int C;
    int D;
} Tuple_S;

typedef struct {
    int A;
    int B;
    int C;
    int D;
} Tuple_RS;

typedef struct HashBucket{
    int A;
    int B;
    struct HashBucket *next;    // 链表用于处理冲突
} HashBucket;

void generateRelation_R(unsigned int startAddr, Buffer *buf)
{
    Tuple_R R[NUM_R_TUPLES];
    unsigned int currentAddr = startAddr;
    int tupleIdx = 0;

    // 随机生成关系R的元组
    for (int i = 0; i < NUM_R_TUPLES; i++) {
        R[i].A = rand() % 40 + 1;   // A的值域为[1, 40]
        R[i].B = rand() % 1000 + 1; // B的值域为[1, 1000]
    }

    while (tupleIdx < NUM_R_TUPLES) {
        unsigned char *blkPtr = getNewBlockInBuffer(buf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx < NUM_R_TUPLES; i++, tupleIdx++) {
            *(Tuple_R*)(blkPtr + i * 8) = R[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < NUM_R_TUPLES) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 7 * 8) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, buf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }
}

void generateRelation_S(unsigned int startAddr, Buffer *buf)
{
    Tuple_S S[NUM_S_TUPLES];
    unsigned int currentAddr = startAddr;
    int tupleIdx = 0;

    for (int i = 0; i < NUM_S_TUPLES; i++) {
        S[i].C = rand() % 41 + 20;  // C的值域为[20, 60], 41=60-20+1
        S[i].D = rand() % 1000 + 1; // D的值域为[1, 1000]
    }

    while (tupleIdx < NUM_S_TUPLES) {
        unsigned char *blkPtr = getNewBlockInBuffer(buf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx < NUM_S_TUPLES; i++, tupleIdx++) {
            *(Tuple_S*)(blkPtr + i * 8) = S[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < NUM_S_TUPLES) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 7 * 8) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, buf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }
}

/* 关系选择算法 */
void selectRelation(unsigned int startAddr_R, unsigned int startAddr_S, unsigned int storeAddr_R, unsigned int storeAddr_S, Buffer *buf)
{
    Tuple_R selected_R[NUM_R_TUPLES];
    Tuple_S selected_S[NUM_S_TUPLES];
    int count_R = 0, count_S = 0, tupleIdx_R = 0, tupleIdx_S = 0;
    Buffer resultBuf;
    unsigned int currentAddr_R = storeAddr_R, currentAddr_S = storeAddr_S;

    if (!initBuffer(BUFFER_SIZE, BLOCK_SIZE, &resultBuf)) {
        perror("Fail to initialize buffer.\n");
    }

    // 选择 R.A=40 的元组
    for (int addr = startAddr_R; addr < startAddr_R + 16; addr++) {
        unsigned char *blkPtr = readBlockFromDisk(addr, buf);
        if (blkPtr == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_R < NUM_R_TUPLES; i++, tupleIdx_R++) {
            Tuple_R *tuple = (Tuple_R*)(blkPtr + i * 8);
            if(tuple->A == 40) {
                printf("Find a tuple of A:%d and B:%d.\n", tuple->A, tuple->B);
                selected_R[count_R++] = *tuple;
            }
        }

        freeBlockInBuffer(blkPtr, buf);
    }

    // 将结果写入磁盘
    tupleIdx_R = 0;
    while (tupleIdx_R < count_R) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_R < count_R; i++, tupleIdx_R++) {
            *(Tuple_R*)(blkPtr + i * 8) = selected_R[tupleIdx_R];

        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx_R < count_R) ? currentAddr_R + 1 : 0;
        *(unsigned int*)(blkPtr + 7 * 8) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr_R, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr_R++;
    }

    // 选择 S.C=60 的元组
    for (int addr = startAddr_S; addr < startAddr_S + 32; addr++) {
        unsigned char *blkPtr = readBlockFromDisk(addr, buf);
        if (blkPtr == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }
        
        for (int i = 0; i < 7 && tupleIdx_S < NUM_S_TUPLES; i++, tupleIdx_S++) {
            Tuple_S *tuple = (Tuple_S*)(blkPtr + i * 8);
            if(tuple->C == 60) {
                printf("Find a tuple of C:%d and D:%d.\n", tuple->C, tuple->D);
                selected_S[count_S++] = *tuple;
            }
        }

        freeBlockInBuffer(blkPtr, buf);
    }

    // 将结果写入磁盘
    tupleIdx_S = 0;
    while (tupleIdx_S < count_S) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_S < count_S; i++, tupleIdx_S++) {
            *(Tuple_S*)(blkPtr + i * 8) = selected_S[tupleIdx_S];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx_S < count_S) ? currentAddr_S + 1 : 0;
        *(unsigned int*)(blkPtr + 7 * 8) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr_S, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr_S++;
    }

    // 释放缓冲区
    freeBuffer(&resultBuf);
}

/* 关系投影算法 */
void projectAFromRelation_R(unsigned int startAddr, unsigned int storeAddr, Buffer *buf)
{
    int projected_A[NUM_R_TUPLES];
    int count = 0, tupleIdx = 0;
    Buffer resultBuf;
    unsigned int currentAddr = storeAddr;

    if (!initBuffer(BUFFER_SIZE, BLOCK_SIZE, &resultBuf)) {
        perror("Fail to initialize buffer.\n");
    }

    // 对关系R的A属性进行投影
    for (int addr = startAddr; addr < startAddr + 16; addr++) {
        unsigned char *blkPtr = readBlockFromDisk(addr, buf);
        if (blkPtr == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx < NUM_R_TUPLES; i++, tupleIdx++) {
            Tuple_R *tuple = (Tuple_R*)(blkPtr + i * 8);
            projected_A[count++] = tuple->A;
            printf("Projection A of R: %d\n", tuple->A);
        }

        freeBlockInBuffer(blkPtr, buf);
    }

    // 将结果写入磁盘
    tupleIdx = 0;
    while (tupleIdx < NUM_R_TUPLES) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 14 && tupleIdx < NUM_R_TUPLES; i++, tupleIdx++) {
            *(int*)(blkPtr + i * 8) = projected_A[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < NUM_R_TUPLES) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 7 * 8) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }

    // 释放缓冲区
    freeBuffer(&resultBuf);
}

// 嵌套循环连接算法
void nestedLoopJoin(unsigned int startAddr_R, unsigned int startAddr_S, unsigned int storeAddr, Buffer *buf)
{
    Tuple_RS RS[NUM_R_TUPLES * NUM_S_TUPLES];
    int count = 0, tupleIdx = 0, tupleIdx_R = 0, tupleIdx_S = 0;
    Buffer resultBuf;
    unsigned int currentAddr = storeAddr;

    if (!initBuffer(BUFFER_SIZE, BLOCK_SIZE, &resultBuf)) {
        perror("Fail to initialize buffer.\n");
    }
    
    // 对关系R和S计算R.A连接S.C
    for (int addr_S = startAddr_S; addr_S < startAddr_S + 32; addr_S++) {
        unsigned char *blkPtr_S = readBlockFromDisk(addr_S, buf);
        if (blkPtr_S == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int addr_R = startAddr_R; addr_R < startAddr_R + 16; addr_R++) {
            unsigned char *blkPtr_R = readBlockFromDisk(addr_R, buf);
            if (blkPtr_R == NULL) {
                perror("Fail to read block from disk.\n");
                return;
            }

            int flag = -1;
            for (int i = 0; i < 7; i++) {
                Tuple_S *tuple_S = (Tuple_S*)(blkPtr_S + i * 8);
                for (int j = 0; j < 7; j++) {
                    Tuple_R *tuple_R = (Tuple_R*)(blkPtr_R + j * 8);
                    if (tuple_R->A == tuple_S->C) {
                        RS[count].A = tuple_R->A;
                        RS[count].B = tuple_R->B;
                        RS[count].C = tuple_S->C;
                        RS[count++].D = tuple_S->D;
                        printf("Join R: A=%d, B=%d and S: C=%d, D=%d.\n", tuple_R->A, tuple_R->B, tuple_S->C, tuple_S->D);
                    }
                }
            }

            freeBlockInBuffer(blkPtr_R, buf);
        }

        freeBlockInBuffer(blkPtr_S, buf);
    }

    // 将结果写入磁盘
    while (tupleIdx < count) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 3 && tupleIdx < count; i++, tupleIdx++) {
            *(Tuple_RS*)(blkPtr + i * 16) = RS[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < count) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 3 * 16) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }

    // 释放缓冲区
    freeBuffer(&resultBuf);
}

// 哈希连接算法
void hashJoin(unsigned int startAddr_R, unsigned int startAddr_S, unsigned int storeAddr, Buffer *buf)
{
    Tuple_RS RS[NUM_R_TUPLES * NUM_S_TUPLES];
    int count = 0, tupleIdx = 0, tupleIdx_R = 0, tupleIdx_S = 0;
    Buffer resultBuf;
    unsigned int currentAddr = storeAddr;

    if (!initBuffer(BUFFER_SIZE, BLOCK_SIZE, &resultBuf)) {
        perror("Fail to initialize buffer.\n");
    }

    // 对关系R建立哈希表
    HashBucket *hashTable[HASH_TABLE_SIZE] = {0};
    for (int addr_R = startAddr_R; addr_R < startAddr_R + 16; addr_R++) {
        unsigned char *blkPtr_R = readBlockFromDisk(addr_R, buf);
        if (blkPtr_R == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_R < NUM_R_TUPLES; i++, tupleIdx_R++) {
            Tuple_R *tuple_R = (Tuple_R*)(blkPtr_R + i * 8);
            int hashIdx = tuple_R->A % HASH_TABLE_SIZE;   // 哈希函数，简单取模
            HashBucket *newBucket = (HashBucket*)malloc(sizeof(HashBucket));;
            newBucket->A = tuple_R->A;
            newBucket->B = tuple_R->B;
            newBucket->next = hashTable[hashIdx];
            hashTable[hashIdx] = newBucket;   // 插入到链表头
        }

        freeBlockInBuffer(blkPtr_R, buf);
    }

    // 探测阶段
    for (int addr_S = startAddr_S; addr_S < startAddr_S + 32; addr_S++) {
        unsigned char *blkPtr_S = readBlockFromDisk(addr_S, buf);
        if (blkPtr_S == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_S < NUM_S_TUPLES; i++, tupleIdx_S++) {
            Tuple_S *tuple_S = (Tuple_S*)(blkPtr_S + i * 8);
            int hashIdx = tuple_S->C % HASH_TABLE_SIZE;   // 哈希函数，简单取模

            // 查找匹配的桶
            HashBucket *bucket = hashTable[hashIdx];
            while (bucket != NULL) {
                if (bucket->A == tuple_S->C) {
                    RS[count].A = bucket->A;
                    RS[count].B = bucket->B;
                    RS[count].C = tuple_S->C;
                    RS[count++].D = tuple_S->D;
                    printf("Join R: A=%d, B=%d and S: C=%d, D=%d.\n", bucket->A, bucket->B, tuple_S->C, tuple_S->D);
                }
                bucket = bucket->next;
            }
        }

        freeBlockInBuffer(blkPtr_S, buf);
    }

    // 将结果写入磁盘
    while (tupleIdx < count) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 3 && tupleIdx < count; i++, tupleIdx++) {
            *(Tuple_RS*)(blkPtr + i * 16) = RS[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < count) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 3 * 16) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }

    // 释放缓冲区
    freeBuffer(&resultBuf);
}

// 对Tuple_R按R.A排序
int compareTuple_R(const void *a, const void *b)
{
    return ((Tuple_R*)a)->A - ((Tuple_R*)b)->A;
}

// 对Tuple_S按S.C排序
int compareTuple_S(const void *a, const void *b)
{
    return ((Tuple_S*)a)->C - ((Tuple_S*)b)->C;
}

// 排序归并连接算法
void sortMergeJoin(unsigned int startAddr_R, unsigned int startAddr_S, unsigned int storeAddr, Buffer *buf)
{
    Tuple_RS RS[NUM_R_TUPLES * NUM_S_TUPLES];
    int count = 0, tupleIdx = 0, tupleIdx_R = 0, tupleIdx_S = 0;
    Buffer resultBuf;
    unsigned int currentAddr = storeAddr;

    if (!initBuffer(BUFFER_SIZE, BLOCK_SIZE, &resultBuf)) {
        perror("Fail to initialize buffer.\n");
        return;
    }
    
    // 对关系R按R.A排序
    Tuple_R sortedR[NUM_R_TUPLES];
    int count_R = 0;
    for (int addr_R = startAddr_R; addr_R < startAddr_R + 16; addr_R++) {
        unsigned char *blkPtr_R = readBlockFromDisk(addr_R, buf);
        if (blkPtr_R == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0;  i< 7 && tupleIdx_R < NUM_R_TUPLES; i++, tupleIdx_R++) {
            sortedR[count_R++] = *(Tuple_R*)(blkPtr_R + i * 8);
        }

        freeBlockInBuffer(blkPtr_R, buf);
    }

    // 对关系S按S.C排序
    Tuple_S sortedS[NUM_S_TUPLES];
    int count_S = 0;
    for (int addr_S = startAddr_S; addr_S < startAddr_S + 32; addr_S++) {
        unsigned char *blkPtr_S = readBlockFromDisk(addr_S, buf);
        if (blkPtr_S == NULL) {
            perror("Fail to read block from disk.\n");
            return;
        }

        for (int i = 0; i < 7 && tupleIdx_S < NUM_S_TUPLES; i++, tupleIdx_S++) {
            sortedS[count_S++] = *(Tuple_S*)(blkPtr_S + i * 8);
        }

        freeBlockInBuffer(blkPtr_S, buf);
    }

    // 排序
    qsort(sortedR, count_R, sizeof(Tuple_R), compareTuple_R);
    qsort(sortedS, count_S, sizeof(Tuple_S), compareTuple_S);

    // 执行排序归并连接
    int i = 0, j = 0;
    while (i < count_R && j < count_S) {
        if (sortedR[i].A < sortedS[j].C) {
            i++;
        } else if (sortedR[i].A > sortedS[j].C) {
            j++;
        } else {  // sortedR[i].A == sortedS[j].C
            // 遍历所有匹配的R元组
            int tempIdx_R = i, tempIdx_S = j;
            while (tempIdx_R < count_R && sortedR[tempIdx_R].A == sortedR[i].A) {
                // 遍历所有匹配的S元组
                tempIdx_S = j;
                while (tempIdx_S < count_S && sortedS[tempIdx_S].C == sortedS[j].C) {
                    RS[count].A = sortedR[tempIdx_R].A;
                    RS[count].B = sortedR[tempIdx_R].B;
                    RS[count].C = sortedS[tempIdx_S].C;
                    RS[count++].D = sortedS[tempIdx_S].D;
                    printf("Join R: A=%d, B=%d and S: C=%d, D=%d.\n", sortedR[tempIdx_R].A, sortedR[tempIdx_R].B, sortedS[tempIdx_S].C, sortedS[tempIdx_S].D);
                    tempIdx_S++;
                }
                tempIdx_R++;
            }
            i = tempIdx_R;
            j = tempIdx_S;
        }
    }

    // 将结果写入磁盘
    while (tupleIdx < count) {
        unsigned char *blkPtr = getNewBlockInBuffer(&resultBuf);
        if (blkPtr == NULL) {
            perror("Fail to get a new block in buffer.\n");
            return;
        }

        for (int i = 0; i < 3 && tupleIdx < count; i++, tupleIdx++) {
            *(Tuple_RS*)(blkPtr + i * 16) = RS[tupleIdx];
        }
        // 下一磁盘块地址
        unsigned int nextBlockAddr = (tupleIdx < count) ? currentAddr + 1 : 0;
        *(unsigned int*)(blkPtr + 3 * 16) = nextBlockAddr;
        
        // 将数据块写入磁盘
        if (writeBlockToDisk(blkPtr, currentAddr, &resultBuf) != 0) {
            perror("Fail to write block to disk.\n");
            return;
        }
        currentAddr++;
    }

    // 释放缓冲区
    freeBuffer(&resultBuf);
}

int main(int argc, char **argv)
{
    srand(time(NULL)); // 初始化随机数生成器

    Buffer buf; /* A buffer */
    initBuffer(BUFFER_SIZE, BLOCK_SIZE, &buf);
    
    /* 数据准备 */
    generateRelation_R(1, &buf);
    generateRelation_S(17, &buf);

    /* 关系选择&投影算法 */
     printf("\n===============     selectRelation     ===============\n");
     selectRelation(1, 17, 50, 100, &buf);
     printf("\n=============== projectAFromRelation_R ===============\n");
     projectAFromRelation_R(1, 150, &buf);

    /* 关系连接算法 */
    printf("\n===============     nestedLoopJoin     ===============\n");
    nestedLoopJoin(1, 17, 200, &buf);
    printf("\n===============        hashJoin        ===============\n");
    hashJoin(1, 17, 400, &buf);
    printf("\n===============      sortMergeJoin     ===============\n");
    sortMergeJoin(1, 17, 600, &buf);


    // 释放缓冲区
    freeBuffer(&buf);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_TIMES 400
#define rows 8192
#define cols 5120

long img[rows][cols];

void initialize(long img[rows][cols]);          // 初始化图像
void smooth(long img[rows][cols]);              // 平滑算法（未优化）
void simplify_optimize(long img[rows][cols]);   // 一般有用的优化——简化复杂指令
void pointer_optimize(long img[rows][cols]);    // 一般有用的优化——指针记录上下行
void cpu_optimize(long img[rows][cols]);        // 面向CPU的优化——循环展开
void cache_optimize(long img[rows][cols]);      // 面向Cache的优化——分块

int main()
{
    clock_t start, finish;
    double Total_time;

    srand((unsigned int)time(NULL));

    // 初始化图像
    initialize(img);

    // 平滑算法 循环MAX_TIMES次
    start = clock();
    for(int k = 0; k < MAX_TIMES; k++) {
        // 选择所用优化
        // smooth(img);
        // simplify_optimize(img);
        // pointer_optimize(img);
        // cpu_optimize(img);
        cache_optimize(img);
    }
    finish = clock();

    Total_time = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("所用时间：%f秒\n", Total_time);
    
    return 0;
}

// 初始化图像
void initialize(long img[rows][cols])
{
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            img[i][j] = rand() % 256;
        }
    }
}

// 平滑算法
void smooth(long img[rows][cols])
{
    // 计算周围四个点的灰度均值
    for(int i = 1; i < rows-1; i++) {
        for(int j = 1; j < cols-1; j++) {
            img[i][j] = (img[i-1][j] + img[i+1][j] + img[i][j-1] + img[i][j+1]) / 4;
        }
    }
}

// 一般有用的优化——简化复杂指令
void simplify_optimize(long img[rows][cols])
{
    for(int i = 1; i < rows-1; i++) {
        for(int j = 1; j < cols-1; j++) {
            img[i][j] = (img[i-1][j] + img[i+1][j] + img[i][j-1] + img[i][j+1]) >> 2; // 除法变为移位操作
        }
    }
}

// 一般有用的优化——指针记录上下行
void pointer_optimize(long img[rows][cols])
{
    for(int i = 1; i < rows-1; i++) {
        long *up = img[i-1], *down = img[i+1];  // 上下行
        for(int j = 1; j < cols-1; j++) {
            img[i][j] = (up[j] + down[j] + img[i][j-1] + img[i][j+1]) / 4;
        }
    }
}

// 面向CPU的优化——循环展开
void cpu_optimize(long img[rows][cols])
{
    long bef1, aft1, bef2, aft2;
    for(int i = 1; i < rows-1; i++) {
        int j;
        for(j = 1; j < cols-8; j += 8)
        {
            bef1 = img[i][j+1];
            aft1 = img[i][j+2];
            bef2 = img[i][j+5];
            aft2 = img[i][j+6];
            img[i][j] = (img[i-1][j] + img[i+1][j] + img[i][j-1] + bef1) / 4;
            img[i][j+1] = (img[i-1][j+1] + img[i+1][j+1] + img[i][j] + aft1) / 4;
            img[i][j+2] = (img[i-1][j+2] + img[i+1][j+2] + bef1 + img[i][j+3]) / 4;
            img[i][j+3] = (img[i-1][j+3] + img[i+1][j+3] + aft1 + img[i][j+4]) / 4;
            img[i][j+4] = (img[i-1][j+4] + img[i+1][j+4] + img[i][j+3] + bef2) / 4;
            img[i][j+5] = (img[i-1][j+5] + img[i+1][j+5] + img[i][j+4] + aft2) / 4;
            img[i][j+6] = (img[i-1][j+6] + img[i+1][j+6] + bef2 + img[i][j+7]) / 4;
            img[i][j+7] = (img[i-1][j+7] + img[i+1][j+7] + aft2 + img[i][j+8]) / 4;
        }
        for(; j < cols-1; j++) {
            img[i][j] = (img[i-1][j] + img[i+1][j] + img[i][j-1] + img[i][j+1]) / 4;
        }
    }
}

// 面向Cache的优化——分块
void cache_optimize(long img[rows][cols])
{
    int bsize = 8;  // 块的大小
    for(int ii = 1; ii < rows-1; ii+=bsize) {
        int end = ii+bsize < rows-1 ? ii+bsize : rows-1;
        for(int i = ii; i < end; i++) {
            for(int j = 1; j < cols-1; j++) {
                img[i][j] = (img[i-1][j] + img[i+1][j] + img[i][j-1] + img[i][j+1]) / 4;
            }
        }
    }
}


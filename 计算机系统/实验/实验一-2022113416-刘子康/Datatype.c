#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student
{
    int id;
    char name[9];
};

union Data
{
    int i;
    float f;
    char str[9];
};

enum DAY
{
    MON=1, TUE, WED, THU, FRI, SAT, SUN
};

int i = 2022113416;
float f = 131025200405104814;
double d = 131025200405104814;
char str[9] = {"liuzikang"};
char c = 'a';
int *p = &i;
struct student stu = {2022113416, "liuzikang"};
union Data u;
enum DAY day = TUE;

void show_bytes(unsigned char *start, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        printf("%.2x ", start[i]);
    }
    printf("\n");
}  


int main()
{
    printf("变量名\t\t内容\t\t\t\t地址\t\t\t内存字节\n");
    printf("i(int)\t\t%d\t\t\t%p\t", i, &i);                    // 整型
    show_bytes((unsigned char*)&i, sizeof(i));
    printf("f(float)\t%f\t%p\t", f, &f);                        // 单精度浮点型
    show_bytes((unsigned char*)&f, sizeof(f));
    printf("d(double)\t%lf\t%p\t", d, &d);                      // 双精度浮点型
    show_bytes((unsigned char*)&d, sizeof(d));
    printf("c(char)\t\t%c\t\t\t\t%p\t", c, &c);                 // 字符型
    show_bytes((unsigned char*)&c, sizeof(c));
    printf("str(string)\t%s\t\t\t%p\t", str, &str);             // 字符串
    show_bytes((unsigned char*)&str, sizeof(str));
    printf("p(pointer)\t%d\t\t\t\t%p\t", p, &p);                // 指针
    show_bytes((unsigned char*)&p, sizeof(p));
    printf("stu(struct)\t\t\t\t\t%p\n", &stu);                  // 结构体
    printf("stu.id\t\t%d\t\t\t%p\t", stu.id, &stu.id);
    show_bytes((unsigned char*)&stu.id, sizeof(stu.id));
    printf("stu.name\t%s\t\t\t%p\t", stu.name, &stu.name);
    show_bytes((unsigned char*)&stu.name, sizeof(stu.name));
    printf("u(union)\t\t\t\t\t%p\n", &u);                       // 共用体
    u.i = 2022113416;
    printf("u.i\t\t%d\t\t\t%p\t", u.i, &u.i);
    show_bytes((unsigned char*)&u.i, sizeof(u.i));
    u.f = 131025200405104814;
    printf("u.f\t\t%f\t%p\t", u.f, &u.f);
    show_bytes((unsigned char*)&u.f, sizeof(u.f));
    strcpy(u.str, "liuzikang");
    printf("u.str\t\t%s\t\t\t%p\t", u.str, &u.str);
    show_bytes((unsigned char*)&u.str, sizeof(u.str));
    printf("day(enum)\t%d\t\t\t\t%p\t", day, &day);             // 枚举类型
    show_bytes((unsigned char*)&day, sizeof(day));
    printf("main函数\t\t\t\t\t%p\n", &main);                         // main函数
    printf("printf函数\t\t\t\t\t%p\n", &printf);                     // printf函数

    return 0;
}
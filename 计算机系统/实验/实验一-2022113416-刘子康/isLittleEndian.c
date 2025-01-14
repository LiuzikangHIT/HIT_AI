#include <stdio.h>

int isLittleEndian()
{
    int a = 1;
    return *(char*)&a;
}

int main()
{
    if(isLittleEndian() == 1){
        printf("小端\n");
    }
    else{
        printf("大端\n");
    }
    return 0;
}
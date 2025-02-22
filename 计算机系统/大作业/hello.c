// 大作业的 hello.c 程序
// gcc -m64 -Og -no-pie -fno-stack-protector -fno-PIC hello.c -o hello
// 程序运行过程中可以按键盘，如不停乱按，包括回车，Ctrl-Z，Ctrl-C等。
// 可以 运行 ps  jobs  pstree fg 等命令
// 秒数=手机号%5

#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>

int main(int argc,char *argv[]){
	int i;

	if(argc!=5){
		printf("用法: Hello 学号 姓名 手机号 秒数！\n");
		exit(1);
	}
	for(i=0;i<10;i++){
		printf("Hello %s %s %s\n",argv[1],argv[2],argv[3]);
		sleep(atoi(argv[4]));
	}
	getchar();
	return 0;
}


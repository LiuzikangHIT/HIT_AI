/*
 *  linux/lib/iam.c
 *
 *  (C) 2024  Liu Zikang
 */
 
#define __LIBRARY__
#include <unistd.h>

_syscall2(int, whoami, char*, name, unsigned int, size);

int main(int argc, char **argv)
{
	char output[24];
	whoami(output, 24);
	printf("%s\n", output);
	return 0;
}


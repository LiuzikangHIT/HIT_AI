/*
 *  linux/lib/iam.c
 *
 *  (C) 2024  Liu Zikang
 */
 
#define __LIBRARY__
#include <unistd.h>

_syscall1(int, iam, const char*, name);

int main(int argc, char **argv)
{
	iam(argv[1]);
	return 0;
}


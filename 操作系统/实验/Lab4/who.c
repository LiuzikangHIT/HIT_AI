/*
 *  linux/kernel/who.c
 *
 *  (C) 2024  Liu Zikang
 */

#include <errno.h>
#include <string.h>
#include <asm/segment.h>


char msg[24];

int sys_iam(const char * name)
{
	char tmp[25] = "";
	int len = 0;
	
	do {
		tmp[len] = get_fs_byte(&name[len]);
	} while(tmp[len++] != '\0' && len < 25);
	
	if (len > 24) {
		return -(EINVAL);
	}

	strcpy(msg, tmp);
	return len - 1;
}

int sys_whoami(char* name, unsigned int size)
{
	int len = strlen(msg), i = 0;
	
	if (size < len) {
		return -(EINVAL);
	}
	
	for (; i < len; i++) {
		put_fs_byte(msg[i], name+i);
	}

	return len;
}


	使用gcc编译连接源代码connection.c和头文件代码extmem.c：gcc -o connection connection.c extmem.c，并使用./connection运行程序，结果存放于同目录下的disk文件夹中。

	1-48为生成的关系R和S，50-99为R.A=40的元组，100-149为S.C=60的元组，150-200为关系R的A属性的投影，200-399为嵌套循环连接后的元组，400-599为哈希连接后的元组，600-799为排序归并连接后的元组。
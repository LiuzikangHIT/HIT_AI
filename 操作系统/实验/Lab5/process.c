#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>

#define HZ 100	/* 每秒钟的时钟滴答次数 */

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char * argv[])
{
	pid_t pid_1, pid_2, pid_3, pid_4;
	int exit_pid;

	printf("parent pid = [%d]\n", getpid());    /* 获取父进程pid */

	/* 以CPU为主要任务 */
	pid_1 = fork();
	if (pid_1 == 0) {
		printf("[%d] is running now.\n", getpid());
		cpuio_bound(10, 1, 0);
		exit(0); /* 终止进程，返回状态码0 */
	}

	/* 以I/O为主要任务 */
	pid_2 = fork();
	if (pid_2 == 0) {
		printf("[%d] is running now.\n", getpid());
		cpuio_bound(10, 0, 1);
		exit(0); /* 终止进程，返回状态码0 */
	}

	/* CPU和I/O各1秒钟轮回 */
	pid_3 = fork();
	if (pid_3 == 0) {
		printf("[%d] is running now.\n", getpid());
		cpuio_bound(10, 1, 1);
		exit(0); /* 终止进程，返回状态码0 */
	}

	/* 较多的I/O+较少的CPU */
	pid_4 = fork();
	if (pid_4 == 0) {
		printf("[%d] is running now.\n", getpid());
		cpuio_bound(10, 1, 9);
		exit(0); /* 终止进程，返回状态码0 */
	}

	exit_pid = wait(NULL);
	printf("[%d] have exited.\n", exit_pid);

	exit_pid = wait(NULL);
	printf("[%d] have exited.\n", exit_pid);

	exit_pid = wait(NULL);
	printf("[%d] have exited.\n", exit_pid);

	exit_pid = wait(NULL);
	printf("[%d] have exited.\n", exit_pid);

	printf("the program was executed successfully.\n");
	return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		/* CPU Burst */
		times(&start_time);
		/* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
		 * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
		 * 加上很合理。*/
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		/* IO Burst */
		/* 用sleep(1)模拟1秒钟的I/O操作 */
		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}


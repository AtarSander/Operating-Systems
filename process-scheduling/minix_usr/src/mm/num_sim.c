#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int range, i, new_queue = 5;
	long a;
	message m;
	printf("Numerical procces pid: %d\n", getpid());
	if (argc == 2)
		range = atoi(argv[1]);
	else if (argc == 3)
	{
		range = atoi(argv[1]);
		new_queue = atoi(argv[2]);
	}
	else
		return 0;
	m.m1_i1 = getpid();
	_syscall(MM, GETPNR, &m);
	m.m1_i2 = new_queue;
	_syscall(MM, MOVEUSERQUEUE, &m);
	for (i = 0; i < range; i++)
		a = i * i;
	printf("Num proc finished. Class: %d, PID: %d \n", new_queue, getpid());
	return 0;
}

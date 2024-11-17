#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int option, value;
	message m;
	if (argc == 2)
	{
		option = atoi(argv[1]);
		value = atoi(argv[2]);
	}	
	else
		return -1;
	m.m1_i1 = option;
	m.m1_i2 = value;
	_syscall(MM, SETSCHEDPARAMS, &m);
	if (option == 1)
		printf("Set RUN_LIMIT1 to be %d", value);
	else if (option == 2)
		printf("Set WAIT_LIMIT2 to be %d", value);
	else if (option == 3)
		printf("Set WAIT_LIMIT3 to be %d", value);
	else
		printf("Set QUEUE2_RATE to be %d", value);
	return 0;
}

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int proc_nr, value;
	message m;
	if (argc == 3)
	{
		proc_nr = atoi(argv[1]);
		value = atoi(argv[2]);
	}	
	else
		return -1;
	m.m1_i1 = proc_nr;
	m.m1_i2 = value;
	_syscall(MM, MOVEUSERQUEUE, &m);
	if (value == 3)
		printf("Process nr %d moved to queue IO", proc_nr, value);
	if (value == 4)
		printf("Process nr %d moved to queue INT", proc_nr, value);
	if (value == 5)
		printf("Process nr %d moved to queue NUM", proc_nr, value);
	return 0;
}

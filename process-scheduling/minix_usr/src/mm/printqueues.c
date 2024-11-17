#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

int main(argc, argv)
{
	message m;
	int option;
	m.m1_i1 = getpid();
	_syscall(MM, GETPNR, &m);
	m.m1_i2 = 3;
	_syscall(MM, MOVEUSERQUEUE, &m);
	_syscall(MM, PRINTQUEUE, &m);
	
	return 0;
}

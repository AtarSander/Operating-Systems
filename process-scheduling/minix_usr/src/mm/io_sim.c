#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int range, i, new_queue = 3;
	message m;
	char a[10];
	FILE *fptr;
	fptr = fopen("io_text.txt", "w");
	printf("I/O procces pid: %d\n", getpid());
	if (argc == 2)
		range = atoi(argv[1]);
	else if (argc == 3)
	{
		range = atoi(argv[1]);
		new_queue = atoi(argv[2]);
		m.m1_i1 = getpid();
		_syscall(MM, GETPNR, &m);
		m.m1_i2 = new_queue;
		_syscall(MM, MOVEUSERQUEUE, &m);
	}
	else
		return 0;
	if(fptr == NULL)
	{
	      printf("Error!");   
	      exit(1);
	}
	for (i = 0; i < range; i++)
	{
		fprintf(fptr, "%d\n", i);
	}
	fclose(fptr);
	fptr = fopen("io_text.txt", "r");
	while (fscanf(fptr, "%s", a) == 1) 
	{
         ;
    	}
	fclose(fptr);
	printf("I/O proc finished. Class: %d, PID: %d \n", new_queue, getpid());		
	return 0;
}

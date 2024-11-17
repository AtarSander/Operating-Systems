#ifndef DISK_H
#define DISK_H
#include <stdio.h>
int createDisk(int size);
int deleteDisk(void); 
int deleteFile(char *fileName);
int copyFromSystemToDisk(char *fileName);
int printCatalog(void); 
int printDiskStats(void); 
int copyFromDiskToSystem(char *fileName);
int printTotalSize(void); 
#endif

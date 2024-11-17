#ifndef INODE_H
#define INODE_H
#include "const.h"
#include <stdio.h>

struct iNode
{
    char name[32];
    int size;
    int addresses[5];
};

struct iNodeTable
{
    struct iNode table[MAX_FILE_NUM];
};

void saveInode(FILE *disk, struct iNode newInode);
int isFileNameUsed(FILE *disk, char *fileName);
void readInode(FILE *disk, struct iNodeTable *returnInode);
void dropInode(FILE *disk, int index);
#endif

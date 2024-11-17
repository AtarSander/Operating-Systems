#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include "const.h"
struct Block
{
    char data[BLOCK_SIZE];
};

void saveData(FILE *disk, FILE *sourceFile, struct iNode fileInode);
void readData(FILE *disk, FILE *outputFile, struct iNode fileInode);
#endif

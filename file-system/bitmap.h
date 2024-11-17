#ifndef BITMAP_H
#define BITMAP_H
#include "const.h"
#include <stdio.h>
struct FreeBlocksMap
{
    int freeNodes[200];
};

int findFreeBlock(FILE *disk);
void setBlockTaken(FILE *disk, int blockNum);
void setBlockFree(FILE *disk, int blockNum);
void readBitmap(FILE *disk, struct FreeBlocksMap *bitmap);
#endif

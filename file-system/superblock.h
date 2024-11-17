#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H
#include <stdio.h>
struct SuperBlock
{
    int maxFileSize;
    int maxInodeNum;
    int maxBlockNum;
    int totalDiskSize;
    int freeBlocksNum;
    int freeInodeNum;
};

void readSuperBlock(FILE *disk, struct SuperBlock *returnSuperBlock);
void saveSuperBlock(FILE *disk, struct SuperBlock *updatedSuperBlock);
#endif

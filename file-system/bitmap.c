#include "bitmap.h"
#include "superblock.h"
#include <stdio.h>


int findFreeBlock(FILE *disk)
{
    int i;
    struct FreeBlocksMap bitmap;
    struct SuperBlock diskSuperBlock;
    readSuperBlock(disk, &diskSuperBlock);
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fread(&bitmap, sizeof(bitmap), 1, disk);
    for (i = 0; i < diskSuperBlock.maxBlockNum; i++)
    {
        if(bitmap.freeNodes[i] == 0)
            return i;
    }
}

void setBlockTaken(FILE *disk, int blockNum)
{
    struct FreeBlocksMap bitmap;
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fread(&bitmap, sizeof(bitmap), 1, disk);
    bitmap.freeNodes[blockNum] = 1;
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fwrite(&bitmap, sizeof(bitmap), 1, disk);
}

void setBlockFree(FILE *disk, int blockNum)
{
    struct FreeBlocksMap bitmap;
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fread(&bitmap, sizeof(bitmap), 1, disk);
    bitmap.freeNodes[blockNum] = 0;
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fwrite(&bitmap, sizeof(bitmap), 1, disk);
}

void readBitmap(FILE *disk, struct FreeBlocksMap *bitmap)
{
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fread(bitmap, sizeof(struct FreeBlocksMap), 1, disk);
}

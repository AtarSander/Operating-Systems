#include "superblock.h"
#include "inode.h"
#include "bitmap.h"
#include "const.h"
#include <stdio.h>
#include "data.h"


void saveData(FILE *disk, FILE *sourceFile, struct iNode fileInode)
{
    int i = 0;
    struct Block dataBlock;

    while (fileInode.size > BLOCK_SIZE)
    {
        fseek(disk, 3 * BLOCK_SIZE + fileInode.addresses[i] * BLOCK_SIZE, SEEK_SET);
        fseek(sourceFile, i * BLOCK_SIZE, SEEK_SET);
        fread(&dataBlock.data, sizeof(char), BLOCK_SIZE, sourceFile);
        fwrite(&dataBlock.data, sizeof(dataBlock), 1, disk);
        fileInode.size -= BLOCK_SIZE;
        i++;
    }
    if (fileInode.size > 0)
    {
        fseek(disk, 3 * BLOCK_SIZE + fileInode.addresses[i] * BLOCK_SIZE, SEEK_SET);
        fseek(sourceFile, i * BLOCK_SIZE, SEEK_SET);
        fread(&dataBlock.data, sizeof(char), fileInode.size, sourceFile);
        fwrite(&dataBlock.data, sizeof(dataBlock), 1, disk);
    }
}

void readData(FILE *disk, FILE *outputFile, struct iNode fileInode)
{
    int i = 0;
    struct Block dataBlock;
    while (fileInode.size > BLOCK_SIZE)
    {
        fseek(disk, 3 * BLOCK_SIZE + fileInode.addresses[i] * BLOCK_SIZE, SEEK_SET);
        fseek(outputFile, i * BLOCK_SIZE, SEEK_SET);
        fread(&dataBlock.data, sizeof(dataBlock), 1, disk);
        fwrite(&dataBlock.data, sizeof(char), BLOCK_SIZE, outputFile);
        fileInode.size -= BLOCK_SIZE;
        i++;
    }
    if (fileInode.size > 0)
    {
        fseek(disk, 3 * BLOCK_SIZE + fileInode.addresses[i] * BLOCK_SIZE, SEEK_SET);
        fseek(outputFile, i * BLOCK_SIZE, SEEK_SET);
        fread(&dataBlock.data, sizeof(dataBlock), 1, disk);
        fwrite(&dataBlock.data, sizeof(char), fileInode.size, outputFile);
    }
}

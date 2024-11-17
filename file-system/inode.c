#include "inode.h"
#include "superblock.h"
#include "bitmap.h"
#include <stdio.h>
#include <string.h>


void saveInode(FILE *disk, struct iNode newInode)
{
    struct iNodeTable diskInodeTable;
    int i, j;
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fread(&diskInodeTable, sizeof(diskInodeTable), 1, disk);
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        if(diskInodeTable.table[i].size == -1)
            {
                strcpy(diskInodeTable.table[i].name, newInode.name);
                diskInodeTable.table[i].size = newInode.size;
                for (j = 0; j < 5; j++)
                    diskInodeTable.table[i].addresses[j] = newInode.addresses[j];
                break;
            }
    }

    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(&diskInodeTable, sizeof(diskInodeTable), 1, disk);
}

int isFileNameUsed(FILE *disk, char *fileName)
{
    struct iNodeTable diskInodeTable;
    int i;
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fread(&diskInodeTable, sizeof(struct iNodeTable), 1, disk);
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        if(strcmp(fileName, diskInodeTable.table[i].name) == 0)
            return i+1;
    }
    return 0;
}

void readInode(FILE *disk, struct iNodeTable *returnInode)
{
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fread(returnInode, sizeof(struct iNodeTable), 1, disk);
}

void dropInode(FILE *disk, int index)
{
    int j, k;
    struct SuperBlock diskSuperBlock;
    struct iNodeTable diskInodeTable;
    readSuperBlock(disk, &diskSuperBlock);
    readInode(disk, &diskInodeTable);
    index--;
    for (j = 0; j < 32; j++)
        diskInodeTable.table[index].name[j] = -1;
    diskInodeTable.table[index].size = -1;
    for (k = 0; k < 5; k++)
    {
        if (diskInodeTable.table[index].addresses[k] != -1)
            {
                setBlockFree(disk, diskInodeTable.table[index].addresses[k]);
                diskSuperBlock.freeBlocksNum++;
            }
        diskInodeTable.table[index].addresses[k] = -1;
    }
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(&diskInodeTable, sizeof(diskInodeTable), 1, disk);
    diskSuperBlock.freeInodeNum++;
    saveSuperBlock(disk, &diskSuperBlock);
}

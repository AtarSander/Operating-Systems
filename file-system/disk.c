#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "superblock.h"
#include "inode.h"
#include "bitmap.h"
#include "const.h"
#include "data.h"


int createDisk(int size)
{
    FILE *disk;
    int i, j, k;
    struct SuperBlock diskSuperBlock;
    struct FreeBlocksMap diskBitMap;
    struct iNodeTable diskInode;
    struct Block diskBlock;
    if ((disk = fopen("virtualDisk", "w+")) == NULL)
    {
        return 1;
    }
    size *= 1024;
    diskSuperBlock.maxFileSize = MAX_FILE_SIZE;
    diskSuperBlock.maxInodeNum = MAX_FILE_NUM;
    diskSuperBlock.maxBlockNum = ceil(size / BLOCK_SIZE) - 3;
    diskSuperBlock.totalDiskSize = ceil(size / BLOCK_SIZE) * BLOCK_SIZE;
    diskSuperBlock.freeInodeNum = MAX_FILE_NUM;
    diskSuperBlock.freeBlocksNum = ceil(size / BLOCK_SIZE) - 3;
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        for (j = 0; j < 32; j++)
            diskInode.table[i].name[j] = -1;
        diskInode.table[i].size = -1;
        for (k = 0; k < 5; k++)
            diskInode.table[i].addresses[k] = -1;
    }
    for (i = 0; i < diskSuperBlock.maxBlockNum; i++)
    {
        diskBitMap.freeNodes[i] = 0;
    }
    for (i = 0; i < BLOCK_SIZE; i++)
        diskBlock.data[i] = -1;
    saveSuperBlock(disk, &diskSuperBlock);
    fseek(disk, BLOCK_SIZE, SEEK_SET);
    fwrite(&diskInode, sizeof(diskInode), 1, disk);
    fseek(disk, 2 * BLOCK_SIZE, SEEK_SET);
    fwrite(&diskBitMap, sizeof(diskBitMap), 1, disk);
    fseek(disk, 3 * BLOCK_SIZE, SEEK_SET);
    fwrite(&diskBlock, sizeof(diskBlock), diskSuperBlock.maxBlockNum, disk);
    fclose(disk);
    printf("Created disk of size %dB \n", size);
    return 0;
}

int deleteDisk(void)
{
    if (remove("virtualDisk") == 0)
        return 0;
    return 1;
}

int deleteFile(char *fileName)
{
    FILE *disk;
    int deleteIndex;
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    deleteIndex = isFileNameUsed(disk, fileName);
    if(deleteIndex)
    {
        dropInode(disk, deleteIndex);
    }
    else
    {
        printf("File not on disk\n");
        return 1;
    }
    fclose(disk);
    printf("File %s has been deleted\n", fileName);
    return 0;
}

int copyFromSystemToDisk(char *fileName)
{
    FILE *disk, *sourceFile;
    int i;
    struct SuperBlock diskSuperBlock;
    struct iNode fileInode;
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    if((sourceFile = fopen(fileName, "r+")) == NULL)
    {
        printf("Error opening file to copy\n");
        return 1;
    }
    readSuperBlock(disk, &diskSuperBlock);
    if(diskSuperBlock.freeInodeNum - 1 < 0)
    {
        printf("File limit reached\n");
        return 1;
    }
    fseek(sourceFile, 0, SEEK_END);
    strcpy(fileInode.name, fileName);
    fileInode.size = ftell(sourceFile);
    if(fileInode.size > MAX_FILE_SIZE || diskSuperBlock.freeBlocksNum <= 0)
    {
        printf("Not enough space on virtual disk\n");
        return 1;
    }
    if(isFileNameUsed(disk, fileName))
    {
        printf("File named %s already exists\n", fileName);
        return 1;
    }
    for(i = 0; i < 5; i++)
    {
        if (i < (fileInode.size + BLOCK_SIZE - 1)/BLOCK_SIZE)
        {
            int address = findFreeBlock(disk);
            fileInode.addresses[i] = address;
            diskSuperBlock.freeBlocksNum --;
            setBlockTaken(disk, address);
        }
        else
        {
            fileInode.addresses[i] = -1;
        }
    }
    saveData(disk, sourceFile, fileInode);
    saveInode(disk, fileInode);
    diskSuperBlock.freeInodeNum --;
    saveSuperBlock(disk, &diskSuperBlock);
    fclose(sourceFile);
    fclose(disk);
    printf("File %s copied to virtual disk\n", fileName);
    return 0;
}

int copyFromDiskToSystem(char *fileName)
{
    FILE *disk, *outputFile;
    struct iNodeTable diskInodeTable;
    struct iNode fileInode;
    int index;
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    if((outputFile = fopen(fileName, "w+")) == NULL)
    {
        printf("Error opening output file\n");
        return 1;
    }
    index = isFileNameUsed(disk, fileName);
    if(index)
    {
        index--;
        readInode(disk, &diskInodeTable);
        strcpy(fileInode.name, diskInodeTable.table[index].name);
        fileInode.size = diskInodeTable.table[index].size;
        memcpy(fileInode.addresses, diskInodeTable.table[index].addresses, 5);
        readData(disk, outputFile, fileInode);
    }
    else
    {
        printf("File not on disk\n");
        return 1;
    }
    fclose(outputFile);
    fclose(disk);
    printf("File %s copied to the system\n", fileName);
    return 0;
}

int printCatalog(void)
{
    FILE *disk;
    struct iNodeTable diskInodeTable;
    int nextline;
    int i;
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    nextline = 0;
    readInode(disk, &diskInodeTable);
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        if (diskInodeTable.table[i].size != -1)
        {
            printf("%s  ", diskInodeTable.table[i].name);
            nextline++;
        }
        if (nextline % 6 == 0 && nextline > 0)
        {
            printf("\n");
            nextline = 0;
        }

    }
    printf("\n");
    fclose(disk);
}

int printDiskStats(void)
{
    FILE *disk;
    int i, j, sum;
    struct SuperBlock diskSuperBlock;
    struct FreeBlocksMap diskBitMap;
    struct iNodeTable diskInodeTable;
    char addresses[20];
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    readSuperBlock(disk, &diskSuperBlock);
    printf("\n------------------------------------ DISK ------------------------------------\n");
    printf(" --total-dsize  --maxfsize  --maxfcount  --memblocks  --free-memblocks  -free-inodes\n");
    printf("%10dB %12dB %9d %11d %14d %16d\n", diskSuperBlock.totalDiskSize, diskSuperBlock.maxFileSize, diskSuperBlock.maxInodeNum,
             diskSuperBlock.maxBlockNum, diskSuperBlock.freeBlocksNum, diskSuperBlock.freeInodeNum);
    printf("\n-------------------------------- MEMMORY USAGE -------------------------------\n");
    printf("%32s %32s", "--percent-used", "--bytes-free\n");
    readBitmap(disk, &diskBitMap);
    sum = 0;
    fflush(stdout);
    for (i = 0; i < diskSuperBlock.maxBlockNum; i++)
    {
        if(diskBitMap.freeNodes[i] == 1)
            sum += diskBitMap.freeNodes[i];
    }
    printf("%26.2f%%  %32dB\n\n", (float)sum/diskSuperBlock.maxBlockNum * 100, diskSuperBlock.maxBlockNum * BLOCK_SIZE);
    for (i = 0; i < diskSuperBlock.maxBlockNum; i++)
    {
        if(diskBitMap.freeNodes[i] == 1)
            printf("  Block %03d: in use ", i);
        else
            printf("  Block %03d: free   ", i);
        if((i+1)%4 == 0)
            printf("\n");
    }
    fflush(stdout);
    readInode(disk, &diskInodeTable);
    printf("\n\n------------------------------------ FILES -----------------------------------\n");
    printf("%15s %12s %20s %20s\n","--name", "--size", "--blocks-used", "--addresses");
    for (i = 0; i<MAX_FILE_NUM; i++)
    {
        if(diskInodeTable.table[i].size != -1)
        {
            sum = 0;
            addresses[0] = '\0';
            for (j = 0; j < 5; j++)
            {
                if(diskInodeTable.table[i].addresses[j] >= 0)
                {
                    sum++;
                    snprintf(addresses + strlen(addresses), sizeof(addresses) - strlen(addresses), "%03d ", diskInodeTable.table[i].addresses[j]);
                }
            }
            printf("%13s %13dB %13d              %s\n", diskInodeTable.table[i].name, diskInodeTable.table[i].size, sum, addresses);
        }
    }
    fflush(stdout);
    printf("\n");
    return 0;
}

int printTotalSize(void)
{
    FILE *disk;
    int i, sum;
    struct iNodeTable diskInodeTable;
    struct SuperBlock diskSuperBlock;
    if((disk = fopen("virtualDisk", "r+")) == NULL)
    {
        printf("Error opening virtual disk\n");
        return 1;
    }
    sum = 0;
    readInode(disk, &diskInodeTable);
    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        if(diskInodeTable.table[i].size > 0)
            sum += diskInodeTable.table[i].size;
    }
    readSuperBlock(disk, &diskSuperBlock);
    printf("\nFile sizes sum: %12dB\n", sum);
    printf("Total memmory used: %8dB\n", (diskSuperBlock.maxBlockNum - diskSuperBlock.freeBlocksNum) * BLOCK_SIZE);
    return 0;
}

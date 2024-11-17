#include <stdio.h>
#include "superblock.h"


void readSuperBlock(FILE *disk, struct SuperBlock *returnSuperBlock)
{
    fseek(disk, 0, SEEK_SET);
    fread(returnSuperBlock, sizeof(struct SuperBlock), 1, disk);
}

void saveSuperBlock(FILE *disk, struct SuperBlock *updatedSuperBlock)
{
    fseek(disk, 0, SEEK_SET);
    fwrite(updatedSuperBlock, sizeof(struct SuperBlock), 1, disk);
}

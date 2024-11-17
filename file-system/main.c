#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char * argv[])
{
    int size, option, status, i;
    char filename[20];
    char placeholderText[50];
    if (argc != 3)
        return 1; 
    size = atoi(argv[1]);
    option = atoi(argv[2]);
    createDisk(size);
    switch(option)
    {
        case 1:
            printf("\n============================= TEST DATA VALIDITY =============================\n");
            copyFromSystemToDisk("test1.txt");
            printCatalog();
            printDiskStats();
            if(remove("test1.txt")==0)
                printf("test1.txt removed from the system\n");
            sleep(2);
            status = system("ls");
            printf("\n");
            copyFromDiskToSystem("test1.txt");
            break;

        case 2:
            printf("\n============================ TEST TOO LARGE FILE =============================\n");
            printDiskStats();
            printf("\n\n");
            sleep(2);
            status = system("du test2.txt");
            copyFromSystemToDisk("test2.txt");
            break;

        case 3:
            printf("\n============================ TEST TOO MANY FILES =============================\n");
            strcpy(placeholderText, "This is a placeholder text.");
            for (i = 1; i <= 65; ++i) {
            	FILE *file;
                sprintf(filename, "test3_%03d.txt", i);
                
                file = fopen(filename, "w");
                if (file == NULL) {
                    perror("Error opening file");
                    return 1;
                }
                fprintf(file, "%s\n", placeholderText);
                fclose(file);
                if ((i+1)%66 == 0)
                {
                	printCatalog();
                	printDiskStats();
                }
                copyFromSystemToDisk(filename);
            }
            break;
        case 4:
            for (i = 1; i <= 65; ++i)
            {
                sprintf(filename, "test3_%03d.txt", i);
                remove(filename);
            }
            printf("\n============================= TEST DELETE FILES ==============================\n");
            copyFromSystemToDisk("test4_1.txt");
            copyFromSystemToDisk("test4_2.txt");
            copyFromSystemToDisk("test4_3.txt");
            copyFromSystemToDisk("test4_4.txt");
            printCatalog();
            deleteFile("test4_1.txt");
            deleteFile("test4_3.txt");
            printCatalog();
            copyFromSystemToDisk("test4_5.txt");
            copyFromSystemToDisk("test4_5.txt");
            printCatalog();
            printDiskStats();
            break;
        case 5:
            printf("\n======================== TEST EXTERNAL FRAGMENTATION =========================\n");
            copyFromSystemToDisk("test5_1.txt");
            copyFromSystemToDisk("test5_2.txt");
            copyFromSystemToDisk("test5_3.txt");
            copyFromSystemToDisk("test5_4.txt");
            copyFromSystemToDisk("test5_5.txt");
            copyFromSystemToDisk("test5_6.txt");
            copyFromSystemToDisk("test5_7.txt");
            copyFromSystemToDisk("test5_8.txt");
            copyFromSystemToDisk("test5_9.txt");
            copyFromSystemToDisk("test5_10.txt");
            copyFromSystemToDisk("test5_11.txt");
            printCatalog();
            deleteFile("test5_3.txt");
            deleteFile("test5_5.txt");
            deleteFile("test5_7.txt");
            deleteFile("test5_10.txt");
            printCatalog();
            printDiskStats();
            copyFromSystemToDisk("test5_3.txt");
            copyFromSystemToDisk("test5_5.txt");
            copyFromSystemToDisk("test5_7.txt");
            copyFromSystemToDisk("test5_10.txt");
            printDiskStats();
            break;
        case 6:
            printf("\n======================== TEST INTERNAL FRAGMENTATION =========================\n");
            copyFromSystemToDisk("test6_1.txt");
            copyFromSystemToDisk("test6_2.txt");
            copyFromSystemToDisk("test6_3.txt");
            copyFromSystemToDisk("test6_4.txt");
            copyFromSystemToDisk("test6_5.txt");
            printDiskStats();
            printTotalSize();
            break;
    }
    deleteDisk();
    return 0;
}

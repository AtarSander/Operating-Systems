CC = cc

OBJECTS = main.o inode.o superblock.o data.o bitmap.o disk.o

all: main

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o main

main.o: main.c
	$(CC) -c main.c

inode.o: inode.c
	$(CC) -c inode.c

superblock.o: superblock.c
	$(CC) -c superblock.c

data.o: data.c
	$(CC) -c data.c

bitmap.o: bitmap.c
	$(CC) -c bitmap.c

disk.o: disk.c
	$(CC) -c disk.c

clean:
	rm -f $(OBJECTS) main

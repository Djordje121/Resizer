CC = gcc

all: resize.o bmp.o
	$(CC) resize.o bmp.o -o resize


resize.o: resize.c resize.h
	$(CC) -c resize.c

bmp.o: bmp.c bmp.h
	$(CC) -c bmp.c

clean:
	rm -f *.o resize
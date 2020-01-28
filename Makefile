all:FORCE
	/usr/bin/gcc -DDEBUG util.c array.c token.c consume.c make.c capture.c main.c -std=gnu99 -O3
	./a.out

FORCE:;
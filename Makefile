all:FORCE
	/usr/bin/gcc util.c options.c array.c token.c consume.c make.c capture.c main.c -std=gnu99 -O3
	./a.out

clear:
	rm *.out

FORCE:;
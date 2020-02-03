bin_PROGRAMS = libregex libregextest hashmaptest arraytest
libregex_SOURCES = array.h libregex.h array.c capture.c consume.c main.c make.c options.c token.c util.c find.c
libregex_CFLAGS = -g -O3

libregextest_SOURCES = array.h libregex.h array.c capture.c consume.c test.c make.c options.c token.c util.c find.c testlib.c testlib.h
libregextest_CFLAGS = -g -O3

hashmaptest_SOURCES = array.h array.c hashmap.c hashmap.h hashmaptest.c
hashmaptest_CFLAGS = -g -O3

arraytest_SOURCES = array.h array.c arraytest.c
arraytest_CFLAGS = -g -O3

TESTS = arraytest hashmaptest libregextest
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _array_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    void* arg = va_arg(args, char*);
    printf("%p\n", &arg);
    printf("%d\n", arg);
    va_end(args);
    fflush(stdout);
}

int main() {
    _array_debug("hoge\n", 3);
}
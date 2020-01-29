#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

void regex_options_init(regex_options* op){
    memset(op, 0, sizeof(regex_options));
}

regex_options* regex_options_new() {
    return (regex_options*)calloc(sizeof(regex_options), 1);
}

int regex_options_destruct(regex_options* op) {
    if (op->captured) {
        for (int i = 0; i < op->captured->length; i++) {
            free(array_at(op->captured, i));
        }
        free(op->captured);
        op->captured = NULL;
    }
}
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

char upper_case(char c) {
    return is_lower_case(c) ? c - 32 : c;
}

char lower_case(char c) {
    return is_upper_case(c) ? c + 32 : c;
}

bool is_upper_case(char c) {
    return ('A' <= c && c <= 'Z');
}

bool is_lower_case(char c) {
    return ('a' <= c && c <= 'z');
}

regex_state toggle_match_state(regex_state state) {
    return (RS_MATCHED + RS_FAILED) - state;
}
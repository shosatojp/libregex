#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

char tokenize(regex* root, const char** pat) {
    root->ms = array_new(sizeof(void*), true, 8);

    char c = '\0';

    while (c = **pat) {
        if (root->mp && (root->mp->type == RT_ANYOF || root->mp->type == RT_NONOF) &&
            strchr("+*.^$(|)[{}?", **pat)) {
            // []の中ではエスケープ無しで使える
            array_push(root->ms, make_consume_char_matcher(root, **pat));
        } else {
            switch (c) {
                case '+':
                case '*': {
                    regex* m0 = array_last(root->ms);
                    regex* m;
                    switch (c) {
                        case '+':
                            m = make_consume_plus_matcher(root, *(*pat - 1), m0);
                            break;
                        case '*':
                            m = make_consume_star_matcher(root, *(*pat - 1), m0);
                            break;
                    }

                    if (*(*pat + 1) == '?') {
                        m->non_greedy = true;
                        (*pat)++;
                    }

                    array_pop(root->ms);
                    array_push(root->ms, m);
                    break;
                }
                case '.': {
                    array_push(root->ms, make_consume_any_matcher(root));
                    break;
                }
                case '^': {
                    array_push(root->ms, make_consume_head_matcher(root));
                    break;
                }
                case '$': {
                    array_push(root->ms, make_consume_tail_matcher(root));
                    break;
                }
                case '(': {
                    regex* mor = make_consume_or_matcher(root);
                    (*pat)++;  // skip '('

                    // goto downer
                    bool break_while = false;
                    while (!break_while) {
                        regex* mtmp = make_consume_seq_matcher(root);
                        mtmp->mp = mor;
                        switch (tokenize(mtmp, pat)) {
                            case '|':
                                break;
                            case ')':
                            default:
                                break_while = true;
                                break;
                        }
                        array_push(mor->ms, mtmp);
                    }
                    array_push(root->ms, mor);
                    break;
                }
                case ')': {
                    // goto upper
                    if (root->mp->type == RT_OR) {
                        return ')';
                    } else {
                        break;
                    }
                }
                case '|': {
                    // goto next sibling
                    if (root->mp->type == RT_OR) {
                        (*pat)++;  // skip '|'
                        return '|';
                    } else {
                        break;
                    }
                }
                case '[': {
                    regex* manyof = NULL;
                    if (*(*pat + 1) == '^') {
                        manyof = make_consume_nonof_matcher(root);
                        (*pat) += 2;  // skip '[^'
                    } else {
                        manyof = make_consume_anyof_matcher(root);
                        (*pat)++;  // skip '['
                    }

                    // goto downer
                    bool break_while = false;
                    while (!break_while && **pat != ']') {
                        regex* mtmp = make_consume_seq_matcher(root);
                        mtmp->mp = manyof;
                        switch (tokenize(mtmp, pat)) {
                            case ']':
                                break_while = true;
                                break;
                            default:
                                break;
                        }
                        array_push(manyof->ms, mtmp);
                    }
                    array_push(root->ms, manyof);
                    break;
                }
                case ']': {
                    if (root->mp->type == RT_ANYOF || root->mp->type == RT_NONOF) {
                        return ']';
                    } else {
                        break;
                    }
                }
                case '?': {
                    if (root->mp && root->mp->type == RT_OR && *(*pat + 1) == ':') {
                        (*pat)++;
                        root->mp->no_capture = true;
                    } else {
                        regex* m0 = array_last(root->ms);
                        regex* m = make_consume_times_matcher(root, 0, 1, m0);

                        if (*(*pat + 1) == '?') {
                            m->non_greedy = true;
                            (*pat)++;
                        }

                        array_pop(root->ms);
                        array_push(root->ms, m);
                    }
                    break;
                }
                case '{': {
                    int u = -1, v = -1;
                    regex* m_root = make_pattern_matcher(root, "\\{(\\d+)(,)?(\\d+)?\\}");

                    regex_options op;
                    regex_options_init(&op);

                    regex_match(pat, m_root, &op);
                    (*pat)--;  // パターン読み取りで過ぎるから一つ戻す

                    switch (op.captured->length) {
                        case 1: {
                            v = regex_capture_int(array_at(op.captured, 0));
                            break;
                        }
                        case 2: {
                            u = regex_capture_int(array_at(op.captured, 0));
                            break;
                        }
                        case 3: {
                            u = regex_capture_int(array_at(op.captured, 0));
                            v = regex_capture_int(array_at(op.captured, 2));
                            break;
                        }
                        default:
                            break;
                    }
                    regex_options_destruct(&op);
                    regex_destruct(m_root);

                    regex* m0 = array_last(root->ms);
                    regex* m = make_consume_times_matcher(root, u, v, m0);

                    if (*(*pat + 1) == '?') {
                        m->non_greedy = true;
                        (*pat)++;
                    }

                    array_pop(root->ms);
                    array_push(root->ms, m);
                    break;
                }
                case '\\': {
                    (*pat)++;
                    switch (**pat) {
                        case 't':
                            array_push(root->ms, make_consume_char_matcher(root, '\t'));
                            break;
                        case 'r':
                            array_push(root->ms, make_consume_char_matcher(root, '\r'));
                            break;
                        case 'n':
                            array_push(root->ms, make_consume_char_matcher(root, '\n'));
                            break;
                        case 'f':
                            array_push(root->ms, make_consume_char_matcher(root, '\f'));
                            break;
                        case 'd':
                            array_push(root->ms, make_pattern_matcher(root, "[0-9]"));
                            break;
                        case 'D':
                            array_push(root->ms, make_pattern_matcher(root, "[^0-9]"));
                            break;
                        case 's':
                            array_push(root->ms, make_pattern_matcher(root, "[ \\t\\f\\r\\n]"));
                            break;
                        case 'S':
                            array_push(root->ms, make_pattern_matcher(root, "[^ \\t\\f\\r\\n]"));
                            break;
                        case 'w':
                            array_push(root->ms, make_pattern_matcher(root, "[a-zA-Z_0-9]"));
                            break;
                        case 'W':
                            array_push(root->ms, make_pattern_matcher(root, "[^a-zA-Z_0-9]"));
                            break;
                        default:
                            array_push(root->ms, make_consume_char_matcher(root, **pat));
                            break;
                    }
                    break;
                }
                case '-': {
                    if (root->mp->type == RT_ANYOF || root->mp->type == RT_NONOF) {
                        char prev = *(*pat - 1),
                             next = *(*pat + 1);
                        if (prev == '[' || (prev == '^' && *(*pat - 2) == '[') || next == ']') {
                            array_push(root->ms, make_consume_char_matcher(root, **pat));
                        } else {
                            regex* m = make_consume_span_matcher(root, prev, next);
                            (*pat)++;

                            regex_destruct(array_pop(root->mp->ms));
                            array_push(root->ms, m);
                        }
                        break;
                    }
                    // nobreak
                }
                default: {
                    array_push(root->ms, make_consume_char_matcher(root, c));
                    break;
                }
            }
        }

        (*pat)++;
        if (root->mp && (root->mp->type == RT_ANYOF || root->mp->type == RT_NONOF)) {
            return '\0';
        }
    }
}

int regex_destruct(regex* root) {
    if (root && root->ms) {
        for (int i = 0; i < root->ms->length; i++) {
            regex* m = array_at(root->ms, i);
            regex_destruct(m);
            if (m) {
                free(m);
                m = NULL;
            }
        }
        free(root->ms);
        root->ms = NULL;
    }
}

void consumer_debug(regex* m, const char* format, ...) {
#ifdef DEBUG
    for (int i = 0, depth = matcher_depth(m); i < depth; i++) {
        printf("|   ");
    }
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    putc('\n', stdout);
    fflush(stdout);
#endif
}

void debug(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
}

int matcher_depth(regex* m0) {
    regex* m = m0;
    int depth = 0;
    while (m->mp) {
        m = m->mp;
        depth++;
    }
    return depth;
}

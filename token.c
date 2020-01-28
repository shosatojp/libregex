#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

char tokenize(struct matcher* root, const char** pat) {
    root->ms = array_new(sizeof(void*), true);
    array_expand(root->ms, 8);

    int count = 0;
    char c = '\0';

    while (c = **pat) {
        debug("%d,%c\n", count, c);
        switch (c) {
            case '+':
            case '*': {
                struct matcher* m0 = (struct matcher*)array_at(root->ms, count - 1);
                struct matcher* m;
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
                array_del(root->ms, root->ms->length - 1);
                array_push(root->ms, m);
                break;
            }
            case '.': {
                array_push(root->ms, make_consume_any_matcher(root));
                count++;
                break;
            }
            case '^': {
                array_push(root->ms, make_consume_head_matcher(root));
                count++;
                break;
            }
            case '$': {
                array_push(root->ms, make_consume_tail_matcher(root));
                count++;
                break;
            }
            case '(': {
                struct matcher* mor = make_consume_or_matcher(root);
                (*pat)++;  // skip '('

                // goto downer
                bool break_while = false;
                while (!break_while) {
                    struct matcher* mtmp = make_consume_seq_matcher(root);
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
                count++;
                break;
            }
            case ')': {
                // goto upper
                if (root->mp->type == MT_OR) {
                    return ')';
                } else {
                    break;
                }
            }
            case '|': {
                // goto next sibling
                if (root->mp->type == MT_OR) {
                    (*pat)++;  // skip '|'
                    return '|';
                } else {
                    break;
                }
            }
            case '[': {
                struct matcher* manyof = NULL;
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
                    struct matcher* mtmp = make_consume_seq_matcher(root);
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
                count++;
                break;
            }
            case ']': {
                if (root->mp->type == MT_ANYOF || root->mp->type == MT_NONOF) {
                    return ']';
                } else {
                    break;
                }
            }
            case '?': {
                if (root->mp->type == MT_OR && *(*pat + 1) == ':') {
                    (*pat)++;
                    root->mp->nocapture = true;
                } else {
                    struct matcher* m0 = (struct matcher*)array_at(root->ms, count - 1);
                    struct matcher* m = make_consume_times_matcher(root, 0, 1, m0);

                    array_del(root->ms, root->ms->length - 1);
                    array_push(root->ms, m);
                    count++;
                }
                break;
            }
            case '{': {
                int u = -1, v = -1;
                struct matcher* m_root = make_pattern_matcher(root, "\\{(\\d+)(?,(\\d+))\\}");
                root_executor(pat, m_root);
                (*pat)--;  // パターン読み取りで過ぎるから一つ戻す

                struct Array* captured = array_new(sizeof(struct capture*), true);
                _captured_all(m_root, captured);

                switch (captured->length) {
                    case 1: {
                        v = capture_int((struct capture*)array_at(captured, 0));
                        break;
                    }
                    case 2: {
                        u = capture_int((struct capture*)array_at(captured, 0));
                        v = capture_int((struct capture*)array_at(captured, 1));
                    }
                    default:
                        break;
                }
                destruct_matcher(m_root);

                struct matcher* m0 = (struct matcher*)array_at(root->ms, count - 1);
                struct matcher* m = make_consume_times_matcher(root, u, v, m0);

                array_del(root->ms, root->ms->length - 1);
                array_push(root->ms, m);
                count++;
                break;
            }
            case '\\': {
                (*pat)++;
                switch (**pat) {
                    case 't':
                        array_push(root->ms, make_consume_char_matcher(root, '\t'));
                        count++;
                        break;
                    case 'r':
                        array_push(root->ms, make_consume_char_matcher(root, '\r'));
                        count++;
                        break;
                    case 'n':
                        array_push(root->ms, make_consume_char_matcher(root, '\n'));
                        count++;
                        break;
                    case 'f':
                        array_push(root->ms, make_consume_char_matcher(root, '\f'));
                        count++;
                        break;
                    case 'd':
                        array_push(root->ms, make_pattern_matcher(root, "[0-9]"));
                        count++;
                        break;
                    case 'D':
                        array_push(root->ms, make_pattern_matcher(root, "[^0-9]"));
                        count++;
                        break;
                    case 's':
                        array_push(root->ms, make_pattern_matcher(root, "[ \\t\\f\\r\\n]"));
                        count++;
                        break;
                    case 'S':
                        array_push(root->ms, make_pattern_matcher(root, "[^ \\t\\f\\r\\n]"));
                        count++;
                        break;
                    case 'w':
                        array_push(root->ms, make_pattern_matcher(root, "[a-zA-Z_0-9]"));
                        count++;
                        break;
                    case 'W':
                        array_push(root->ms, make_pattern_matcher(root, "[^a-zA-Z_0-9]"));
                        count++;
                        break;
                    default:
                        array_push(root->ms, make_consume_char_matcher(root, **pat));
                        count++;
                        break;
                }
                break;
            }
            case '-': {
                if (root->mp->type == MT_ANYOF || root->mp->type == MT_NONOF) {
                    struct matcher* m = make_consume_span_matcher(root, *(*pat - 1), *(*pat + 1));
                    (*pat)++;

                    array_del(root->mp->ms, root->mp->ms->length - 1);  // TODO: free()
                    array_push(root->ms, m);
                    count++;
                    break;
                }
                // nobreak
            }
            default: {
                array_push(root->ms, make_consume_char_matcher(root, c));
                count++;
                break;
            }
        }
        (*pat)++;
        if (root->mp && (root->mp->type == MT_ANYOF || root->mp->type == MT_NONOF)) {
            return '\0';
        }
    }
}

int destruct_matcher(struct matcher* root) {
    if (root->ms) {
        for (int i = 0; i < root->ms->length; i++) {
            struct matcher* m = (struct matcher*)array_at(root->ms, i);
            if (m->captured) {
                free(m->captured);
                m->captured = NULL;
            }
            destruct_matcher(m);
            if (m) {
                free(m);
                m = NULL;
            }
        }
        free(root->ms);
        root->ms = NULL;
    }
}

/* initialize str based data */
int clear_matcher(struct matcher* root) {
    if (root->ms) {
        for (int i = 0; i < root->ms->length; i++) {
            struct matcher* m = (struct matcher*)array_at(root->ms, i);
            if (m->captured) {
                free(m->captured);
                m->captured = NULL;
            }
            clear_matcher(m);
        }
    }
    root->head = NULL;
    root->tail = NULL;
}

void consumer_debug(struct matcher* m, const char* format, ...) {
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

int matcher_depth(struct matcher* m0) {
    struct matcher* m = m0;
    int depth = 0;
    while (m->mp) {
        m = m->mp;
        depth++;
    }
    return depth;
}

int find_all(const char** ptr, struct matcher* m) {
    const char* head = *ptr;
    while (**ptr) {
        const char* init = *ptr;
        m->head = head;
        switch (match_regex(ptr, m)) {
            case MS_MATCHED: {
                char* str = (char*)strcut(init, *ptr - 1);
                printf(">>%s\n", str);
                free(str);
                break;
            }
            case MS_FAILED:
                *ptr = init + 1;
                break;
        }
        clear_matcher(m);
    }
}
#pragma once
#include <stdarg.h>
#include <stdbool.h>

struct _regex;
struct _regex_options;
struct _regex_capture;
enum _regex_state;
enum _regex_type;

typedef struct _regex regex;
typedef struct _regex_options regex_options;
typedef struct _regex_capture regex_capture;
typedef enum _regex_state regex_state;
typedef enum _regex_type regex_type;

enum _regex_state {
    RS_MATCHED = 0,
    RS_FAILED
};

enum _regex_type {
    RT_SPAN,
    RT_CHAR,
    RT_PLUS,
    RT_STAR,
    RT_NOT,
    RT_OR,
    RT_ROOT,
    RT_ANY,
    RT_NONOF,
    RT_ANYOF,
    RT_HEAD,
    RT_TAIL,
    RT_SEQ,
    RT_TIMES
};

typedef regex_state (*consumer)(const char**, regex*, regex_options*);

struct _regex {
    regex_type type;

    /* consumer function */
    consumer fn;

    /* pattern based data */
    char c;
    char c2;
    char* matched_c;
    int u, v;
    bool ignore_case;
    bool multi_line;
    bool dot_all;
    bool non_greedy;
    bool noregex_capture;

    /* parent */
    regex* mp;

    /* children */
    array* ms;
};

/* options at execution */
struct _regex_options {
    /* str based data */
    const char* head;
    const char* tail;
    array* captured;
};

/* regex_capture result struct */
struct _regex_capture {
    const char* begin;
    const char* end;
};

/* consume.c */
regex_state toggle_match_state(regex_state state);
regex_state consume_char(const char** ptr, regex* m, regex_options* op);
regex_state consume_span(const char** ptr, regex* m, regex_options* op);
regex_state consume_times(const char** ptr, regex* m, regex_options* op);
regex_state consume_plus(const char** ptr, regex* m, regex_options* op);
regex_state consume_or(const char** ptr, regex* m, regex_options* op);
regex_state consume_nor(const char** ptr, regex* m, regex_options* op);
regex_state consume_head(const char** ptr, regex* m, regex_options* op);
regex_state consume_tail(const char** ptr, regex* m, regex_options* op);
regex_state consume_any(const char** ptr, regex* m, regex_options* op);
regex_state consume_seq(const char** ptr, regex* m, regex_options* op);
regex_state regex_match(const char** ptr, regex* m, regex_options* op);
regex_state consume_not(const char** ptr, regex* m, regex_options* op);

/* make.c */
regex* make_empty_matcher();
regex* make_consume_char_matcher(regex* mp, char c);
regex* make_consume_head_matcher(regex* mp);
regex* make_consume_tail_matcher(regex* mp);
regex* make_consume_times_matcher(regex* mp, int u, int v, regex* m0);
regex* make_consume_span_matcher(regex* mp, char c, char c2);
regex* make_consume_not_matcher(regex* mp, regex* m0);
regex* make_consume_or_matcher(regex* mp);
regex* make_consume_anyof_matcher(regex* mp);
regex* make_consume_nonof_matcher(regex* mp);
regex* make_consume_seq_matcher(regex* mp);
regex* make_consume_any_matcher(regex* mp);
regex* make_consume_plus_matcher(regex* mp, char c, regex* m0);
regex* make_consume_star_matcher(regex* mp, char c, regex* m0);
regex* make_pattern_matcher(regex* mp, const char* pat);
regex* regex_compile(const char* pat, const char* flags);

/* token.c */
char tokenize(regex* root, const char** pat);
int regex_destruct(regex* root);
int matcher_depth(regex* m0);
void consumer_debug(regex* m, const char* format, ...);
void debug(const char* format, ...);
int find_all(const char** ptr, regex* m);

/* regex_capture.c */
regex_capture* regex_capture_new(const char* begin, const char* end);
int regex_capture_int(regex_capture* c);
char* regex_capture_str(regex_capture* c);
char* strcut(const char* begin, const char* end);

/* util.c */
regex_state toggle_match_state(regex_state state);
char upper_case(char c);
char lower_case(char c);
bool is_upper_case(char c);
bool is_lower_case(char c);

/* options.c */
regex_options* regex_options_new();
int regex_options_destruct(regex_options* op);
void regex_options_init(regex_options* op);

#pragma once
#include <stdarg.h>
#include <stdbool.h>

struct matcher;

typedef struct matcher regex;

enum MATCH_STATE {
    MS_MATCHED = 0,
    MS_FAILED
};

typedef enum MATCH_STATE match_state;
typedef enum MATCH_STATE (*consume_fn)(const char**, struct matcher*);

enum MATCH_STATE toggle_match_state(enum MATCH_STATE state);

enum MATCHER_TYPE {
    MT_SPAN,
    MT_CHAR,
    MT_PLUS,
    MT_STAR,
    MT_NOT,
    MT_OR,
    MT_ROOT,
    MT_ANY,
    MT_NONOF,
    MT_ANYOF,
    MT_HEAD,
    MT_TAIL,
    MT_SEQ,
    MT_TIMES
};

struct matcher {
    enum MATCHER_TYPE type;
    /* consumer function */
    consume_fn fn;

    /* pattern based data */
    char c;
    char c2;
    char* matched_c;
    int u, v;
    bool ignore_case;
    bool multi_line;
    bool dot_all;
    bool non_greedy;
    bool nocapture;

    /* str based data */
    const char* head;
    const char* tail;
    struct capture* captured;

    /* parent */
    struct matcher* mp;

    /* children */
    struct Array* ms;
};

/* capture result struct */
struct capture {
    const char* begin;
    const char* end;
};

/* consume.c */
enum MATCH_STATE toggle_match_state(enum MATCH_STATE state);
enum MATCH_STATE consume_char(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_span(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_times(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_plus(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_or(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_nor(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_head(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_tail(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_any(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_seq(const char** ptr, struct matcher* m);
enum MATCH_STATE root_executor(const char** ptr, struct matcher* m);
enum MATCH_STATE consume_not(const char** ptr, struct matcher* m);
#define match_regex(...) root_executor(__VA_ARGS__)

/* make.c */
void copy_constants(struct matcher* m);
struct matcher* make_empty_matcher();
struct matcher* make_consume_char_matcher(struct matcher* mp, char c);
struct matcher* make_consume_head_matcher(struct matcher* mp);
struct matcher* make_consume_tail_matcher(struct matcher* mp);
struct matcher* make_consume_times_matcher(struct matcher* mp, int u, int v, struct matcher* m0);
struct matcher* make_consume_span_matcher(struct matcher* mp, char c, char c2);
struct matcher* make_consume_not_matcher(struct matcher* mp, struct matcher* m0);
struct matcher* make_consume_or_matcher(struct matcher* mp);
struct matcher* make_consume_anyof_matcher(struct matcher* mp);
struct matcher* make_consume_nonof_matcher(struct matcher* mp);
struct matcher* make_consume_seq_matcher(struct matcher* mp);
struct matcher* make_consume_any_matcher(struct matcher* mp);
struct matcher* make_consume_plus_matcher(struct matcher* mp, char c, struct matcher* m0);
struct matcher* make_consume_star_matcher(struct matcher* mp, char c, struct matcher* m0);
struct matcher* make_pattern_matcher(struct matcher* mp, const char* pat);
struct matcher* compile_regex(const char* pat, const char* flags);

/* token.c */
char tokenize(struct matcher* root, const char** pat);
int destruct_matcher(struct matcher* root);
#define destruct_regex(...) destruct_matcher(__VA_ARGS__);
int clear_matcher(struct matcher* root);
#define clear_regex(...) clear_matcher(__VA_ARGS__);
int matcher_depth(struct matcher* m0);
void consumer_debug(struct matcher* m, const char* format, ...);
void debug(const char* format, ...);
int find_all(const char** ptr, struct matcher* m);

/* capture.c */
struct capture* capture_new(const char* begin, const char* end);
int _captured_all(struct matcher* root, struct Array* cs);
struct Array* captured_all(struct matcher* root);
int capture_int(struct capture* c);
char* capture_str(struct capture* c);
char* strcut(const char* begin, const char* end);

/* util.c */
char upper_case(char c);
char lower_case(char c);
bool is_upper_case(char c);
bool is_lower_case(char c);
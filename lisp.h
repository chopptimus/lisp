#include <stdlib.h>

// Keeps track of how much we've read.
typedef struct tokens {
    char **tokens;
    size_t size;
    size_t head;
} Tokens;

// Types that an atom can be.
typedef enum type {
    NUMBER,
    STRING,
    SYMBOL,
    LIST,
    FUNCTION
} Type;

struct object;

// Holds information about the symbols.
typedef struct environment {
} Environment;

// This is a lisp after all.
typedef struct list {
    struct object *first;
    struct list *rest;
} List;

typedef struct function {
    Environment *env;
    struct object *body;
} Function;

typedef struct object {
    Type type;
    union {
        int int_val;
        char *str_val;
        List *list_val;
        Function *func_val;
    };
} Object;

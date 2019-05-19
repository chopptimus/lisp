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

// This is a lisp after all.
typedef struct list {
    struct object *first;
    struct list *rest;
} List;

// Everything is either a list or an atom.
typedef struct object {
    Type type;
    union {
        int int_val;
        char *str_val;
        List *list_val;
    };
} Object;

// Holds information about the symbols.
typedef struct environment {
} Environment;

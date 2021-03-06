#include <stdlib.h>

// Keeps track of how much we've read.
typedef struct tokens {
    char **tokens;
    size_t size;
    size_t head;
} Tokens;

// Types that an atom can be.
typedef enum type {
    LONG,
    FLOAT,
    STRING,
    SYMBOL,
    LIST,
    FUNCTION
} Type;

struct object;

typedef struct key_value {
    char *key;
    struct object *val;
} KeyValue;

typedef struct table {
    KeyValue *entries[20];
    size_t head;
} Table;

// Holds information about the symbols.
typedef struct environment {
    struct environment *parent;
    Table *table;
} Environment;

// This is a lisp after all.
typedef struct list {
    struct object *first;
    struct list *rest;
} List;

typedef struct function {
    struct object *body;
    Environment *env;
    List *parameters;
} Function;

typedef struct object {
    Type type;
    union {
        int long_val;
        float float_val;
        char *str_val;
        char *sym_val;
        List *list_val;
        Function *fn_val;
    };
} Object;

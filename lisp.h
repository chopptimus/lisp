#include <stdlib.h>

// Keeps track of how much we've read.
typedef struct tokens {
    char **tokens;
    size_t size;
    size_t head;
} Tokens;

// Types that an atom can be.
enum atom_type {
    NUMBER,
    STRING,
    SYMBOL
};

// Number, string or symbol.
typedef struct atom {
    enum atom_type type;
    union {
        int ival;
        char *sval;
    } value;
} Atom;

struct expr;

// This is a lisp after all.
typedef struct list {
    struct expr *first;
    struct list *rest;
} List;

enum exprtype { ATOM, LIST };

// Everything is either a list or an atom.
typedef struct expr {
    enum exprtype type;
    union {
        Atom *aexpr;
        List *lexpr;
    } expr;
} Expr;

// Holds information about the symbols.
typedef struct environment {
} Environment;

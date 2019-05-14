#include <stdlib.h>

// Just a list of tokens for parsing into an AST.
struct Tokens {
    size_t size;
    char **tokens;
};

// Types that an atom can be.
enum Type {
    Number,
    String,
    Symbol
};

// Number, string or symbol.
typedef struct {
    enum Type type;
    void *data;
} Atom;

// This is a lisp after all.
typedef struct List {
    Atom *first;
    struct List *rest;
} List;

// Holds information about the symbols.
typedef struct {
} Environment;

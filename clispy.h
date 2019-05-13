#include <stdlib.h>

enum Type {
    Number,
    Symbol,
    List
};

typedef struct ParseTree {
    enum Type type;
    struct ParseTree *children;
} ParseTree;

typedef struct {
    size_t size;
    char **tokens;
} Tokens;

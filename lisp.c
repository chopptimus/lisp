#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"

#define thead(t) t->tokens[t->head]

void atom_repr(Atom *atom) {
    if (atom->type == NUMBER) {
        printf("%d", atom->value.ival);
    } else if (atom->type == SYMBOL) {
        printf("%s", atom->value.sval);
    }
}

void repr(const Expr *expr);

void list_repr(const List *list) {
    if (list == NULL) {
        printf("'()");
    }
    printf("(");

    repr(list->first);

    list = list->rest;
    while (list != NULL) {
        printf(" ");
        repr(list->first);
        list = list->rest;
    }

    printf(")");
}

void repr(const Expr *expr) {
    if (expr->type == LIST) {
        list_repr(expr->expr.lexpr);
    } else {
        atom_repr(expr->expr.aexpr);
    }
}

Atom *parse_atom(Atom *atom, const char *token) {
    if isdigit(token[0]) {
        atom->type = NUMBER;
        atom->value.ival = atoi(token);
    } else {
        atom->type = SYMBOL;
        size_t len = strlen(token);
        char *symbol = malloc(sizeof(char) * (len + 1));
        strcpy(symbol, token);
        atom->value.sval = symbol;
    }

    return atom;
}

List *parse_list(List *list, Tokens *tokens);

Expr *parse_expr(Expr *expr, Tokens *tokens) {
    if (strcmp(thead(tokens), "(") == 0) {
        List *list = malloc(sizeof(List));
        list = parse_list(list, tokens);
        expr->type = LIST;
        expr->expr.lexpr = list;
    } else {
        Atom *atom = malloc(sizeof(Atom));
        atom = parse_atom(atom, thead(tokens));
        tokens->head += 1;
        expr->type = ATOM;
        expr->expr.aexpr = atom;
    }
    return expr;
}

List *parse_list(List *list, Tokens *tokens) {
    if (tokens->size < 2) {
        printf("List must have at least 2 tokens.\n");
        exit(1);
    } else if (strcmp(thead(tokens), "(") != 0) {
        printf("List must start with an open paren.\n");
        exit(1);
    } else if (strcmp(thead(tokens), ")") == 0) {
        list = NULL;
        tokens->head += 2;
        return list;
    } else if (tokens->size < 3) {
        printf("If list isn't empty, it must contain at least 3 tokens\n");
        exit(1);
    }
    tokens->head++; // Opening paren.

    List *first = list;
    Expr *expr = malloc(sizeof(Expr));

    first->first = parse_expr(expr, tokens);
    first->rest = NULL;

    char *token = thead(tokens);
    while (strcmp(token, ")") != 0) {
        first->rest = malloc(sizeof(List));
        first = first->rest;
        expr = malloc(sizeof(Expr));
        first->first = parse_expr(expr, tokens);
        first->rest = NULL;

        if (tokens->head == tokens->size) {
            // Reaching the end of the input without a closing paren is an error.
            printf("No closing paren.\n");
            exit(1);
        }
        token = thead(tokens);
    }

    tokens->head++; // Closing paren;
    return list;
}

char *capture_token(const char *s, size_t n) {
    char *tok = malloc(sizeof(char) * (n + 1));
    strncpy(tok, s, n);
    tok[n] = 0;
    return tok;
}

Tokens tokenize(const char *s, size_t n) {
    char **tokens = malloc(sizeof(char *) * n);
    int j = 0;
    int start = 0;
    for (int i = 0; i < n; i++) {
        if (s[i] == '(' || s[i] == ')') {
            size_t len = i - start;
            if (len > 0) {
                tokens[j] = capture_token(&s[start], len);
                j++;
            }
            tokens[j] = capture_token(&s[i], 1);
            j++;
            start = i + 1;
        } else if (s[i] == ' ') {
            size_t len = i - start;
            if (len > 0) {
                tokens[j] = capture_token(&s[start], len);
                j++;
            }
            start = i + 1;
        }
    }
    if (start != n) {
        tokens[j] = capture_token(&s[start], n - start - 1);
        j++;
    }

    tokens = realloc(tokens, sizeof(char *) * j);
    return (Tokens) { .tokens = tokens, .size = j, .head = 0 };
}

// Homoiconicity and all that.
Expr *parse(const char *program, size_t n) {
    if (n == 0) {
        exit(0);
    }

    Tokens tokens = tokenize(program, n);

    Expr *expr = malloc(sizeof(Expr));
    return parse_expr(expr, &tokens);
}

int main(int args, char *argv[]) {
    FILE *f = fopen(argv[1], "rb"); // Error handling?
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *program = malloc(fsize + 1);
    fread(program, 1, fsize, f);
    fclose(f);

    program[fsize] = 0;

    Expr *expr = parse(program, fsize);
    repr(expr);
    printf("\n");
}

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clispy.h"

char *capture_token(const char *s, size_t n) {
    char *tok = malloc(sizeof(char) * (n + 1));
    strncpy(tok, s, n);
    tok[n] = 0;
    return tok;
}

struct Tokens tokenize(const char *s, size_t n) {
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

    tokens = realloc(tokens, sizeof(char *) * j);
    return (struct Tokens) { j, tokens };
}

void atom_repr(Atom *atom) {
    if (atom->type == Number) {
        printf("Number %d\n", *(int *)atom->data);
    } else if (atom->type == Symbol) {
        printf("Symbol %s\n", (char *)atom->data);
    }
}

Atom *parse_atom(Atom *atom, const char *token) {
    if isdigit(token[0]) {
        atom->type = Number;
        int *number = malloc(sizeof(int));
        *number = atoi(token);
        atom->data = number;
    } else {
        atom->type = Symbol;
        size_t len = strlen(token);
        char *symbol = malloc(sizeof(char) * (len + 1));
        strcpy(symbol, token);
        atom->data = symbol;
    }

    return atom;
}

List *parse_list(List *list, char **tokens, size_t n) {
    if (strcmp(tokens[0], "(") != 0) {
        printf("Syntax error.");
        exit(1);
    }

    Atom *first;
    first = malloc(sizeof(Atom));
    list->first = parse_atom(first, tokens[1]);
    list->rest = NULL;

    char *next = tokens[2];
    int i = 3;
    List *rest;
    while (strcmp(next, ")") != 0) {
        if (i == n) {
            printf("Syntax error.");
            exit(1);
        }
        first = malloc(sizeof(Atom));
        list->first = parse_atom(first, tokens[i]);
        printf("%d\n", strcmp(next, ")"));
        atom_repr(first);
        rest = malloc(sizeof(List));
        list->rest = rest;
        list = rest;
        next = tokens[i];
        i++;
    }
    return list;
}

// Homoiconicity and all that.
List *parse(const char *program, size_t n) {
    struct Tokens ts = tokenize(program, n);
    char **tokens = ts.tokens;
    char size = ts.size;

    if (size == 0) {
        exit(0);
    }

    List *list = malloc(sizeof(List));
    return parse_list(list, tokens, size);
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

    List *list = parse(program, fsize);
    atom_repr(list->first);
}

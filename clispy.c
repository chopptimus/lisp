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
        printf("%d", *(int *)atom->data);
    } else if (atom->type == Symbol) {
        printf("%s", (char *)atom->data);
    }
}

void list_repr(const List *list) {
    if (list == NULL) {
        printf("'()");
    }
    printf("(");
    atom_repr(list->first);
    list = list->rest;
    while (list != NULL) {
        printf(" ");
        atom_repr(list->first);
        list = list->rest;
    }
    printf(")");
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
    if (n < 2) {
        printf("List must have at least 2 tokens.\n");
        exit(1);
    } else if (strcmp(tokens[0], "(") != 0) {
        printf("List must start with an open paren.\n");
        exit(1);
    } else if (strcmp(tokens[1], ")") == 0) {
        list = NULL;
        return list;
    } else if (n < 3) {
        printf("If list isn't empty, it must contain at least 3 tokens\n");
        exit(1);
    }
    List *head = list;

    Atom *a = malloc(sizeof(Atom));
    list->first = parse_atom(a, tokens[1]);
    list->rest = NULL;

    char *token = tokens[2];
    int i = 2;
    while (strcmp(token, ")") != 0) {
        list->rest = malloc(sizeof(List));
        list = list->rest;
        a = malloc(sizeof(Atom));
        list->first = parse_atom(a, token);
        list->rest = NULL;

        i++;
        if (i == n) {
            // Reaching the end of the input without a closing paren is an error.
            printf("No closing paren.\n");
            exit(1);
        }
        token = tokens[i];
    }

    // It's a syntax error if there's any input left.
    if (i + 1 != n) {
        printf("There is still more input to parse.\n");
        exit(1);
    }

    return head;
}

// Homoiconicity and all that.
List *parse(const char *program, size_t n) {
    if (n == 0) {
        exit(0);
    }

    struct Tokens ts = tokenize(program, n);
    char **tokens = ts.tokens;
    char size = ts.size;

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
    list_repr(list);
}

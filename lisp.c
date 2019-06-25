#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"

#define THEAD(t) t->tokens[t->head]

void repr(const Object *obj);

void list_repr(const List *list)
{
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

void repr(const Object *obj)
{
    switch (obj->type) {
        case LIST:
            list_repr(obj->list_val);
            break;
        case NUMBER:
            printf("%d", obj->int_val);
            break;
        case SYMBOL:
        case STRING:
            printf("%s", obj->str_val);
            break;
        case FUNCTION:
            printf("fn");
    }
}

char *capture_token(const char *s, size_t n)
{
    char *tok = malloc(sizeof(char) * (n + 1));
    strncpy(tok, s, n);
    tok[n] = 0;
    return tok;
}

Tokens tokenize(const char *s, size_t n)
{
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

Object *read(Object *obj, Tokens *tokens);

List *read_list(List *list, Tokens *tokens)
{
    if (tokens->size < 1) {
        printf("List must have at least 2 tokens.\n");
        exit(1);
    } else if (strcmp(THEAD(tokens), ")") == 0) {
        list = NULL;
        tokens->head += 2;
        return list;
    } else if (tokens->size < 2) {
        printf("If list isn't empty, it must contain at least 3 tokens\n");
        exit(1);
    }

    Object *obj = malloc(sizeof(Object));
    List *first = list;

    first->first = read(obj, tokens);
    first->rest = NULL;

    char *token = THEAD(tokens);
    while (strcmp(token, ")") != 0) {
        first->rest = malloc(sizeof(List));
        first = first->rest;
        obj = malloc(sizeof(Object));
        first->first = read(obj, tokens);
        first->rest = NULL;

        if (tokens->head == tokens->size) {
            // Reaching the end of the input without a closing paren is an error.
            printf("No closing paren.\n");
            exit(1);
        }
        token = THEAD(tokens);
    }

    tokens->head++; // Closing paren;
    return list;
}

Object *read(Object *obj, Tokens *tokens)
{
    if (strcmp(THEAD(tokens), "(") == 0) {
        tokens->head++;
        List *list = malloc(sizeof(List));
        list = read_list(list, tokens);
        obj->type = LIST;
        obj->list_val = list;
    } else if (isdigit(THEAD(tokens)[0])) {
        obj->type = NUMBER;
        obj->int_val = atoi(THEAD(tokens));
        tokens->head++;
    } else {
        obj->type = SYMBOL;
        size_t len = strlen(THEAD(tokens));
        char *symbol = malloc(sizeof(char) * (len + 1));
        strcpy(symbol, THEAD(tokens));
        symbol[len] = 0;
        obj->str_val = symbol;
        tokens->head++;
    }
    return obj;
}

Object *eval(Object *result, Object *obj, Environment *env);

Object *apply(Object *result, Object *fn, List *arguments)
{
    if (strcmp(fn->name, "+") == 0) {
    }
}

Object *eval(Object *result, Object *obj, Environment *env)
{
    if (obj->type == LIST) {
        Object *fn = eval(result, obj->list_val->first, env);
        if (fn->type != FUNCTION) {
            printf("Not a function.\n");
        }
        Object *result = malloc(sizeof(Object));
        return apply(result, fn, obj->list_val->rest);
    } else {
        return obj;
    }
}

int main(int args, char *argv[])
{
    if (args != 2)
        exit(1);

    FILE *f = fopen(argv[1], "rb"); // Error handling?
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *program = malloc(fsize + 1);
    fread(program, 1, fsize, f);
    fclose(f);

    if (fsize == 0)
        exit(0);

    program[fsize] = 0;

    Tokens tokens = tokenize(program, fsize);
    Object *obj = malloc(sizeof(Object));
    obj = read(obj, &tokens);

    repr(obj);
    printf("\n");
}

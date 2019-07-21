#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"

#define THEAD(t) t->tokens[t->head]

#define car(l) l->list_val->first
#define cdr(l) l->list_val->rest

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

void type_repr(const Object *obj)
{
    switch (obj->type) {
        case LIST:
            printf("LIST\n");
            break;
        case FUNCTION:
            printf("FUNCTION\n");
            break;
        case LONG:
            printf("LONG\n");
            break;
        case FLOAT:
            printf("FLOAT\n");
            break;
        case STRING:
            printf("STRING\n");
            break;
        case SYMBOL:
            printf("SYMBOL\n");
            break;
    }
}

void repr(const Object *obj)
{
    switch (obj->type) {
        case LIST:
            list_repr(obj->list_val);
            break;
        case LONG:
            printf("%d", obj->long_val);
            break;
        case FLOAT:
            printf("%f", obj->float_val);
            break;
        case SYMBOL:
            printf("%s", obj->sym_val);
            break;
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
    char *tok = THEAD(tokens);
    if (strcmp(tok, "(") == 0) {
        tokens->head++;
        List *list = malloc(sizeof(List));
        list = read_list(list, tokens);
        obj->type = LIST;
        obj->list_val = list;
        return obj;
    }

    char **endptr = malloc(sizeof(char **));
    size_t len = strlen(tok);
    long long_val = strtol(tok, endptr, 10);
    if (*endptr == &tok[len]) {
        obj->type = LONG;
        obj->long_val = long_val;
        tokens->head++;
        return obj;
    }

    float float_val = strtof(THEAD(tokens), endptr);
    if (*endptr != THEAD(tokens)) {
        obj->type = FLOAT;
        obj->float_val = float_val;
        tokens->head++;
        return obj;
    } else {
        obj->type = SYMBOL;
        char *symbol = malloc(sizeof(char) * (len + 1));
        strcpy(symbol, THEAD(tokens));
        symbol[len] = 0;
        obj->sym_val = symbol;
        tokens->head++;
        return obj;
    }
}

Object *get(char *k, Table *t)
{
    KeyValue *keyval;
    for (int i = 0; i < 20; i++) {
        keyval = t->entries[i];
        if (strcmp(keyval->key, k) == 0) {
            return keyval->val;
        }
    }
    return NULL;
}

Object *resolve(char *sym, Environment *env)
{
    Object *result = get(sym, env->table);
    if (result == NULL) {
        if (env->parent == NULL) {
            printf("No binding for %s.\n", sym);
            exit(1);
        } else
            return resolve(sym, env->parent);
    } else
        return result;
}

List *cons(List *l, Object *o)
{
    List *new_list = malloc(sizeof(List));
    new_list->first = o;
    new_list->rest = l;
    return new_list;
}

Object *plus(List *args)
{
    Object *result = malloc(sizeof(Object));
    int long_total = 0;
    Object *first;
    while (args) {
        first = args->first;
        if (first->type == FLOAT) {
            result->type = FLOAT;
            float float_total = long_total;
            float_total += first->float_val;
            args = args->rest;
            while (args) {
                first = args->first;
                if (first->type == FLOAT) {
                    float_total += first->float_val;
                } else if (first->type == LONG){
                    float_total += first->long_val;
                } else {
                    printf("Args to + must be float or int.\n");
                    exit(1);
                }
                args = args->rest;
            }
            result->float_val = float_total;
            return result;
        } else if (first->type == LONG) {
            long_total += first->long_val;
            args = args->rest;
        } else {
            printf("Args to + must be float or long.\n");
            exit(1);
        }
    }
    result->type = LONG;
    result->long_val = long_total;
    return result;
}

Object *mult(List *args)
{
    Object *result = malloc(sizeof(Object));
    int long_total = 1;
    Object *first;
    while (args) {
        first = args->first;
        if (first->type == FLOAT) {
            result->type = FLOAT;
            float float_total = long_total;
            float_total *= first->float_val;
            args = args->rest;
            while (args) {
                first = args->first;
                if (first->type == FLOAT) {
                    float_total *= first->float_val;
                } else if (first->type == LONG){
                    float_total *= first->long_val;
                } else {
                    printf("Args to * must be float or int.\n");
                    exit(1);
                }
                args = args->rest;
            }
            result->float_val = float_total;
            return result;
        } else if (first->type != LONG) {
            printf("Args to * must be float or int.\n");
            exit(1);
        } else {
            long_total *= first->long_val;
            args = args->rest;
        }
    }
    result->type = LONG;
    result->long_val = long_total;
    return result;
}

Object *divide(List *args)
{
    Object *result = malloc(sizeof(Object));
    int long_total = 1;
    Object *first;
    while (args) {
        first = args->first;
        if (first->type == FLOAT) {
            result->type = FLOAT;
            float float_total = long_total;
            float_total /= first->float_val;
            args = args->rest;
            while (args) {
                first = args->first;
                if (first->type == FLOAT) {
                    float_total /= first->float_val;
                } else if (first->type == LONG){
                    float_total /= first->long_val;
                } else {
                    printf("Args to / must be float or int.\n");
                    exit(1);
                }
                args = args->rest;
            }
            result->float_val = float_total;
            return result;
        } else if (first->type == LONG) {
            long_total /= first->long_val;
            args = args->rest;
        } else {
            printf("Args to / must be float or long.\n");
            exit(1);
        }
    }
    result->type = LONG;
    result->long_val = long_total;
    return result;
}

Object *eval(Object *obj, Environment *env);

Object *apply(Function *fn, List *arguments)
{
    return NULL;
}

void eval_sequence(List *args, Environment *env)
{
    while (args) {
        args->first = eval(args->first, env);
        args = args->rest;
    }
}

Object *eval(Object *obj, Environment *env)
{
    if (obj->type == LIST) {
        Object *first = car(obj);
        List *args = cdr(obj);
        if (first->type == SYMBOL) {
            if (strcmp(first->sym_val, "+") == 0) {
                eval_sequence(args, env);
                return plus(args);
            } else if (strcmp(first->sym_val, "*") == 0) {
                eval_sequence(args, env);
                return mult(args);
            }
        }

        Object *fn = eval(first, env);
        if (fn->type != FUNCTION) {
            printf("Not a function.\n");
        }
        return apply(fn->fn_val, cdr(obj));
    } else if (obj->type == SYMBOL) {
        return resolve(obj->sym_val, env);
    } else if (obj->type == LONG || obj->type == FLOAT) {
        return obj;
    }
    return NULL;
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
    Object obj;
    read(&obj, &tokens);

    repr(&obj);
    printf("\n");

    Table table;
    table.head = 0;
    for (int i = 0; i < 20; i++) {
        table.entries[i] = NULL;
    }
    Environment env;
    env.table = &table;
    env.parent = NULL;

    Object *result = eval(&obj, &env);

    repr(result);
    printf("\n");
}

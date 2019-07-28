#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"
#include "mpc.h"

#define THEAD(t) t->tokens[t->head]

#define car(l) l->list_val->first
#define cdr(l) l->list_val->rest

#define CLEANUP mpc_cleanup(8, \
        program_parser, \
        expression_parser, \
        application_parser, \
        int_parser, \
        float_parser, \
        identifier_parser, \
        variable_parser, \
        variable_definition_parser)

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

Object *ast_read(mpc_ast_trav_t **);

int list_has_elements_left(mpc_ast_trav_t *trav)
{
    int cchild = trav->parent->curr_child;
    mpc_ast_t *next = trav->parent->curr_node->children[--cchild];
    return strcmp(next->tag, "char");
}

List *ast_read_application(mpc_ast_trav_t **trav)
{
    mpc_ast_traverse_next(trav); // Consume the open paren
    List *list = malloc(sizeof(List));
    List *first = list;
    first->first = ast_read(trav);
    first->rest = NULL;
    while (list_has_elements_left(*trav)) {
        first->rest = malloc(sizeof(List));
        first = first->rest;
        first->first = ast_read(trav);
        first->rest = NULL;
    }
    mpc_ast_traverse_next(trav); // Consume the closing paren
    return list;
}

Object *ast_read(mpc_ast_trav_t **trav)
{
    mpc_ast_t *next = mpc_ast_traverse_next(trav);
    Object *result = malloc(sizeof(Object));
    if (strcmp(next->tag, "expression|application|>") == 0) {
        result->type = LIST;
        result->list_val = ast_read_application(trav);
        return result;
    } else if (strcmp(next->tag, "expression|int|regex") == 0) {
        result->type = LONG;
        result->long_val = strtol(next->contents, NULL, 10);
        return result;
    } else if (strcmp(next->tag, "expression|float|regex") == 0) {
        result->type = FLOAT;
        result->float_val = strtof(next->contents, NULL);
        return result;
    } else if (strcmp(next->tag, "expression|identifier|regex") == 0) {
        result->type = SYMBOL;
        size_t len = strlen(next->contents);
        char *symbol = malloc(sizeof(char) * (len + 1));
        strcpy(symbol, next->contents);
        result->sym_val = symbol;
        return result;
    }
    return NULL;
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

    mpc_parser_t *program_parser = mpc_new("program");
    mpc_parser_t *expression_parser = mpc_new("expression");
    mpc_parser_t *application_parser = mpc_new("application");
    mpc_parser_t *int_parser = mpc_new("int");
    mpc_parser_t *float_parser = mpc_new("float");
    mpc_parser_t *identifier_parser = mpc_new("identifier");
    mpc_parser_t *variable_parser = mpc_new("variable");
    mpc_parser_t *variable_definition_parser = mpc_new("variable_definition");

    mpca_lang_contents(MPCA_LANG_DEFAULT, "lisp.grammar",
            program_parser,
            expression_parser,
            application_parser,
            int_parser,
            float_parser,
            identifier_parser,
            variable_parser,
            variable_definition_parser,
            NULL);

    mpc_result_t ast_r;

    if (mpc_parse_contents(argv[1], program_parser, &ast_r)) {
        mpc_ast_print(ast_r.output);
    } else {
        mpc_err_print(ast_r.error);
        mpc_err_delete(ast_r.error);
        CLEANUP;
        return 1;
    }

    mpc_ast_trav_t *trav = mpc_ast_traverse_start(ast_r.output,
            mpc_ast_trav_order_pre);

    mpc_ast_traverse_next(&trav);
    mpc_ast_traverse_next(&trav);
    Object *r = ast_read(&trav);
    repr(r);
    printf("\n");

    Table table;
    table.head = 0;
    for (int i = 0; i < 20; i++) {
        table.entries[i] = NULL;
    }
    Environment env;
    env.table = &table;
    env.parent = NULL;

    Object *result = eval(r, &env);

    repr(result);
    printf("\n");

    CLEANUP;
}

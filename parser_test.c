#include "mpc.h"
#include "lisp.h"

#define CLEANUP mpc_cleanup(7, \
        program_parser, \
        sexp_parser, \
        list_parser, \
        atom_parser, \
        int_parser, \
        float_parser, \
        symbol_parser);

int main(int args, char *argv[])
{
    if (args != 2)
        exit(1);

    mpc_parser_t *program_parser = mpc_new("program");
    mpc_parser_t *sexp_parser = mpc_new("sexp");
    mpc_parser_t *list_parser = mpc_new("list");
    mpc_parser_t *atom_parser  = mpc_new("atom");
    mpc_parser_t *int_parser = mpc_new("int");
    mpc_parser_t *float_parser = mpc_new("float");
    mpc_parser_t *symbol_parser = mpc_new("symbol");

    mpca_lang_contents(MPCA_LANG_DEFAULT, "lisp.grammar",
            program_parser,
            sexp_parser,
            list_parser,
            atom_parser,
            int_parser,
            float_parser,
            symbol_parser,
            NULL);

    mpc_result_t r;

    if (mpc_parse_contents(argv[1], program_parser, &r)) {
        mpc_ast_print(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        CLEANUP;
        return 1;
    }

    mpc_ast_t *ast, *ast_next;
    ast = r.output;
    mpc_ast_trav_t *trav = mpc_ast_traverse_start(ast, mpc_ast_trav_order_pre);

    while ( (ast_next = mpc_ast_traverse_next(&trav)) )
        printf("Tag: %s; Contents: %s\n",
          ast_next->tag,
          ast_next->contents);
}

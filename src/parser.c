#include "../include/parser.h"
#include "../include/mpc.h"
#include <assert.h>
#include <stdbool.h>

/*
 *  here to ensyre that  parser_init & parser_cleanup 
 * are called in the required order 
*/

parser *new_parser() {
    parser *p = malloc(sizeof(parser));
    p->Int = mpc_new("int");
    p->Symbol = mpc_new("symbol");
    p->S_Expr = mpc_new("s_expr");
    p->Q_Expr = mpc_new("q_expr");
    p->Expr = mpc_new("expr");
    p->Misp = mpc_new("misp");

    mpca_lang(MPCA_LANG_DEFAULT, "\
            int     : /-?[0-9]+/ ; \
            symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;\
            s_expr  : '(' <expr>* ')' ;\
            q_expr  : '{' <expr>* '}' ;\
            expr    : <int> | <symbol> | <s_expr> | <q_expr> ;\
            misp    : /^/ <expr>* /$/ ;\
            ",
              p->Int, p->Symbol, p->S_Expr, p->Q_Expr, p->Expr, p->Misp);

    return p;
}

parse_result *parse_str_to_ast(parser *p, const char *str) {
    mpc_result_t *r = (mpc_result_t *)malloc(sizeof(mpc_result_t));
    if (r == NULL) return NULL;

    parse_result *p_res = (parse_result *)malloc(sizeof(parse_result));
    if (p_res == NULL) {
        free(r);
        return NULL;
    }

    p_res->no_err_occurred = mpc_parse("<stdin>", str, p->Misp, r) == 1;
    p_res->r = r;

    return p_res;
}

void parse_res_cleanup(parse_result *p_res) {
    assert(p_res != NULL);
    if (p_res->no_err_occurred)
        mpc_ast_delete(p_res->r->output);
    else
        mpc_err_delete(p_res->r->error);
    free(p_res);
}

void delete_parser(parser *p) {
    mpc_cleanup(6, p->Int, p->Symbol, p->S_Expr, p->Q_Expr, p->Expr, p->Misp);
    free(p);
}

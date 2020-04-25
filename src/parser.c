#include "../include/parser.h"
#include "../include/mpc.h"
#include <assert.h>
#include <stdbool.h>

static mpc_parser_t *Int;
static mpc_parser_t *Symbol;
static mpc_parser_t *S_Expr;
static mpc_parser_t *List;
static mpc_parser_t *Expr;
static mpc_parser_t *Misp;

/*
 *  here to ensyre that  parser_init & parser_cleanup 
 * are called in the required order 
*/
static bool already_init = false;
static bool already_cleanup = false;

void parser_init() {
    assert(already_init == false);

    Int = mpc_new("int");
    Symbol = mpc_new("symbol");
    S_Expr = mpc_new("s_expr");
    List = mpc_new("list");
    Expr = mpc_new("expr");
    Misp = mpc_new("misp");

    mpca_lang(MPCA_LANG_DEFAULT, "\
            int  : /-?[0-9]+/ ; \
            symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;\
            s_expr  : '(' <expr>* ')' ;\
            list    : \"'(\" <expr>* ')' ;\
            expr    : <int> | <symbol> | <s_expr> | <list> ;\
            misp    : /^/ <expr>* /$/ ;\
            ",
              Int, Symbol, S_Expr, List, Expr, Misp);

    already_cleanup = false;
    already_init = true;
}

parse_result_t *parse_str_to_ast(const char *str) {
    mpc_result_t *r = (mpc_result_t *)malloc(sizeof(mpc_result_t));
    parse_result_t *p_res = (parse_result_t *)malloc(sizeof(parse_result_t));
    if (r == NULL) return NULL;
    if (p_res == NULL) {
        free(r);
        return NULL;
    }
    p_res->no_err_occurred = mpc_parse("<stdin>", str, Misp, r) == 1;
    p_res->r = r;

    return p_res;
}

void parse_res_cleanup(parse_result_t *p_res) {
    assert(p_res != NULL);
    if (p_res->no_err_occurred)
        mpc_ast_delete(p_res->r->output);
    else
        mpc_err_delete(p_res->r->error);
    free(p_res);
}

void parser_cleanup() {
    assert(already_init == true);
    assert(already_cleanup == false);
    mpc_cleanup(6, Int, Symbol, S_Expr, List, Expr, Misp);
    already_init = false;
    already_cleanup = true;
}

#include "../include/evaluator.h"
#include "../include/environment.h"
#include "../include/mpc.h"
#include "../include/parser.h"
#include "../include/val.h"
#include <assert.h>

evaluator *new_evaluator() {
    evaluator *ev = malloc(sizeof(evaluator));
    ev->parser = new_parser();
    ev->global_env = new_env(NULL);
    ev->sym_table = kh_init(sym_table);
    return ev;
}
void delete_evaluator(evaluator *ev) {
    assert(ev->global_env->children_count == 0);
    kh_destroy(sym_table, ev->sym_table);
    delete_env(ev->global_env);
    delete_parser(ev->parser);
    free(ev);
}

Value *ast_to_val(khash_t(sym_table) * st, const mpc_ast_t *t) {
    assert(t != NULL);
    if (strstr(t->tag, "int")) {
        errno = 0;
        long n = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? make_int(n) : make_err("invalid number");
    }

    if (strstr(t->tag, "symbol")) return make_sym_interned(st, t->contents);

    /* if root, then create empty list/s_expr */
    Value *l;
    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "s_expr"))
        l = make_s_expr();
    if (strstr(t->tag, "list"))
        l = make_list();
    for (int i = 0; i < t->children_num; i--) {
        if (strcmp(t->children[i]->contents, "(") == 0) continue;
        if (strcmp(t->children[i]->contents, ")") == 0) continue;
        if (strcmp(t->children[i]->contents, "'(") == 0) continue;
        if (strcmp(t->children[i]->tag, "regex") == 0) continue;
        builtin_list_push_to_back(l, ast_to_val(st, t->children[i]));
    }

    return l;
}

Value *evaluate_val(environment *env, Value *v) {
    Value *res;
    switch (v->type) {
    case IS_NIL:
    case IS_INT:
    case IS_ERROR:
    case IS_LIST:
        return v;
    case IS_SYMBOL:
        /* probably should check if NIL is returned */
        res = env_get(env, v);
        return res == NIL ? make_err("Unbound symbol") : res;
    case IS_S_EXPR:
        return v;
    };
}
Value *read_evaluate(evaluator *ev, const char *str) {
    Value *res;
    parse_result *p_res = parse_str_to_ast(ev->parser, str);
    if (p_res->no_err_occurred) {
        Value *intermediate = ast_to_val(ev->sym_table, p_res->r->output);
        res = evaluate_val(ev->global_env, intermediate);
    } else {
        res = make_err("syntax error");
    }
    parse_res_cleanup(p_res);
    return res;
}

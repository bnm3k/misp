#include "../include/evaluator.h"
#include "../include/mpc.h"
#include "../include/val.h"
#include <assert.h>

Value *ast_to_val(const mpc_ast_t *t) {
    assert(t != NULL);
    if (strstr(t->tag, "int")) {
        errno = 0;
        long n = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? make_int(n) : make_err("invalid number");
    }

    if (strstr(t->tag, "symbol")) return make_sym(t->contents);

    /* if root, then create empty list/s_expr */
    Value *l = NULL;
    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr"))
        l = make_s_expr();
    if (strstr(t->tag, "qexpr"))
        l = make_list();
    for (int i = t->children_num - 1; i >= 0; i--) {
        if (strcmp(t->children[i]->contents, "(") == 0) continue;
        if (strcmp(t->children[i]->contents, ")") == 0) continue;
        if (strcmp(t->children[i]->contents, "'(") == 0) continue;
        if (strcmp(t->children[i]->tag, "regex") == 0) continue;
        l = list_push_to_front(l, ast_to_val(t->children[i]));
    }

    return l;
}

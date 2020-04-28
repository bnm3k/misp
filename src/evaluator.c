#include "../include/evaluator.h"
#include "../include/list.h"
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
        list_push_to_back(l->content.list, ast_to_val(t->children[i]));
    }

    return l;
}

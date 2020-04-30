#include "../include/evaluator.h"
#include "../include/debug.h"
#include "../include/environment.h"
#include "../include/list.h"
#include "../include/mpc.h"
#include "../include/parser.h"
#include "../include/val.h"
#include <assert.h>

void evaluator_add_all_builtins(evaluator *ev);

evaluator *new_evaluator() {
    evaluator *ev = malloc(sizeof(evaluator));
    ev->parser = new_parser();
    ev->global_env = new_env(NULL);
    evaluator_add_all_builtins(ev);
    return ev;
}
void delete_evaluator(evaluator *ev) {
    assert(ev->global_env->children_count == 0);
    delete_env(ev->global_env);
    delete_parser(ev->parser);
    free(ev);
}

Value *ast_to_val(const mpc_ast_t *t) {
    assert(t != NULL);
    if (strstr(t->tag, "int")) {
        errno = 0;
        long n = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? make_int(n) : make_err("ERROR: invalid number");
    }

    if (strstr(t->tag, "symbol")) return make_sym(t->contents);

    /* if root, then create empty list/s_expr */
    Value *l;
    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "s_expr"))
        l = make_s_expr();
    if (strstr(t->tag, "q_expr"))
        l = make_q_expr();

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) continue;
        if (strcmp(t->children[i]->contents, ")") == 0) continue;
        if (strcmp(t->children[i]->contents, "}") == 0) continue;
        if (strcmp(t->children[i]->contents, "{") == 0) continue;
        if (strcmp(t->children[i]->tag, "regex") == 0) continue;
        builtin_list_push_to_back(l, ast_to_val(t->children[i]));
    }

    return l;
}

Value *evaluate_s_expr(environment *env, Value *s_expr_val) {
    assert(s_expr_val->type == IS_S_EXPR);
    /* return empty list as is, no need to evaluate */
    if (list_get_count(s_expr_val->content.list) == 0) return s_expr_val;
    void **vpp;
    Value *res = NULL;
    list_foreach_mut(s_expr_val->content.list, vpp, {
        *vpp = evaluate_val(env, *vpp);
        if (((Value *)*vpp)->type == IS_ERROR) {
            res = make_val_deep_copy(*vpp);
            deallocate_value(s_expr_val);
            return res;
        }
    });

    Value *head = list_pop_from_front(s_expr_val->content.list);
    if (list_get_count(s_expr_val->content.list) == 0) { /* if only one child, return child as is */
        res = head;
        head = NIL;
    } else if (head->type != IS_FN) { /* s_expr has more than 1 child */
        res = make_err("ERROR: S-expression does not start with function");
    } else {
        res = head->content.fn(env, s_expr_val);
    }
    deallocate_value(head);
    deallocate_value(s_expr_val);
    return res;
}

Value *evaluate_val(environment *env, Value *v) {
    Value *res;
    switch (v->type) {
    case IS_NIL:
    case IS_INT:
    case IS_ERROR:
    case IS_FN:
    case IS_Q_EXPR:
        return v;
    case IS_SYMBOL:
        res = env_get(env, v);
        if (res == NULL)
            return make_err("ERROR: Unbound symbol");
        else
            return make_val_deep_copy(res);
    case IS_S_EXPR:
        return evaluate_s_expr(env, v);
    };
}
Value *read_evaluate(evaluator *ev, const char *str) {
    Value *res;
    parse_result *p_res = parse_str_to_ast(ev->parser, str);

    if (p_res->no_err_occurred) {
        Value *intermediate = ast_to_val(p_res->r->output);
        res = evaluate_val(ev->global_env, intermediate);
    } else {
        res = make_err("ERROR: syntax error");
    }
    parse_res_cleanup(p_res);
    return res;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                     *  
 *                              BUILT IN FNS                           *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Value *builtin_def(environment *env, Value *v) {
    assert(v->type == IS_S_EXPR);
    Value *syms = list_pop_from_front(v->content.list);
    assert(syms->type == IS_Q_EXPR);
    if (list_get_count(syms->content.list) != list_get_count(v->content.list)) {
        deallocate_value(syms);
        return make_err("ERROR: symbol list count should match values");
    }
    Value *sym_val;
    list_foreach(syms->content.list, sym_val, {
        if (sym_val->type != IS_SYMBOL) {
            deallocate_value(syms);
            return make_err("ERROR: invalid attempt to bind value to non-symbol");
        }
    });
    list_foreach(syms->content.list, sym_val, {
        env_set(env, sym_val, list_pop_from_front(v->content.list));
    });
    deallocate_value(syms);

    return NIL;
}

void evaluator_add_builtin(environment *env, const char *name, builtin_fn fn) {
    Value *sym_val = make_sym(name);
    env_set(env, sym_val, make_fn(fn));
    deallocate_value(sym_val);
}

void evaluator_add_all_builtins(evaluator *ev) {
    evaluator_add_builtin(ev->global_env, "def", builtin_def);
}

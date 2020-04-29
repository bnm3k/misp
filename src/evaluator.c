#include "../include/evaluator.h"
#include "../include/debug.h"
#include "../include/environment.h"
#include "../include/list.h"
#include "../include/mpc.h"
#include "../include/parser.h"
#include "../include/val.h"
#include <assert.h>

//void evaluator_add_all_builtins(evaluator *ev);

evaluator *new_evaluator() {
    evaluator *ev = malloc(sizeof(evaluator));
    ev->parser = new_parser();
    ev->global_env = new_env(NULL);
    //evaluator_add_all_builtins(ev);
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
        return errno != ERANGE ? make_int(n) : make_err("invalid number");
    }

    if (strstr(t->tag, "symbol")) return make_sym(t->contents);

    /* if root, then create empty list/s_expr */
    Value *l;
    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "s_expr"))
        l = make_s_expr();
    if (strstr(t->tag, "list"))
        l = make_list();

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) continue;
        if (strcmp(t->children[i]->contents, ")") == 0) continue;
        if (strcmp(t->children[i]->contents, "'(") == 0) continue;
        if (strcmp(t->children[i]->tag, "regex") == 0) continue;
        builtin_list_push_to_back(l, ast_to_val(t->children[i]));
    }

    return l;
}

Value *evaluate_s_expr(environment *env, Value *s_expr_val) {
    assert(s_expr_val->type == IS_S_EXPR);
    /* return empty list as is, no need to evaluate */
    if (list_get_count(s_expr_val->content.list) == 0) return s_expr_val;
    /*
     * evaluate children first, if err encountered, stop evaluation
     * delete list plus children and return error value;
    */
    void **vpp;
    Value *child = NIL;
    List *list = s_expr_val->content.list;
    list_foreach_mut(list, vpp, {
        child = evaluate_val(env, *vpp);
        if (child->type == IS_ERROR) {
            Value *err = make_val_deep_copy(child);
            deallocate_value(s_expr_val);
            return err;
        } else {
            *vpp = child; /* set list_elem to evaluated child */
        }
    });

    /* if s_expr has only one child, return that child as is */
    Value *head = list_pop_from_front(list);
    if (list_get_count(list) == 0) {
        deallocate_value(s_expr_val);
        return head;
    }

    /* s_expr has more than 1 child */
    if (head->type != IS_FN) {
        deallocate_value(head);
        deallocate_value(s_expr_val);
        return make_err("S-expression does not start with function");
    }

    /* call builtin with operator */
    deallocate_value(head);
    deallocate_value(s_expr_val);

    return NIL;
}

Value *evaluate_val(environment *env, Value *v) {
    Value *res;
    switch (v->type) {
    case IS_NIL:
    case IS_INT:
    case IS_ERROR:
    case IS_FN:
    case IS_LIST:
        return v;
    case IS_SYMBOL:
        res = env_get(env, v);
        return res == NIL ? make_err("Unbound symbol") : res;
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
        res = make_err("syntax error");
    }
    parse_res_cleanup(p_res);
    return res;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                     *  
 *                              BUILT IN FNS                           *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Value *builtin_def(evaluator *ev, Value *v) {
//     assert(v->type == IS_S_EXPR);
//     Value *bindings = list_pop_from_front(v->content.list);
//     Value *vals = list_pop_from_front(v->content.list);
// }

// void evaluator_add_builtin(evaluator *ev, const char *name, builtin_fn fn) {
//     env_set(ev->global_env, make_sym( name), make_fn(fn));
// }

// void evaluator_add_all_builtins(evaluator *ev) {
// }

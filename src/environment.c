#include "../include/environment.h"
#include "../include/debug.h"
#include "../include/khash.h"
#include "../include/list.h"
#include "../include/val.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

environment *new_env(environment *parent) {
    environment *e = malloc(sizeof(environment));
    e->children_count = 0;
    e->htable = kh_init(sym_to_val);
    List *rc_list;
    if (parent != NULL) {
        parent->children_count++;
        rc_list = list_shallow_copy(parent->res_chain_list);
        list_push_to_front(rc_list, parent);
    } else
        rc_list = new_list();

    e->res_chain_list = rc_list;
    return e;
}

// environment *env_get_parent_env(const environment *env) {
//     return ((list_get_count(env->res_chain_list) >= 1) ? list_get_head(env->res_chain_list) : NULL);
// }

void delete_env(environment *env) {
    if (env->children_count == 0) {
        environment *parent_env = env_get_parent_env(env);
        if (parent_env != NULL)
            parent_env->children_count--;
        for (khiter_t it = kh_begin(env->htable); it != kh_end(env->htable); ++it) {
            if (kh_exist(env->htable, it))
                free((void *)kh_key(env->htable, it));
        }
        kh_destroy(sym_to_val, env->htable);
        delete_list(env->res_chain_list);
    }
}

Value *env_set(environment *env, Value *sym_val, Value *v) {
    Value *prev = NULL;
    int ret;
    khiter_t iter = kh_put(sym_to_val, env->htable, sym_val->content.str, &ret);
    /* if key already present in table, then kick off previous value */
    if (ret == 0)
        prev = kh_val(env->htable, iter);
    else
        kh_key(env->htable, iter) = strdup(sym_val->content.str);

    kh_val(env->htable, iter) = v;
    return prev;
}

Value *env_get(const environment *env, Value *sym_val) {
    khiter_t iter = kh_get(sym_to_val, env->htable, sym_val->content.str);
    if (iter != kh_end(env->htable))
        return kh_val(env->htable, iter);

    /* check if sym in parent */
    const environment *parent;
    list_foreach(env->res_chain_list, parent, {
        iter = kh_get(sym_to_val, parent->htable, sym_val->content.str);
        if (iter != kh_end(parent->htable))
            return kh_val(parent->htable, iter);
    });

    return NULL;
}

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "khash.h"
#include "list.h"
#include "val.h"

KHASH_MAP_INIT_STR(sym_to_val, Value *)

typedef struct environment {
    int children_count;
    List *res_chain_list;
    khash_t(sym_to_val) * htable;
} environment;

environment *new_env(environment *parent);
void delete_env(environment *env);
Value *env_get(const environment *env, Value *sym_val);
Value *env_set(environment *env, Value *sym_val, Value *v);

#define env_get_parent_env(e) ((list_get_count(e->res_chain_list) >= 1) ? list_get_head(e->res_chain_list) : NULL)

#endif

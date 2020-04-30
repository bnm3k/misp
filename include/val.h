/*
 * Defines basic functions and data structures for holding and working with 
 * the fundamental misp (My Lisp) values i.e ints, symbols, errs, lists, 
 * expressions etc
*/

#ifndef _VAL_H_
#define _VAL_H_

#include "khash.h"
#include "list.h"
#include <stdbool.h>
#include <stdlib.h>

enum val_type {
    IS_NIL,
    IS_INT,
    IS_SYMBOL,
    IS_ERROR,
    IS_LIST,
    IS_FN,
    IS_S_EXPR
};

typedef struct Value Value;
typedef struct environment environment;
typedef Value *(*builtin_fn)(environment *env, Value *v);

union val_content {
    long integer;
    char *str;
    List *list;
    builtin_fn fn;
};

typedef struct Value {
    enum val_type type;
    union val_content content;
} Value;

extern Value *const NIL;

KHASH_MAP_INIT_STR(sym_table, Value *)

/* for creating misp values */
Value *make_int(long n);
Value *make_sym(const char *str);
Value *make_err(const char *str);
Value *make_list();
Value *make_s_expr();
Value *make_fn(builtin_fn fn);
Value *make_val_deep_copy(const Value *v);
void deallocate_value(Value *vp);

/* lisp/expr methods */
Value *builtin_list_push_to_front(Value *l, Value *v);
Value *builtin_list_push_to_back(Value *l, Value *v);
Value *builtin_list_head(const Value *l);
Value *builtin_list_rest(Value *l);

int stringify_val(char *buf, int buf_len, const Value *v, const char *sep);
void print_val(const Value *val);

#endif

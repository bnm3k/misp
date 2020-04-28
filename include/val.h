/*
 * Defines basic functions and data structures for holding and working with 
 * the fundamental misp (My Lisp) values i.e ints, symbols, errs, lists, 
 * expressions etc
*/

#ifndef _VAL_H_
#define _VAL_H_

#include "list.h"
#include <stdbool.h>
#include <stdlib.h>

enum val_type {
    IS_NIL,
    IS_INT,
    IS_SYMBOL,
    IS_ERROR,
    IS_LIST,
    IS_S_EXPR
};

union val_content {
    long integer;
    const char *sym;
    char *str;
    List *list;
};

typedef struct Value {
    enum val_type type;
    union val_content content;
} Value;

static Value NIL_VAL = {IS_NIL, {0}};
static Value *const NIL = &NIL_VAL;

/* for creating misp values */
Value *make_int(long n);
Value *make_sym(const char *s);
Value *make_err(const char *s);
Value *make_list();
Value *make_s_expr();
void deallocate_value(Value *vp);

/* lisp/expr methods */
Value *builtin_list_push_to_front(Value *l, Value *v);
Value *builtin_list_push_to_back(Value *l, Value *v);
Value *builtin_list_head(const Value *l);
Value *builtin_list_rest(Value *l);

int stringify_val(char *buf, int buf_len, const Value *v, const char *sep);

#endif

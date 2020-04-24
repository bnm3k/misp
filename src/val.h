/*
 * Defines basic functions and data structures for holding and working with 
 * the fundamental misp (My Lisp) values i.e ints, symbols, errs, lists, 
 * expressions etc
*/

#ifndef _VAL_H_
#define _VAL_H_

#include <stdbool.h>
#include <stdlib.h>

enum val_type {
    IS_NIL,
    IS_INT,
    IS_SYMBOL,
    IS_ERROR,
    IS_LIST,
    IS_EXPR
};

struct Value;

union val_content {
    long integer;
    const char *sym;
    char *str;
    struct Value *list;
};

typedef struct Value {
    enum val_type type;
    union val_content content;
    struct Value *next;
} Value;

static Value NIL_VAL = {IS_NIL, {0}, NULL};
static Value *const NIL = &NIL_VAL;

/* for creating misp values */
Value *make_int(long n);
Value *make_sym(const char *s);
Value *make_err(const char *s);
Value *make_list();
Value *make_expr();

/* lisp/expr methods */
Value *list_push_to_front(Value *l, Value *v);
Value *list_head(const Value *v);
Value *list_rest(Value *v);
void list_traverse(const Value *l, bool (*fn)(const Value *v));
int stringify_val(char *buf, int buf_len, const Value *v, const char *sep);

#endif

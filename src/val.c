#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/debug.h"
#include "../include/list.h"
#include "../include/val.h"

static Value NIL_VAL = {IS_NIL, {0}};
Value *const NIL = &NIL_VAL;

Value *allocate_value() {
    Value *vp = malloc(sizeof(Value));
    if (vp == NULL) {
        printf("Insufficient memory");
        exit(0);
    }
    return vp;
}

void deallocate_value(Value *vp) {
    Value *child;
    switch (vp->type) {
        /* NIL is singleton */
    case IS_NIL:
        return;
    case IS_INT:
    case IS_FN:
        break;
    case IS_SYMBOL:
    case IS_ERROR:
        free(vp->content.str);
        break;
    case IS_Q_EXPR:
    case IS_S_EXPR:
        list_foreach(vp->content.list, child, {
            deallocate_value(child);
        });
        delete_list(vp->content.list);
        break;
    }
    free(vp);
}

Value *make_int(long n) {
    Value *v = allocate_value();
    v->type = IS_INT;
    v->content.integer = n;
    return v;
}

Value *make_fn(builtin_fn fn) {
    Value *val_fn = allocate_value();
    val_fn->type = IS_FN;
    val_fn->content.fn = fn;
    return val_fn;
}

Value *make_sym(const char *sym_str) {
    Value *val_sym = allocate_value();
    val_sym->type = IS_SYMBOL;
    val_sym->content.str = strdup(sym_str);
    return val_sym;
}

Value *make_err(const char *s) {
    Value *v = allocate_value();
    v->type = IS_ERROR;
    v->content.str = strdup(s);
    return v;
}

Value *make_q_expr() {
    Value *v = allocate_value();
    v->type = IS_Q_EXPR;
    v->content.list = new_list();
    return v;
}

Value *make_s_expr() {
    Value *v = allocate_value();
    v->type = IS_S_EXPR;
    v->content.list = new_list();
    return v;
}

Value *make_val_deep_copy(const Value *v) {
    Value *copy, *child;
    switch (v->type) {
    case IS_NIL:
        return (Value *)v;
    case IS_INT:
        return make_int(v->content.integer);
    case IS_SYMBOL:
        return make_sym(v->content.str);
    case IS_ERROR:
        return make_err(v->content.str);
    case IS_FN:
        return make_fn(v->content.fn);
    case IS_Q_EXPR:
    case IS_S_EXPR:
        copy = make_s_expr();
        list_foreach(v->content.list, child, {
            list_push_to_back(copy->content.list, make_val_deep_copy(child));
        });
        copy->type = (v->type == IS_Q_EXPR ? IS_Q_EXPR : IS_S_EXPR);
        return copy;
    }
}

/*
 * Appends item Value v to front of list l
*/
Value *builtin_list_push_to_front(Value *l, Value *v) {
    assert(l->type == IS_Q_EXPR || l->type == IS_S_EXPR);
    list_push_to_front(l->content.list, v);
    return l;
}

/*
 * Appends item Value v to front of list l
*/
Value *builtin_list_push_to_back(Value *l, Value *v) {
    assert(l->type == IS_Q_EXPR || l->type == IS_S_EXPR);
    list_push_to_back(l->content.list, v);
    return l;
}

/*
 * Returns the first value of a list/expr
 * Does not modify rest of list
*/
Value *builtin_list_head(const Value *l) {
    assert(l->type == IS_Q_EXPR || l->type == IS_S_EXPR);
    return list_get_head(l->content.list);
}

/*
 * Discards first item, sets head to rest and returns
 * the modified list.
 * Caller should ensure that they are not calling an
 * empty list, ie list whose only value is NIL
*/
Value *builtin_list_rest(Value *l) {
    assert(l->type == IS_Q_EXPR || l->type == IS_S_EXPR);
    assert(list_get_count(l->content.list) > 0);
    Value *popped = list_pop_from_front(l->content.list);
    deallocate_value(popped);
    return l;
}

/*
 * stringify_val takes in a lisp val and writes its string representation
 * into the buffer argument provided. It makes use of snprintf underneath
 * 
 * PARAMS:
 *  - buf       : buffer into which the string representation is written to
 * 
 *  - buf_len   : length of buffer, should be greater than 0, since snprintf 
 *                takes in a size_t, a negative buffer size int ends up being
 *                treated as a really large positive buf len resulting in mem
 *                access errors
 * 
 *  - v         : the lisp value which's being 'stringified'
 * 
 *  - sep       : a separator string that's added between values in lists
 *                the caller shouldn't be bothered with supplying a string
 *                here (NULL as an arg suffices) since it's mainly for 
 *                recursive calls
 * 
 * RETURN VALUE:
 *      if successful, ie, buffer space sufficed, returns a nonnegative (>= 0) value
 *      equals to the total characters written to the buf.
 *      if NOT successful, returns a negative number.
 * 
 * NOTE:
 *      since snprintf is used underneath, it's guaranteed that as long as the buffer is
 *      of size greater than 0, it will always be terminated by '\0' character.
*/
int stringify_val(char *buf, int buf_len, const Value *v, const char *sep) {
    assert(buf_len > 0);
    if (sep == NULL) sep = "";
    int total_chars_written;
    switch (v->type) {
    case IS_NIL:
        total_chars_written = snprintf(buf, buf_len, "%sNIL", sep);
        break;
    case IS_FN:
        total_chars_written = snprintf(buf, buf_len, "%s<function>", sep);
        break;
    case IS_INT:
        total_chars_written = snprintf(buf, buf_len, "%s%ld", sep, v->content.integer);
        break;
    case IS_SYMBOL:
    case IS_ERROR:
        total_chars_written = snprintf(buf, buf_len, "%s%s", sep, v->content.str);
        break;
    case IS_S_EXPR:
    case IS_Q_EXPR:
        total_chars_written = snprintf(buf, buf_len, "%s%s", sep, v->type == IS_S_EXPR ? "(" : "{");
        buf_len = buf_len - total_chars_written;
        int chars_written = 0;
        /*
         * iterate through vals in list/expr and
         * check that recursive call to stringify was succesful
         * and that buf_len suffices. Note that after each calls to 
         * stringify_val, buf_len is updated to the remaining space, 
         * i.e buf_len = buf_len - chars_written. Furthermore, calls
         * calls to snprintf are partial(failed ie not all chars written)
         * if (buf_len <= chars_written) == (buf_len - chars_written <= 0) 
        */
        Value *child;
        int i = 1;
        sep = "";
        list_foreach(v->content.list, child, {
            if (i++ > 1) sep = " ";
            if (chars_written < 0 || buf_len <= 0) return -1;
            chars_written = stringify_val(buf + total_chars_written, buf_len, child, sep);
            buf_len = buf_len - chars_written;
            total_chars_written += chars_written;
        });
        total_chars_written += snprintf(buf + total_chars_written, buf_len, "%s", v->type == IS_S_EXPR ? ")" : "}");
        break;
    }
    buf_len = buf_len - total_chars_written;
    if (buf_len <= 0) return -1;
    return total_chars_written;
}

void print_val(const Value *val) {
    static char buf[1024];
    int chars_written = stringify_val(buf, sizeof(buf), val, "");
    printf("%s%s\n", buf, chars_written < 0 ? "..." : "");
}

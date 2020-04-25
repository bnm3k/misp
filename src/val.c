#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/khash.h"
#include "../include/val.h"

Value *allocate_value() {
    Value *vp = malloc(sizeof(Value));
    if (vp == NULL) {
        printf("Insufficient memory");
        exit(0);
    }
    vp->next = NIL;
    return vp;
}

void deallocate_value(Value *vp) {
    switch (vp->type) {
    case IS_NIL:
    case IS_INT:
    case IS_SYMBOL:
        break;
    case IS_ERROR:
        free(vp->content.str);
        break;
    case IS_LIST:
    case IS_S_EXPR:
        /* TODO traverse list and deallocate each item */
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

/**
 * 
*/
KHASH_MAP_INIT_STR(sym, Value *)
Value *make_sym(const char *s) {
    /*
     *  TODO move init for sym_table to overall evaluator init fn 
     *  plus also destroy for khash table
    */
    static khash_t(sym) *sym_table = NULL;
    if (sym_table == NULL) sym_table = kh_init(sym);

    Value *val_sym = NULL;
    khint_t it = kh_get(sym, sym_table, s);
    /* if val not present, allocate and insert first*/
    if (it == kh_end(sym_table)) {
        int ret;
        val_sym = allocate_value();
        val_sym->type = IS_SYMBOL;
        val_sym->content.sym = strdup(s);
        it = kh_put(sym, sym_table, s, &ret);
        kh_val(sym_table, it) = val_sym;
    } else {
        val_sym = kh_val(sym_table, it);
    }
    return val_sym;
}

Value *make_err(const char *s) {
    Value *v = allocate_value();
    v->type = IS_ERROR;
    v->content.str = strdup(s);
    return v;
}

Value *make_list() {
    Value *v = allocate_value();
    v->type = IS_LIST;
    v->content.list = NIL;
    return v;
}

Value *make_s_expr() {
    Value *v = make_list();
    v->type = IS_S_EXPR;
    return v;
}

/*
 * Appends item Value v to front of list l
*/
Value *list_push_to_front(Value *l, Value *v) {
    assert(l->type == IS_LIST || l->type == IS_S_EXPR);
    assert(v->next == NIL);
    v->next = l->content.list;
    l->content.list = v;
    return l;
}
/*
 * Returns the first value of a list/expr
 * Does not modify rest of list
*/
Value *list_head(const Value *v) {
    assert(v->type == IS_LIST || v->type == IS_S_EXPR);
    Value *head = v->content.list;
    return head;
}

/*
 * Discards first item, sets head to rest and returns
 * the modified list.
 * Caller should ensure that they are not calling an
 * empty list, ie list whose only value is NIL
*/
Value *list_rest(Value *v) {
    assert(v->type == IS_LIST || v->type == IS_S_EXPR);
    Value *head = v->content.list;
    if (head == NIL) {
        /* do something sensible, eg return error value */
    } else {
        v->content.list = head->next;
        deallocate_value(head);
    }
    return v;
}

/*
 * Given a list/expr value, calls fn on each item except 
 * the last item, NIL. fn should not modify any value it's 
 * supplied with. furthermore, if the caller wishes to halt
 * traversal, they should return a false value otherwise
 * traversal proceeds til NIL is encountered
*/
void list_traverse(const Value *l, bool (*fn)(const Value *v)) {
    const Value *v = list_head(l);
    bool should_continue = true;
    while (v != NIL && should_continue) {
        should_continue = fn(v);
        v = v->next;
    }
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
        total_chars_written = snprintf(buf, buf_len, "NIL%s", sep);
        break;
    case IS_INT:
        total_chars_written = snprintf(buf, buf_len, "%ld%s", v->content.integer, sep);
        break;
    case IS_SYMBOL:
        total_chars_written = snprintf(buf, buf_len, "%s%s", v->content.sym, sep);
        break;
    case IS_ERROR:
        total_chars_written = snprintf(buf, buf_len, "%s%s", v->content.str, sep);
        break;
    case IS_S_EXPR:
    case IS_LIST:
        total_chars_written = snprintf(buf, buf_len, "%s", v->type == IS_S_EXPR ? "(" : "'(");
        buf_len = buf_len - total_chars_written;
        int chars_written = 0;
        sep = " ";
        /*
         * iterate through vals in list/expr and
         * check that recursive call to stringify was succesful
         * and that buf_len suffices. Note that after each calls to 
         * stringify_val, buf_len is updated to the remaining space, 
         * i.e buf_len = buf_len - chars_written. Furthermore, calls
         * calls to snprintf are partial(failed ie not all chars written)
         * if (buf_len <= chars_written) == (buf_len - chars_written <= 0) 
        */
        for (const Value *c = list_head(v); c != NIL; c = c->next) {
            /* dont add space after last val*/
            if (c->next == NIL) sep = "";
            if (chars_written < 0 || buf_len <= 0) return -1;
            chars_written = stringify_val(buf + total_chars_written, buf_len, c, sep);
            buf_len = buf_len - chars_written;
            total_chars_written += chars_written;
        }
        total_chars_written += snprintf(buf + total_chars_written, buf_len, ")");
        break;
    }
    buf_len = buf_len - total_chars_written;
    if (buf_len <= 0) return -1;
    return total_chars_written;
}

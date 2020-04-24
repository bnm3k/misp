#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    case IS_EXPR:
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

Value *make_sym(const char *s) {
    Value *v = allocate_value();
    v->type = IS_SYMBOL;
    v->content.sym = strdup(s);
    return v;
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

Value *make_expr() {
    Value *v = make_list();
    v->type = IS_EXPR;
    return v;
}

/*
 * Appends item Value v to front of list l
*/
Value *list_push_to_front(Value *l, Value *v) {
    assert(l->type == IS_LIST || l->type == IS_EXPR);
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
    assert(v->type == IS_LIST || v->type == IS_EXPR);
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
    assert(v->type == IS_LIST || v->type == IS_EXPR);
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
    case IS_EXPR:
    case IS_LIST:
        total_chars_written = snprintf(buf, buf_len, "%s", v->type == IS_EXPR ? "(" : "'(");
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

void test_stringify_val() {
    char buf[128];
    int buf_len = sizeof(buf);
    int chars_written;

    Value *n1000 = make_int(1000);
    chars_written = stringify_val(buf, buf_len, n1000, NULL);
    assert(chars_written == 4);
    assert(strcmp(buf, "1000") == 0);

    Value *s1 = make_sym("foo_sym");
    chars_written = stringify_val(buf, buf_len, s1, NULL);
    assert(chars_written == 7);
    assert(strcmp(buf, "foo_sym") == 0);

    Value *e1 = make_err("bar_err");
    chars_written = stringify_val(buf, buf_len, e1, NULL);
    assert(chars_written == 7);
    assert(strcmp(buf, "bar_err") == 0);

    Value *n1 = make_int(1), *n2 = make_int(2), *n3 = make_int(3);
    Value *l1 = make_list();
    Value *l2 = make_expr();

    list_push_to_front(l1, n1);
    list_push_to_front(l1, n2);
    list_push_to_front(l1, n3); // l1 = '(3 2 1)

    list_push_to_front(l2, l1);
    list_push_to_front(l2, make_sym("+"));
    list_push_to_front(l2, make_sym("map")); // l2 = (map + '(3 2 1))
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    assert(chars_written == 16);
    assert(strcmp(buf, "(map + '(3 2 1))") == 0);
}

int main(void) {
    printf("---------------\n");
    test_stringify_val();
    return 0;
}

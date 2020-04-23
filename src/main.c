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

int stringify_val_simple(char *buf, int buf_len, const Value *v) {
    if (buf_len <= 0) return buf_len - 1;
    int chars_written;
    switch (v->type) {
    case IS_NIL:
        chars_written = snprintf(buf, buf_len, "NIL");
        break;
    case IS_INT:
        chars_written = snprintf(buf, buf_len, "%ld", v->content.integer);
        break;
    case IS_SYMBOL:
        chars_written = snprintf(buf, buf_len, "%s", v->content.sym);
        break;
    case IS_ERROR:
        chars_written = snprintf(buf, buf_len, "%s", v->content.str);
        break;
    case IS_EXPR:
    case IS_LIST:
        chars_written = snprintf(buf, buf_len, "(...)");
    }
    if (buf_len <= 0) {
        return buf_len - 1;
    }
    //if (chars_written < 0) return chars_written;
    return chars_written;
}

int main(void) {
    // Value *n1 = make_int(1);
    // Value *n2 = make_int(2);
    // Value *n3 = make_int(3);
    // Value *l1 = make_list();
    // list_push_to_front(l1, n1);
    // list_push_to_front(l1, n2);
    // list_push_to_front(l1, n3);
    // Value *l2 = make_expr();
    // list_push_to_front(l2, l1);
    // list_push_to_front(l2, make_sym("+"));
    // list_push_to_front(l2, make_sym("map"));
    // print_val(l2);
    char buf[128];
    int buf_len = sizeof(buf);
    int chars_written;
    Value *n1 = make_int(1);
    Value *s1 = make_sym("foo_sym");
    Value *e1 = make_err("bar_err");

    chars_written = stringify_val_simple(buf, buf_len, n1);
    printf("buf = %s (len = %d)\n", buf, chars_written);

    chars_written = stringify_val_simple(buf, buf_len, s1);
    printf("buf = %s (len = %d)\n", buf, chars_written);

    chars_written = stringify_val_simple(buf, buf_len, e1);
    printf("buf = %s (len = %d)\n", buf, chars_written);

    return 0;
}

// int print_val(char *buf, int buf_len, const Value *v) {
//     switch (v->type) {

//     case IS_EXPR:
//     case IS_LIST:
//         // printf("%s", v->type == IS_EXPR ? "(" : "'(");
//         // list_traverse(v, print_list_val);
//         // printf(")");
//         printf("LIST");
//     }
// }

// bool print_list_val(const Value *v) {
//     assert(v != NIL);
//     print_val(v);
//     if (v->next != NIL) printf(" ");
//     return true;
// }

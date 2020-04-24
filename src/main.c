#include "val.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

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

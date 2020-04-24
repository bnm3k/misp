#include "../include/val.h"
#include "unity.h"

void test_stringify_val_sufficient_buf_size(void) {
    char buf[128];
    int buf_len = sizeof(buf);
    int chars_written;

    Value *n1000 = make_int(1000);
    chars_written = stringify_val(buf, buf_len, n1000, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 4);
    TEST_ASSERT_EQUAL_STRING(buf, "1000");

    Value *s1 = make_sym("foo_sym");
    chars_written = stringify_val(buf, buf_len, s1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 7);
    TEST_ASSERT_EQUAL_STRING(buf, "foo_sym");

    Value *e1 = make_err("bar_err");
    chars_written = stringify_val(buf, buf_len, e1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 7);
    TEST_ASSERT_EQUAL_STRING(buf, "bar_err");

    Value *n1 = make_int(1), *n2 = make_int(2), *n3 = make_int(3);
    Value *l1 = make_list();
    Value *l2 = make_expr();
    /* TODO add tests for empty lists and expressions here */

    list_push_to_front(l1, n1);
    list_push_to_front(l1, n2);
    list_push_to_front(l1, n3);
    chars_written = stringify_val(buf, buf_len, l1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 8);
    TEST_ASSERT_EQUAL_STRING(buf, "'(3 2 1)");

    list_push_to_front(l2, l1);
    list_push_to_front(l2, make_sym("+"));
    list_push_to_front(l2, make_sym("map"));
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 16);
    TEST_ASSERT_EQUAL_STRING(buf, "(map + '(3 2 1))");

    /* TODO deallocate vals */
}

void test_val(void) {
    RUN_TEST(test_stringify_val_sufficient_buf_size);
}

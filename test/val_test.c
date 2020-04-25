#include "../include/val.h"
#include "unity.h"

void test_stringify_val_sufficient_buf_size(void) {
    char buf[128];
    int buf_len = sizeof(buf);
    int chars_written;

    /* test nil val */
    chars_written = stringify_val(buf, buf_len, NIL, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 3);
    TEST_ASSERT_EQUAL_STRING(buf, "NIL");

    /* test numerical val */
    Value *n1000 = make_int(1000);
    chars_written = stringify_val(buf, buf_len, n1000, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 4);
    TEST_ASSERT_EQUAL_STRING(buf, "1000");

    /* test symbols and error vals */
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

    /* test empty lists and expressions */
    chars_written = stringify_val(buf, buf_len, l1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 3);
    TEST_ASSERT_EQUAL_STRING(buf, "'()");
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 2);
    TEST_ASSERT_EQUAL_STRING(buf, "()");

    /* test non-empty list */
    list_push_to_front(l1, n1);
    list_push_to_front(l1, n2);
    list_push_to_front(l1, n3);
    chars_written = stringify_val(buf, buf_len, l1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 8);
    TEST_ASSERT_EQUAL_STRING(buf, "'(3 2 1)");

    /* test non-empty expr */
    list_push_to_front(l2, l1);
    list_push_to_front(l2, make_sym("+"));
    list_push_to_front(l2, make_sym("map"));
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 16);
    TEST_ASSERT_EQUAL_STRING(buf, "(map + '(3 2 1))");

    /* TODO deallocate vals */
}

void test_stringify_val_insufficient_buf_size(void) {
    TEST_IGNORE_MESSAGE("TODO");
}

void test_identical_symbols_eval_to_same_value(void) {
    Value *s1 = make_sym("foo_sym");
    Value *s2 = make_sym("bar_sym");
    Value *s3 = make_sym("foo_sym");
    Value *s4 = make_sym("baz_sym");
    Value *s5 = make_sym("foo_sym");
    TEST_ASSERT_EQUAL_PTR(s1, s3);
    TEST_ASSERT_EQUAL_PTR(s1, s5);
    TEST_ASSERT_TRUE(s1 != s2);
    TEST_ASSERT_TRUE(s1 != s4);
    TEST_ASSERT_TRUE(s2 != s4);
    /* TODO deallocate vals */
}

void test_val(void) {
    RUN_TEST(test_stringify_val_sufficient_buf_size);
    RUN_TEST(test_stringify_val_insufficient_buf_size);
    RUN_TEST(test_identical_symbols_eval_to_same_value);
}
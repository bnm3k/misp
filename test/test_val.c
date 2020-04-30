#include "../include/debug.h"
#include "../include/val.h"
#include "test_include/unity.h"

void test_stringify_val_sufficient_buf_size_simple_vals(void) {
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
    deallocate_value(n1000);

    /* test symbols and error vals */
    Value *s1 = make_sym("foo_sym");
    chars_written = stringify_val(buf, buf_len, s1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 7);
    TEST_ASSERT_EQUAL_STRING(buf, "foo_sym");
    deallocate_value(s1);

    Value *e1 = make_err("bar_err");
    chars_written = stringify_val(buf, buf_len, e1, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 7);
    TEST_ASSERT_EQUAL_STRING(buf, "bar_err");
    deallocate_value(e1);
}

void test_stringify_val_sufficient_buf_size_lists(void) {
    char buf[128];
    int buf_len = sizeof(buf);
    int chars_written;
    Value *l1 = make_q_expr();
    Value *l2 = make_s_expr();

    /* test empty lists and expressions */
    chars_written = stringify_val(buf, buf_len, l1, NULL);
    TEST_ASSERT_EQUAL_STRING("{}", buf);
    TEST_ASSERT_EQUAL_INT(2, chars_written);
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    TEST_ASSERT_EQUAL_STRING(buf, "()");
    TEST_ASSERT_EQUAL_INT(2, chars_written);

    /* test non-empty list */
    builtin_list_push_to_front(l1, make_int(1));
    builtin_list_push_to_front(l1, make_int(2));
    builtin_list_push_to_front(l1, make_int(3));
    chars_written = stringify_val(buf, buf_len, l1, NULL);
    TEST_ASSERT_EQUAL_STRING(buf, "{3 2 1}");
    TEST_ASSERT_EQUAL_INT(7, chars_written);

    /* test non-empty expr */
    builtin_list_push_to_front(l2, l1);
    builtin_list_push_to_front(l2, make_sym("+"));
    builtin_list_push_to_front(l2, make_sym("map"));
    chars_written = stringify_val(buf, buf_len, l2, NULL);
    TEST_ASSERT_EQUAL_INT(chars_written, 15);
    TEST_ASSERT_EQUAL_STRING(buf, "(map + {3 2 1})");
    deallocate_value(l2);
}

void test_stringify_val_insufficient_buf_size(void) {
    TEST_IGNORE_MESSAGE("!** TODO **!");
}

void test_val(void) {
    RUN_TEST(test_stringify_val_sufficient_buf_size_simple_vals);
    RUN_TEST(test_stringify_val_sufficient_buf_size_lists);
    RUN_TEST(test_stringify_val_insufficient_buf_size);
}

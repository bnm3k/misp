#include "../include/evaluator.h"
#include "../include/val.h"
#include "test_include/unity.h"

void test_evaluator_identical_symbols_eval_to_same_value(void) {
    evaluator *ev = new_evaluator();
    Value *s1 = make_sym_interned(ev->sym_table, "foo_sym");
    Value *s2 = make_sym_interned(ev->sym_table, "bar_sym");
    Value *s3 = make_sym_interned(ev->sym_table, "foo_sym");
    Value *s4 = make_sym_interned(ev->sym_table, "baz_sym");
    Value *s5 = make_sym_interned(ev->sym_table, "foo_sym");
    TEST_ASSERT_EQUAL_PTR(s1, s3);
    TEST_ASSERT_EQUAL_PTR(s1, s5);
    TEST_ASSERT_TRUE(s1 != s2);
    TEST_ASSERT_TRUE(s1 != s4);
    TEST_ASSERT_TRUE(s2 != s4);
    deallocate_value(s1);
    deallocate_value(s2);
    deallocate_value(s4);
    delete_evaluator(ev);
}

void test_evaluator(void) {
    RUN_TEST(test_evaluator_identical_symbols_eval_to_same_value);
}

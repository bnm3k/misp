#include "../include/evaluator.h"
#include "../include/val.h"
#include "test_include/unity.h"

void test_evaluator_stuff(void) {
    evaluator *ev = new_evaluator();
    Value *res;
    res = read_evaluate(ev, "def");
    TEST_ASSERT_TRUE(res->type == IS_FN);
    deallocate_value(res);
    TEST_IGNORE_MESSAGE("ensure vals in global_env are deallocated: !** TODO **!");
    delete_evaluator(ev);
}

void test_evaluator(void) {
    RUN_TEST(test_evaluator_stuff);
}

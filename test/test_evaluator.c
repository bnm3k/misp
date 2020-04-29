#include "../include/evaluator.h"
#include "../include/val.h"
#include "test_include/unity.h"

void test_evaluator_stuff(void) {
    evaluator *ev = new_evaluator();
    TEST_IGNORE_MESSAGE("!** TODO **!");
    delete_evaluator(ev);
}

void test_evaluator(void) {
    RUN_TEST(test_evaluator_stuff);
}

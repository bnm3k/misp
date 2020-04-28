#include "test_include/test.h"
#include "test_include/unity.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    test_list();
    test_val();
    test_parser();
    test_environment();
    return UNITY_END();
}

#include "test.h"
#include "unity.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    test_val();
    return UNITY_END();
}

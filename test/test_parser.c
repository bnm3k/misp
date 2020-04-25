#include "../include/mpc.h"
#include "../include/parser.h"
#include "test_include/unity.h"
#include <assert.h>

void test_single_input_validity(const char *str, bool is_valid) {
    parse_result_t *p = parse(str);
    if (p != NULL) {
        TEST_ASSERT(p->no_err_occurred == is_valid);
        parse_res_cleanup(p);
    } else {
        printf("on parse, malloc returned NULL");
    }
}

void test_parser_valid_inputs(void) {
#define valid_inputs_len 5
    const char *valid_inputs[valid_inputs_len] = {
        "1", "()",
        "(+ 1 2 3)",
        "+ 5 (* 2 2)",
        "(map - '(1 2 3 4))"};
    for (unsigned i = 0; i < valid_inputs_len; i++) {
        test_single_input_validity(valid_inputs[i], true);
    }
}

void test_parser_invalid_inputs(void) {
#define invalid_inputs_len 2
    const char *invalid_inputs[invalid_inputs_len] = {
        ")(", "((1) quote))"};
    for (unsigned i = 0; i < invalid_inputs_len; i++) {
        test_single_input_validity(invalid_inputs[i], false);
    }
}

void test_parser(void) {
    parser_init();
    RUN_TEST(test_parser_valid_inputs);
    RUN_TEST(test_parser_invalid_inputs);
    parser_cleanup();
}

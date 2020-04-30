#include "../include/mpc.h"
#include "../include/parser.h"
#include "test_include/unity.h"
#include <assert.h>

void test_single_input_validity(parser *p, const char *str, bool is_valid) {
    parse_result *p_res = parse_str_to_ast(p, str);
    if (p != NULL) {
        TEST_ASSERT(p_res->no_err_occurred == is_valid);
        parse_res_cleanup(p_res);
    } else {
        printf("error on parse");
    }
}

#define valid_inputs_len 6
void test_parser_valid_inputs(void) {
    parser *p = new_parser();
    const char *valid_inputs[valid_inputs_len] = {
        "1", "()", "NIL",
        "(+ 1 2 3)",
        "+ 5 (* 2 2)",
        "(map - {1 2 3 4})"};
    for (unsigned i = 0; i < valid_inputs_len; i++) {
        test_single_input_validity(p, valid_inputs[i], true);
    }
    delete_parser(p);
}

#define invalid_inputs_len 2
void test_parser_invalid_inputs(void) {
    parser *p = new_parser();
    const char *invalid_inputs[invalid_inputs_len] = {
        ")(", "((1) quote))"};
    for (unsigned i = 0; i < invalid_inputs_len; i++) {
        test_single_input_validity(p, invalid_inputs[i], false);
    }
    delete_parser(p);
}

void test_parser(void) {
    RUN_TEST(test_parser_valid_inputs);
    RUN_TEST(test_parser_invalid_inputs);
}

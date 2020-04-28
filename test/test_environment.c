#include "../include/environment.h"
#include "../include/val.h"
#include "./test_include/unity.h"
#include <assert.h>

void test_environemnt_single_layer_resolution(void) {
    environment *env = new_env(NULL);
    Value *identifier = make_sym_interned(NULL, "foo");
    Value *num_literal = make_int(9000);
    env_set(env, identifier, num_literal);
    Value *gotten = env_get(env, identifier);
    TEST_ASSERT_EQUAL_PTR(num_literal, gotten);
    deallocate_value(identifier);
    deallocate_value(num_literal);
    delete_env(env);
}

void test_environemnt_multiple_layer_resolution(void) {

    // env1 <- env2 <- env3 <- env4
    environment *env1 = new_env(NULL);
    environment *env2 = new_env(env1);
    environment *env3 = new_env(env2);
    environment *env4 = new_env(env3);

    Value *id1 = make_sym_interned(NULL, "foo");
    Value *id2 = make_sym_interned(NULL, "bar");
    Value *id3 = make_sym_interned(NULL, "baz");
    Value *n1 = make_int(8000);
    Value *n2 = make_int(7000);
    Value *n3 = make_int(6000);

    /* parent env chain */
    env_set(env1, id1, n1);
    TEST_ASSERT_EQUAL_PTR(n1, env_get(env1, id1));
    TEST_ASSERT_EQUAL_PTR(n1, env_get(env2, id1));
    TEST_ASSERT_EQUAL_PTR(n1, env_get(env3, id1));
    env_set(env1, id1, n2);
    TEST_ASSERT_EQUAL_PTR(n2, env_get(env4, id1));

    /* shadowing */
    env_set(env4, id1, n1);
    env_set(env4, id2, n2);
    env_set(env4, id3, n3);
    TEST_ASSERT_EQUAL_PTR(n1, env_get(env4, id1));
    TEST_ASSERT_EQUAL_PTR(n2, env_get(env4, id2));
    TEST_ASSERT_EQUAL_PTR(n3, env_get(env4, id3));

    deallocate_value(id1);
    deallocate_value(id2);
    deallocate_value(id3);
    deallocate_value(n1);
    deallocate_value(n2);
    deallocate_value(n3);
    delete_env(env4);
    delete_env(env3);
    delete_env(env2);
    delete_env(env1);
}

void test_environment(void) {
    RUN_TEST(test_environemnt_single_layer_resolution);
    RUN_TEST(test_environemnt_multiple_layer_resolution);
}

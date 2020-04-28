#include "../include/list.h"
#include "../include/val.h"
#include "test_include/unity.h"
#include <stdio.h>

void test_list_basics(void) {
    List *l = new_list();
    TEST_ASSERT_EQUAL_INT(0, list_get_count(l));

    list_push_to_front(l, make_int(200));
    TEST_ASSERT_EQUAL_INT(200, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(1, list_get_count(l));

    list_push_to_back(l, make_int(91));
    TEST_ASSERT_EQUAL_INT(200, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(2, list_get_count(l));

    list_push_to_front(l, make_int(300));
    TEST_ASSERT_EQUAL_INT(300, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(3, list_get_count(l));

    list_push_to_back(l, make_int(92));
    TEST_ASSERT_EQUAL_INT(300, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(4, list_get_count(l));

    list_push_to_front(l, make_int(400));
    TEST_ASSERT_EQUAL_INT(400, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(5, list_get_count(l));

    list_push_to_back(l, make_int(93));
    TEST_ASSERT_EQUAL_INT(400, ((Value *)list_get_head(l))->content.integer);
    TEST_ASSERT_EQUAL_INT(6, list_get_count(l));

    //   400-300-200-91-92-93
    Value *curr_elem;
    int i = 0;
    int expected[] = {400, 300, 200, 91, 92, 93};
    list_foreach(l, curr_elem, {
        TEST_ASSERT_EQUAL_INT(expected[i], curr_elem->content.integer);
        i++;
    });
    list_foreach(l, curr_elem, {
        deallocate_value(curr_elem);
    });
    delete_list(l);
}

void test_list_insert_100_items_from_back(void) {
    Value **arr = malloc(sizeof(Value *) * 100);
    List *l = new_list();
    Value *new_num_val;
    for (int i = 0; i < 100; i++) {
        new_num_val = make_int(i);
        list_push_to_back(l, new_num_val);
        arr[i] = new_num_val;
    }
    Value *curr_list_elem;
    int i = 0;
    list_foreach(l, curr_list_elem, {
        TEST_ASSERT_EQUAL_INT(arr[i]->content.integer, curr_list_elem->content.integer);
        i++;
    });
    TEST_ASSERT_EQUAL_INT(100, l->curr_size);
    TEST_ASSERT_EQUAL_INT(0, ((Value *)list_get_head(l))->content.integer);

    free(arr);
    list_foreach(l, curr_list_elem, {
        deallocate_value(curr_list_elem);
    });
    delete_list(l);
}

void test_list_insert_100_items_from_front(void) {
    Value **arr = malloc(sizeof(Value *) * 100);
    List *l = new_list();
    Value *new_num_val;
    for (int i = 0; i < 100; i++) {
        new_num_val = make_int(i);
        list_push_to_front(l, new_num_val);
        arr[i] = new_num_val;
    }
    Value *curr_list_elem;
    /* iter from front to back */
    int i = 99;
    list_foreach(l, curr_list_elem, {
        TEST_ASSERT_EQUAL_INT(arr[i]->content.integer, curr_list_elem->content.integer);
        i--;
    });

    /* iter from back to front */
    i = 0;
    list_foreach_reverse(l, curr_list_elem, {
        TEST_ASSERT_EQUAL_INT(arr[i]->content.integer, curr_list_elem->content.integer);
        i++;
    });

    TEST_ASSERT_EQUAL_INT(100, l->curr_size);
    TEST_ASSERT_EQUAL_INT(99, ((Value *)list_get_head(l))->content.integer);

    free(arr);
    list_foreach(l, curr_list_elem, {
        deallocate_value(curr_list_elem);
    });
    delete_list(l);
}

void test_list_pop_items(void) {
    List *l = new_list();
    return;
    /* insert 20 items */
    for (int i = 1; i <= 20; i++) {
        list_push_to_back(l, make_int(i));
    }
    Value *popped;
    for (int i = 0; i < 10; i++) {
        popped = list_pop_from_back(l);
        deallocate_value(popped);
    }

    TEST_ASSERT_EQUAL_INT(10, l->curr_size);

    int arr1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Value *curr_elem;
    int i = 0;
    list_foreach(l, curr_elem, {
        TEST_ASSERT_EQUAL_INT(arr1[i], curr_elem->content.integer);
        i++;
    });

    for (int i = 0; i < 5; i++) {
        popped = list_pop_from_front(l);
        deallocate_value(popped);
    }

    TEST_ASSERT_EQUAL_INT(5, l->curr_size);
    int arr2[] = {6, 7, 8, 9, 10};
    i = 0;
    list_foreach(l, curr_elem, {
        TEST_ASSERT_EQUAL_INT(arr2[i], curr_elem->content.integer);
        i++;
    });

    list_foreach(l, curr_elem, {
        deallocate_value(curr_elem);
    });
    delete_list(l);
}

void test_list_shallow_copy(void) {
    List *l1 = new_list();
    Value *v_l1;
    for (int i = 0; i < 20; i++) {
        list_push_to_back(l1, make_int(i));
    }
    List *l2 = list_shallow_copy(l1);
    TEST_ASSERT_EQUAL_INT(list_get_count(l1), list_get_count(l2));
    Value *popped_l2;
    list_foreach(l1, v_l1, {
        popped_l2 = list_pop_from_front(l2);
        TEST_ASSERT_EQUAL_PTR(v_l1, popped_l2);
    });

    list_foreach(l1, v_l1, {
        deallocate_value(v_l1);
    });
    delete_list(l1);
    delete_list(l2);
}

void test_list(void) {
    RUN_TEST(test_list_basics);
    RUN_TEST(test_list_insert_100_items_from_back);
    RUN_TEST(test_list_insert_100_items_from_front);
    RUN_TEST(test_list_pop_items);
    RUN_TEST(test_list_shallow_copy);
}

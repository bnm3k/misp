#include "../include/list.h"
#include "../include/val.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_CAPACITY 5

List *new_list() {
    List *l = malloc(sizeof(List));
    l->capacity = INIT_CAPACITY;
    l->curr_size = 0;
    l->elements = calloc(INIT_CAPACITY, sizeof(Value *));
    l->first = l->last = -1;
    return l;
}

void list_resize_up(List *l) {
    const int new_capacity = l->capacity * 2;
    Value **new_elements = calloc(new_capacity, sizeof(Value *));
    Value *vp;
    int i = 0;
    list_foreach(l, vp, {
        new_elements[i] = vp;
        i++;
    });
    free(l->elements);
    l->first = 0;
    l->last = i - 1;
    l->capacity = new_capacity;
    l->elements = new_elements;
}

void delete_list(List *l) {
    free(l->elements);
    free(l);
}

void list_push_to_front(List *l, Value *vp) {
    assert(l->curr_size <= l->capacity);
    /* check first if full */
    if (l->curr_size == l->capacity) list_resize_up(l);
    /* check if adding first item */
    if (l->curr_size == 0)
        l->first = l->last = 0;
    else /* otherwise add as expected */
        l->first = MOD((l->first - 1), l->capacity);
    assert(l->first >= 0 && l->first < l->capacity);
    l->curr_size++;
    l->elements[l->first] = vp;
}
void list_push_to_back(List *l, Value *vp) {
    assert(l->curr_size <= l->capacity);
    /* check first if full */
    if (l->curr_size == l->capacity) list_resize_up(l);
    /* check if adding first item */
    if (l->curr_size == 0)
        l->first = l->last = 0;
    else /* otherwise add as expected */
        l->last = MOD((l->last + 1), l->capacity);
    assert(l->last >= 0 && l->last < l->capacity);
    l->curr_size++;
    l->elements[l->last] = vp;
}
Value *list_pop_from_front(List *l) {
    assert(l->curr_size <= l->capacity);
    Value *popped = l->elements[l->first];
    l->elements[l->first] = NULL; /* clear pos. shouldn't be necessary */
    l->first = MOD((l->first + 1), l->capacity);
    l->curr_size--;
    if (l->curr_size == 0) l->first = l->last = -1;
    return popped;
}
Value *list_pop_from_back(List *l) {
    assert(l->curr_size <= l->capacity);
    Value *popped = l->elements[l->last];
    l->elements[l->last] = NULL; /* clear pos. shouldn't be necessary */
    l->last = MOD((l->last - 1), l->capacity);
    l->curr_size--;
    if (l->curr_size == 0) l->first = l->last = -1;
    return popped;
}

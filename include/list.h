#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

typedef struct List {
    void **elements;
    int capacity, curr_size;
    int first, last;
} List;

List *new_list();
void delete_list(List *l);
List *list_shallow_copy(const List *);
void list_push_to_front(List *l, void *vp);
void list_push_to_back(List *l, void *vp);
void *list_pop_from_front(List *l);
void *list_pop_from_back(List *l);

#define MOD(a, n) a < 0 ? n - 1 : (a) % n

#define list_get_count(list) list->curr_size

#define list_get_head(list) list->elements[list->first]

#define list_foreach(list, v_ptr_var, code)         \
    {                                               \
        int pos = list->first;                      \
        if (list->curr_size > 0)                    \
            while (1) {                             \
                v_ptr_var = list->elements[pos];    \
                code;                               \
                if (pos == list->last) break;       \
                pos = MOD(pos + 1, list->capacity); \
            }                                       \
    }

#define list_foreach_reverse(list, v_ptr_var, code) \
    {                                               \
        int pos = list->last;                       \
        if (list->curr_size > 0)                    \
            while (1) {                             \
                v_ptr_var = list->elements[pos];    \
                code;                               \
                if (pos == list->first) break;      \
                pos = MOD(pos - 1, list->capacity); \
            }                                       \
    }

#endif

//
// Created by Никита Третьяков on 24.02.2024.
//

#ifndef LAB9_BINARY_HEAP_H
#define LAB9_BINARY_HEAP_H

#include <stdlib.h>

#include "utility.h"

typedef struct Binary_heap
{
    unsigned int size;
    unsigned int capacity;
    request** elements;
    int (*compare)(const request *, const request *);
} Binary_heap, bin_heap;

void bh_destruct_node(request* node);
status_code bh_destruct_heap(bin_heap* heap);

status_code bh_set_null(bin_heap* heap);
status_code bh_construct_heap(bin_heap* heap, int (*compare)(const request *, const request *));
status_code bh_top(const bin_heap* heap, request** result);
status_code bh_size(const bin_heap* heap, size_t* res);

status_code bh_sift_down(bin_heap* heap);
status_code bh_sift_up(bin_heap* heap);
status_code bh_insert(bin_heap* heap, request* new_el);

status_code bh_request_copy(request* res, const request* src);
status_code bh_heap_copy(bin_heap* dest, const bin_heap* src);

status_code bh_pop(bin_heap* heap, request** min);
status_code bh_destructive_merge(bin_heap* result, bin_heap* a, bin_heap* b);
status_code bh_merge(bin_heap* result, const bin_heap* a, const bin_heap* b);

#endif //LAB9_BINARY_HEAP_H

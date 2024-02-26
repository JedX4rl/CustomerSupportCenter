#include <string.h>

#include "binary_heap.h"

int compare_pq_key(const request* a, const request* b)
{
    if (a->prior == b->prior)
    {
        return strcmp(a->time, b->time);
    }
    return a->prior > b->prior ? -1 : 1;
}

status_code bh_set_null(bin_heap* heap)
{
    if (heap == NULL)
    {
        return NULL_ARG;
    }
    heap->capacity = 0;
    heap->size = 0;
    heap->elements = NULL;
    heap->compare = NULL;
    return OK;
}

void bh_destruct_node(request* node)
{
    if (node == NULL)
    {
        return;
    }
    node->prior = 0;
    node->id = 0;
    free(node->txt);
    free(node);
}

status_code bh_destruct_heap(bin_heap* heap)
{
    if (heap == NULL)
    {
        return NULL_ARG;
    }

    while (heap->size)
    {
        bh_destruct_node(heap->elements[heap->size]);
        heap->size = heap->size - 1;
    }
    free(heap->elements);
    bh_set_null(heap);
    return OK;
}

status_code bh_construct_heap(bin_heap* heap, int (*compare)(const request *, const request *))
{
    if (heap == NULL)
    {
        return NULL_ARG;
    }
    heap->elements = (request**)calloc(2, sizeof(request*));
    if (heap->elements == NULL)
    {
        bh_destruct_heap(heap);
        return BAD_ALLOC;
    }
    heap->compare = compare;
    heap->capacity = 2;
    heap->size = 0;
    return OK;
}

status_code bh_sift_down(bin_heap* heap)
{
    if (heap == NULL)
    {
        return NULL_ARG;
    }
    int i = 0;
    while (2 * i + 1 < heap->size)
    {
        int left_idx = 2 * i + 1;
        int right_idx = 2 * i + 2;
        int j = left_idx;
        if (right_idx < heap->size && (heap->compare(heap->elements[right_idx], heap->elements[left_idx]) < 0))
        {
            j = right_idx;
        }
        if (heap->compare(heap->elements[i], heap->elements[j]) < 0)
        {
            break;
        }
        request* tmp = heap->elements[i];
        heap->elements[i] = heap->elements[j];
        heap->elements[j] = tmp;
        i = j;
    }
    return OK;
}

status_code bh_sift_up(bin_heap* heap)
{
    if (heap == NULL)
    {
        return NULL_ARG;
    }
    unsigned i = heap->size;

    while (i > 0 && (heap->compare(heap->elements[(i-1) / 2], heap->elements[i]) > 0))
    {
        request* tmp = heap->elements[i];
        heap->elements[i] = heap->elements[(i - 1) / 2];
        heap->elements[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
    return OK;
}

status_code bh_insert(bin_heap* heap, request* new_el)
{
    if (heap == NULL || heap->elements == NULL || new_el == NULL)
    {
        return NULL_ARG;
    }
    if (heap->size == heap->capacity)
    {
        request** tmp = realloc(heap->elements, sizeof(request*) * (heap->capacity * 2));
        if (tmp == NULL)
        {
            return BAD_ALLOC;
        }
        heap->capacity *= 2;
        heap->elements = tmp;
    }
    heap->elements[heap->size] = new_el;
    status_code st = bh_sift_up(heap);
    if (st)
    {
        return st;
    }
    heap->size = heap->size + 1;
    return OK;
}

status_code bh_request_copy(request* res, const request* src)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }

    request tmp;

    tmp = *src;
    tmp.txt = (char*)malloc(sizeof(char) * (strlen(src->txt) + 1));
    if (tmp.txt == NULL)
    {
        return BAD_ALLOC;
    }
    strcpy(tmp.txt, src->txt);

    *res = tmp;
    return OK;
}

status_code bh_top(const bin_heap* heap, request** result)
{
    if (heap == NULL || result == NULL)
    {
        return NULL_ARG;
    }
    *result = heap->elements[0];
    return OK;
}

status_code bh_size(const bin_heap* heap, size_t* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    *res = heap->size;
    return OK;
}

status_code bh_pop(bin_heap* heap, request** min)
{
    if (heap == NULL || min == NULL)
    {
        return NULL_ARG;
    }
    if (!heap->size)
    {
        *min = NULL;
        return OK;
    }
    request* tmp = heap->elements[0];
    heap->elements[0] = heap->elements[heap->size-1];
    heap->size = heap->size - 1;
    status_code st = bh_sift_down(heap);
    if (st)
    {
        return st;
    }
    *min = tmp;
    return OK;
}

status_code bh_heap_copy(bin_heap* dest, const bin_heap* src)
{
    if (dest == NULL || src == NULL)
    {
        return NULL_ARG;
    }
    dest->elements = (request**) calloc(src->capacity, sizeof(request*));
    if (dest->elements == NULL)
    {
        bh_destruct_heap(dest);
        return BAD_ALLOC;
    }
    dest->capacity = src->capacity;
    for (int i = 0; i < src->size; ++i)
    {
        request* req = (request*)malloc(sizeof(request));
        if (req == NULL)
        {
            bh_destruct_heap(dest);
            return BAD_ALLOC;
        }
        status_code st = bh_request_copy(req, src->elements[i]);
        if (st)
        {
            bh_destruct_heap(dest);
            bh_destruct_node(req);
            return st;
        }
        dest->elements[i] = req;
    }
    dest->compare = src->compare;
    dest->size = src->size;
    return OK;
}

status_code bh_destructive_merge(bin_heap* result, bin_heap* a, bin_heap* b)
{
    if (a == NULL || b == NULL)
    {
        return NULL_ARG;
    }
    if (a == b)
    {
        return INVALID_INPUT;
    }
    while (b->size)
    {
        request* tmp = NULL;
        status_code st = bh_pop(b, &tmp);
        if (st)
        {
            return st;
        }
        st = bh_insert(a, tmp);
        if (st)
        {
            return st;
        }
    }
    *result = *a;
    bh_set_null(a);
    bh_set_null(b);
    return OK;
}

status_code bh_merge(bin_heap* result, const bin_heap* a, const bin_heap* b)
{
    if (a == NULL || b == NULL)
    {
        return NULL_ARG;
    }
    bin_heap a_tmp, b_tmp, temp;
    bh_set_null(&a_tmp);
    bh_set_null(&b_tmp);
    bh_set_null(&temp);
    status_code st = bh_heap_copy(&a_tmp, a);
    st = st ? st : bh_heap_copy(&b_tmp, b);
    st = st ? st : bh_destructive_merge(&temp, &a_tmp, &b_tmp);
    if (st)
    {
        bh_destruct_heap(&a_tmp);
        bh_destruct_heap(&b_tmp);
        bh_destruct_heap(&temp);
        return st;
    }
    *result = temp;
    return OK;
}

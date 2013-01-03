/*
 * linkedlist.c
 *
 *  Created on: Sep 28, 2010
 *      Author: buttonfly
 */

#include "linkedlist.h"
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

// symbol definition
//typedef char bool;
#ifdef TRUE
#undef TRUE
#define TRUE    1
#else
#define TRUE    1
#endif

#ifdef FALSE
#undef FALSE
#define FALSE    0
#else
#define    FALSE    0
#endif


// type definition
typedef struct _node
{
    struct _node*    prev;
    struct _node*    next;
    void*            data;
} node;

typedef struct
{
    node*     head;
    node*     tail;
    int        size;
} linkedlist_t;


// function prototype
static node* node_new(void* data);
static node* get_next(node* n);
static node* get_prev(node* n);
static void set_next(node* n, node* next);
static void set_prev(node* n, node* prev);
static node* get_head(linkedlist_t* list);
static node* get_tail(linkedlist_t* list);
static void set_head(linkedlist_t* list, node* n);
static void set_tail(linkedlist_t* list, node* n);
static bool is_first(linkedlist_t* list, node* n);
static bool is_last(linkedlist_t* list, node* n);
static node* find_by_data(linkedlist_t* list, void* data);
static node* find_by_index(linkedlist_t* list, int at);
static int add_at_first(linkedlist_t* list, void* data);
static int add_at_last(linkedlist_t* list, void* data);


// function definition
linkedlist linkedlist_new(void)
{
    int linkedlist_size = sizeof(linkedlist_t);
    linkedlist_t* list = (linkedlist_t*) malloc(linkedlist_size);
    memset(list, 0, linkedlist_size);
    set_head(list, NULL);
    set_tail(list, NULL);
    list->size = 0;
    return (linkedlist) list;
}

int linkedlist_add(linkedlist _list, void* data)
{
    return add_at_last((linkedlist_t*)_list, data);
}


int linkedlist_add_at(linkedlist _list, void* data, int i)
{
    linkedlist_t* list = (linkedlist_t*) _list;
    node* slider, *n;

    if(i > (list->size - 1))
        return add_at_last(list, data);

    if(i == 0)
        return add_at_first(list, data);
    else
    {
        slider = find_by_index(list, i);
        n = node_new(data);
        set_next(get_prev(slider), n);
        set_prev(n, get_prev(slider));
        set_next(n, slider);
        list->size++;
        return list->size;
    }
}

const void* linkedlist_get(linkedlist list, int i)
{
    node* n = find_by_index((linkedlist_t*)list, i);
    if(NULL == n) return NULL;
    return n->data;
}

void* linkedlist_remove_at(linkedlist _list, int i)
{
    node* n = find_by_index((linkedlist_t*)_list, i);
    if(NULL == n) return NULL;
    return linkedlist_remove(_list, n->data);
}

void* linkedlist_remove(linkedlist _list, void* data)
{
    linkedlist_t* list = (linkedlist_t*) _list;
    node* n = find_by_data(list, data);

    if(NULL == n)
        return NULL;

    if(is_first(list, n)) // at the first
    {
        if(is_last(list, n)) // only one exists
        {
            set_head(list, NULL);
            set_tail(list, NULL);
        }
        else // one or more exist
        {
            set_head(list, get_next(n));
            set_prev(n, NULL);
        }
    }
    else if(is_last((linkedlist_t*)_list, n))
    {
        set_next(get_prev(n), NULL);
        set_tail(list, get_prev(n));
    }
    else
    {
        set_prev(get_next(n), get_prev(n));
        set_next(get_prev(n), get_next(n));
    }
    list->size--;
    free(n);
    return data;
}

int linkedlist_size(linkedlist _list)
{
    linkedlist_t* list = (linkedlist_t*) _list;
    return list->size;
}

void linkedlist_clear(linkedlist _list, void (*fp)(void*))
{
    linkedlist_t* list = (linkedlist_t*) _list;
    void* d;
    for(;(NULL != (d = linkedlist_remove_at(list, 0)));)
    {
        if(fp != NULL) fp(d);
    }
}

void linkedlist_delete(linkedlist _list, void (*fp)(void*))
{
    linkedlist_clear(_list, fp);
    free(_list);
}

node* node_new(void* data)
{
    node* n = (node*) malloc(sizeof(node));
    set_next(n, NULL);
    set_prev(n, NULL);
    n->data = data;
    return n;
}

node* get_next(node* n)
{
    return n->next;
}

node* get_prev(node* n)
{
    return n->prev;
}


node* get_head(linkedlist_t* list)
{
    return list->head;
}



node* get_tail(linkedlist_t* list)
{
    return list->tail;
}


node* find_by_data(linkedlist_t* list, void* data)
{
    node* slider;
    slider = get_head(list);
    for(; NULL != slider;)
    {
        if(slider->data == data)
            break;

        slider = get_next(slider);
    }
    return slider;
}


node* find_by_index(linkedlist_t* list, int at)
{
    int i;
    node* slider = slider = get_head(list);
    for(i = 0; i < at; i++)
        slider = get_next(slider);

    return slider;
}

bool is_first(linkedlist_t* list, node* n)
{
    if(n == get_head(list))
        return TRUE;
    return FALSE;
}

bool is_last(linkedlist_t* list, node* n)
{
    if(n == get_tail(list))
        return TRUE;
    return FALSE;
}

int add_at_last(linkedlist_t* list, void* data)
{
    node* n;
    n = node_new(data);

    // empty?
    if(NULL == get_head(list))
    {
        set_head(list, n);
        set_tail(list, n);
        list->size = 1;
        return list->size;
    }

    set_next(get_tail(list), n);
    set_prev(n, get_tail(list));
    set_tail(list, n);

    list->size++;
    return list->size;
}

int add_at_first(linkedlist_t* _list, void* data)
{
    linkedlist_t* list = (linkedlist_t*) _list;
    node* n, *slider;

    if(list->size == 0)
    {
        return add_at_last(list, data);
    }
    else if(list->size == 1)
    {
        n = node_new(data);
        set_head(list, n);
        set_prev(get_tail(list), n);
        set_next(n, get_tail(list));
        list->size++;
    }
    else // list->size > 1
    {
        slider = get_head(list);
        n = node_new(data);
        set_next(n, slider);
        set_prev(slider, n);
        set_head(list, n);
        list->size++;
    }
    return list->size;
}

void set_head(linkedlist_t* list, node* n)
{
    list->head = n;
}

void set_tail(linkedlist_t* list, node* n)
{
    list->tail = n;
}


void set_next(node* n, node* next)
{
    n->next = next;
}

void set_prev(node* n, node* prev)
{
    n->prev = prev;
}

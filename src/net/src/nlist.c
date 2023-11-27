#include "nlist.h"

void nlist_init(nlist_t *nlist)
{
    nlist->head = nlist->tail = (nlist_node_t *)0;
    nlist->count = 0;
}

void nlist_insert_head(nlist_t *list, nlist_node_t *node)
{
    node->next = list->head;
    node->pre = (nlist_node_t *)0;

    if (nlist_is_empty(list))
    {
        list->tail = list->head = node;
    }
    else
    {
        list->head->pre = node;
        list->head = node;
    }
    list->count++;
}
nlist_node_t *nlist_remove_node(nlist_t *list, nlist_node_t *node)
{
    if (node->pre)
    {
        node->pre->next = node->next;
    }

    if (node->next)
    {
        node->next->pre = node->pre;
    }

    if (node == list->head)
    {
        list->head = node->next;
    }
    if (node == list->tail)
    {
        list->tail = node->pre;
    }

    node->next = node->pre = (nlist_node_t *)0;
    list->count--;
    return node;
}

void nlist_insert_tail(nlist_t *list, nlist_node_t *node)
{
    node->next = (nlist_node_t *)0;
    node->pre = list->tail;

    if (nlist_is_empty(list))
    {
        list->head = list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    list->count++;
}

void nlist_insert_after(nlist_t *list, nlist_node_t *pre, nlist_node_t *node)
{
    if (nlist_is_empty(list) || pre == (nlist_node_t *)0)
    {
        nlist_insert_head(list, node);
        return;
    }
    node->next = pre->next;
    node->pre = pre;
    if (pre->next)
    {
        pre->next->pre = node;
    }
    pre->next = node;

    if (pre == list->tail)
    {
        list->tail = node;
    }

    list->count++;
    return;
}
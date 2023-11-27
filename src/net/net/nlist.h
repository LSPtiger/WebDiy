#ifndef NLIST_H
#define NLIST_H

typedef struct _nlist_node_t
{
    struct _nlist_node_t * next;
    struct _nlist_node_t * pre;

}nlist_node_t;


static inline void nlist_node_init (nlist_node_t * node){
    node->next = node->pre = (nlist_node_t *)0;
}

static inline nlist_node_t * nlist_node_next (nlist_node_t * node) {
    return node->next;
}

static inline nlist_node_t * nlist_node_pre (nlist_node_t * node) {
    return node->pre;
}

static inline nlist_node_set_next (nlist_node_t * node, nlist_node_t * next){
    node->next = next;
}

static inline nlist_node_set_pre (nlist_node_t * node, nlist_node_t * pre){
    node->pre = pre;
}

typedef struct _nlist_t
{
    struct _nlist_node_t * head;
    struct _nlist_node_t * tail;
    int count;
}nlist_t;

void nlist_insert_head(nlist_t *list, nlist_node_t *node);

void nlist_insert_after(nlist_t *list, nlist_node_t *pre, nlist_node_t *node);

void nlist_insert_tail(nlist_t *list, nlist_node_t *node);

void nlist_delete_head(nlist_t *list, nlist_node_t *node);

void nlist_init (nlist_t * nlist);

static inline int nlist_count (nlist_t * list){
    return list->count;
}

static inline int nlist_is_empty (nlist_t * list){
    return list->count == 0;
}

static inline nlist_node_t * nlist_head (nlist_t * list) {
    return list->head;
}

static inline nlist_node_t * nlist_tail (nlist_t * list) {
    return list->tail;
}

#define noffset_in_parent(parent_type, node_name)   \
    ((char *)(&(((parent_type *)0))->node_name))

#define noffset_to_parent(node, parent_type, node_name)   \
    (parent_type *)((char *)node - noffset_in_parent(parent_type, node_name))

#define nlist_entry(node, parent_type, node_name)   \
    (( node ) ? noffset_to_parent(( node ), parent_type, node_name) : (parent_type *)0)

#define nlist_for_each(node, list)  for(( node ) = ( list )->head; ( node ); ( node ) = node->next)

nlist_node_t * nlist_remove_node (nlist_t * list, nlist_node_t * node);

static inline nlist_node_t * nlist_remove_head(nlist_t * list){
    nlist_node_t * list_head = nlist_head(list);
    if(list->head){
        nlist_remove_node(list, list->head);
    }
    return list_head;
}


static inline nlist_node_t * nlist_remove_tail(nlist_t * list){
    nlist_node_t * list_tail = nlist_tail(list);
    if(list->tail){
        nlist_remove_node(list, list->tail);
    }
    return list_tail;
}
#endif
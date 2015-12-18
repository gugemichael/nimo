/*
 *	 list.h
 *
 *  Created on: 2011.9.1
 *  Author: Michael LiuXin
 *
 *  Description : a double link list , list_node/list_iter/list
 *
 */

#ifndef __CLOWF_LIST_H__
#define __CLOWF_LIST_H__

#ifdef  __cplusplus
extern "c" {
#endif     

/* Node, List, and Iterator are the only data structures used currently. */

typedef struct list_node_t {
    struct list_node_t *prev;
    struct list_node_t *next;
    void *value;
} list_node;

typedef struct list_iter_t {
    list_node *next;
    int direction;
} list_iter;

typedef struct list {
    list_node *head;
    list_node *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned int len;
} list;

/* Functions implemented as macros */
#define list_length(l) ((l)->len)
#define list_head(l) ((l)->head)
#define list_tail(l) ((l)->tail)
#define list_prev_node(n) ((n)->prev)
#define list_next_node(n) ((n)->next)
#define list_node_value(n) ((n)->value)

#define list_set_dup_proc(l,m) ((l)->dup = (m))
#define list_set_free_proc(l,m) ((l)->free = (m))
#define list_set_match_proc(l,m) ((l)->match = (m))

#define list_dup_proc(l) ((l)->dup)
#define list_free_proc(l) ((l)->free)
#define list_match_proc(l) ((l)->match)

/* Prototypes */
list *list_create(void);
void list_release(list *list);
list *list_add_head(list *list, void *value);
list *list_add_tail(list *list, void *value);
list *list_insert(list *list, list_node *old_node, void *value, int after);
void list_remove(list *list, list_node *node);
list_iter *list_get_iterator(list *list, int direction);
list_node *list_next(list_iter *iter);
void list_release_iterator(list_iter *iter);
list *list_duplicate(list *orig);
list_node *list_search_key(list *list, void *key);
list_node *list_index(list *list, int index);
void list_rewind(list *list, list_iter *li);
void list_rewind_tail(list *list, list_iter *li);

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#ifdef  __cplusplus
}
#endif     

#endif /* __CLOWF_LIST_H__ */

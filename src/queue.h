#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* _T;

struct node {
	_T* data;
	struct node *next;
};

struct ub_queue {
	struct node *front;
	struct node *rear;
	unsigned int count;
	unsigned int is_sync;
	pthread_mutex_t lock;
};

typedef struct ub_queue sync_queue;

void queue_init(struct ub_queue *hq);
void queue_sync_init(struct ub_queue *hq);
void queue_clear(struct ub_queue *hq);
int queue_push(struct ub_queue *hq, _T x);
int queue_empty(struct ub_queue *hq);
_T queue_pop(struct ub_queue *hq);
_T queue_peek(struct ub_queue *hq);

#ifdef __cplusplus
}
#endif
	
#endif /* QUEUE_H_ */

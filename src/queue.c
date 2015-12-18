/*
 * squeue.cpp
 *
 *  Created on: May 12, 2011
 *      Author: boyce
 */

#include "queue.h"

void queue_init(struct ub_queue *hq) {
	hq->is_sync = 0;
	queue_clear(hq);
	hq->front = hq->rear = NULL;
}

void queue_sync_init(struct ub_queue *hq)
{
	hq->is_sync = 1;
	pthread_mutex_init(&hq->lock, NULL);
	queue_clear(hq);
	hq->front = hq->rear = NULL;
}

int queue_push(struct ub_queue *hq, _T x) {

	struct node *newP = (struct node*) malloc(sizeof(struct node));
	if (newP == NULL)
		return -1;

	newP->data = x;
	newP->next = NULL;
	if (hq->is_sync)
		pthread_mutex_lock(&hq->lock);
	if (hq->rear == NULL) {
		hq->front = hq->rear = newP;
	} else {
		hq->rear = hq->rear->next = newP;
	}
	hq->count++;
	if (hq->is_sync)
		pthread_mutex_unlock(&hq->lock);
	return 0;
}

_T queue_pop(struct ub_queue *hq) {
	struct node *p;
	_T temp;
	if (hq->is_sync)
		pthread_mutex_lock(&hq->lock);
	if (hq->front == NULL) {
		if (hq->is_sync)
			pthread_mutex_unlock(&hq->lock);
		return NULL;
	}
	temp = hq->front->data;
	p = hq->front;
	hq->front = p->next;
	hq->count--;
	if (hq->front == NULL) {
		hq->rear = NULL;
	}
	if (hq->is_sync)
		pthread_mutex_unlock(&hq->lock);
	free(p);
	return temp;
}

_T queue_peek(struct ub_queue *hq) {
	if (hq->front == NULL)
		return NULL;
	return hq->front->data;
}

int queue_empty(struct ub_queue *hq) {
	return hq->count == 0;
}

void queue_clear(struct ub_queue *hq) {
	if (hq->is_sync)
		pthread_mutex_lock(&hq->lock);
	struct node *p = hq->front;
	while (p != NULL) {
		hq->front = hq->front->next;
		free(p->data);
		free(p);
		p = hq->front;
	}
	hq->rear = NULL;
	hq->count = 0;
	if (hq->is_sync)
		pthread_mutex_unlock(&hq->lock);
	return;
}


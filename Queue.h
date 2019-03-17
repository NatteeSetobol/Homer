#ifndef _QUEUE_H
#define _QUEUE_H

struct queue_info
{
	int *address;
	struct queue_info *next;
	struct queue_info *prev;
};


typedef struct queue
{
	struct queue_info *head;
	struct queue_info *current;
} Queue;

void QAddToTop(Queue *queue, int *item);
void QAddToBottom(Queue *queue, int *item);
int* QPopBottom(Queue *queue);
void QPopTop(Queue *queue);

#endif

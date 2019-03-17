#include "Queue.h"
#include "memory.h"


void QAddToTop(Queue *queue, int *item)
{
	if (queue->head == 0)
	{
		queue->head = (struct queue_info*) Alloc(sizeof(struct queue_info));
		queue->head->address = item;
		queue->head->next=0;
		queue->head->prev=0;
		queue->current = queue->head;
	} else {
		struct queue_info *newHead = (struct queue_info*) Alloc(sizeof(struct queue_info));
		newHead->address = item;
		newHead->next = queue->current;
		newHead->prev=0;
		queue->head->prev = newHead;
		queue->head = newHead;
	}
}


int* QPopBottom(Queue *queue)
{

	struct queue_info *temp =  queue->current;
	int *results = 0; 

	if (queue->current != 0)
	{
		queue->current = temp->prev;
		if (queue->current)
		{
			queue->current->next = NULL;
		} else {
			queue->head = NULL;
			queue->current = NULL;
		}
	}

	if (temp)
	{
		results = temp->address;

		Free(temp);
		temp=0;
	}

	return  results;
}

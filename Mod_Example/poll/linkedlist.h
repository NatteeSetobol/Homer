#ifndef __LINKED_LIST__
#define __LINKED_LIST__
#include "intrinsic.h"
#include <stdlib.h>
struct link
{
	int id;
	bool free;
	int* address;
	struct link *next;
	struct link *prev;
};

struct list
{
	int count;
	struct link* head;
	struct link* current;
};


int FindChunk(struct list* linkedlist, int* address);
int AddToList(struct list* linkedlist, int* address);
void DeleteFromList(struct list* linkedlist, int index);
int* GetFromList(struct list* linkedlist, int index);
void DeleteList(struct list* linkedlist);


int FindChunk(struct list* linkedlist, int* address)
{
	int count=0;

	for (struct link* iter = linkedlist->head; iter != NULL; iter = iter->next)
	{
		if (iter->address == address)
		{
			free(iter->address);
			iter->address=NULL;
			return iter->id;
		}

		count++;
	}

	return -1;
}

int AddToList(struct list* linkedlist, int* address)
{
	if (linkedlist)
	{
		if (linkedlist->head == NULL)
		{
			linkedlist->head = (struct link*) malloc(sizeof(struct link));
			linkedlist->head->free = false;
			linkedlist->head->address = address;
			linkedlist->head->next = NULL;
			linkedlist->head->prev = NULL;
			linkedlist->current = linkedlist->head;
			linkedlist->head->id = linkedlist->count;
			linkedlist->count++;
		} else {
			linkedlist->current->next = (struct link*) malloc(sizeof(struct link));
			linkedlist->current->next->free= false;
			linkedlist->current->next->address = address;
			linkedlist->current->next->id= linkedlist->count;
			linkedlist->current->next->next = NULL;
			linkedlist->current->next->prev = linkedlist->current;
			linkedlist->current = linkedlist->current->next;
			linkedlist->count++;
		}
	}

	return 1;
}

void DeleteFromList(struct list* linkedlist, int index)
{
	int count=0;

	for (struct link* iter = linkedlist->head; iter != NULL;iter = iter->next)
	{
		if (iter->id == index) 
		{
			iter->free = true;
			free(iter->address);
			iter->address=NULL;
			return ;

		}

		count++;
	}

	return ;
}

int* GetFromList(struct list* linkedlist, int index)
{
	int count=0;

	for (struct link* iter = linkedlist->head; iter != NULL; iter = iter->next)
	{
		if (iter->id == index) 
		{
			return  iter->address;
		}

		count++;
	}

	return NULL;
}

void DeleteList(struct list* linkedlist)
{
	for (struct link* iter = linkedlist->head; iter != NULL; iter)
	{
		struct link* temp = iter;

		if (iter)
		{
			iter = iter->prev;
			if (temp->address)
			{
				free(temp->address);
				temp->address=NULL;
			}
		}

		free(temp);
		temp=NULL;
	}
}

#endif

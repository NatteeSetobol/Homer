#ifndef __ARRAY__H
#define __ARRAY__H

#include "memory.h"
#include "array.h"

struct arrayinfo
{
	int *pointer;
	struct arrayinfo* next;
	struct arrayinfo* prev;
};

typedef struct array
{
	int count;
	struct arrayinfo* head;
	struct arrayinfo* tail;
} Array;


void FreeArrayStack(struct array* Array);
void vAddToArray(struct array* Array, int *pointer, char* filename, int len);
int *GetFromArray(struct array* Array, int index);
void FreeArray(struct array* Array);
void DeleteFromArray(struct array* Array, int index);

#define AddToArray(array, pointer) vAddToArray(array,pointer,__FILE__, __LINE__)

void vAddToArray(struct array* Array, int *pointer, char* filename, int len)
{
	if (Array->head == NULL)
	{
		Array->head = (struct arrayinfo*) vAlloc(sizeof(struct arrayinfo), len, filename);
		Array->head->pointer = pointer;
		Array->head->next=NULL;
		Array->head->prev=NULL;
		Array->tail = Array->head;
	} else {
		Array->tail->next = (struct arrayinfo*) vAlloc(sizeof(struct arrayinfo), len, filename);
		Array->tail->next->pointer = pointer;
		Array->tail->next->next = NULL;
		Array->tail->next->prev = Array->tail;
		Array->tail = Array->tail->next;
	}

	Array->count++;
}

int *GetFromArray(struct array* Array, int index)
{
	struct arrayinfo* temp = Array->head;
	int count=0;

	while(temp != NULL)
	{
		if (count == index)
		{
			return temp->pointer;
		}

		count++;

		temp = temp->next;
	}

	return NULL;
}

void DeleteFromArray(struct array* Array, int index)
{
	struct arrayinfo* temp = NULL;

	temp = Array->head;

	for (int i=0;temp != NULL; temp = temp->next,i++)
	{
		if (index == i)
		{
			struct arrayinfo* nodeToDel =NULL;
			if (Array->head == temp)
			{
				nodeToDel = Array->head;
				Array->head = Array->head->next;
				Array->head->prev = NULL;

			} else 
			if (Array->tail == temp)
			{
				if (Array->tail!=NULL)
				{
					nodeToDel =Array->tail;
					Array->tail = Array->tail->prev;
					Array->count--;
					break;
				}
			} else {
				nodeToDel = temp;
				temp->next->prev = temp->prev;
				temp->next->next = NULL;
				temp->prev->next = temp->next;
			}
			
			if (nodeToDel != NULL)
			{
				Free( (int*) nodeToDel);
				nodeToDel = NULL;
				break;
			}
			
			
		}
	}

}

void FreeArray(struct array* Array)
{
	struct arrayinfo* temp=Array->head;

	while (temp != NULL)
	{
		struct arrayinfo* unlink = Array->head;
		if (Array->head == NULL) break;
		Array->head = Array->head->next;
		if (unlink)
		{
			if (unlink->pointer)
			{
				Free( unlink->pointer);
				unlink->pointer=NULL;
			}


			Free( unlink);
			unlink=NULL;
		}
	}
}

void FreeArrayStack(struct array* Array)
{
	struct arrayinfo* temp=Array->head;

	while (temp != NULL)
	{
		struct arrayinfo* unlink = Array->head;
		if (Array->head == NULL) break;
		Array->head = Array->head->next;
		if (unlink)
		{
			if (unlink->pointer)
			{
				//Free( unlink->pointer);
			}


			Free( unlink);
		}
	}
}
#endif

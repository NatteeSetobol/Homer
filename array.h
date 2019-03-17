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
#endif

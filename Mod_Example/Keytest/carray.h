#ifndef __CARRAY__
#define __CARRAY__

#include <stdlib.h>
#include "memory.h"

struct carray
{
	int count;
	int size;
	int *chunks;
};

int* GetFromCArray(struct carray *cArray, int index);
int CreateCArray(struct carray *cArray);
int AddToCArray(struct carray *cArray, int *addr);
int FreeCArray(struct carray *cArray);
#endif 

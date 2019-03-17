#ifndef MEM_H
#define MEM_H
#define Alloc(size)  vAlloc(size, __LINE__, __FILE__);
#define Free(chunk) vFree(chunk, __LINE__, __FILE__);

#include <stdlib.h>
#include <stdint.h>
//#include "linkedlist.h"
#include "intrinsic.h"

struct Memory_Struct
{
	int size;
	int line;
	char name[1028];

	struct Memory_Struct *next;
	struct Memory_Struct *prev;
};


//int* Allocate( int size);
//void FreeMemory(void* address);

//struct carray memoryList={};

#endif



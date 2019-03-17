#ifndef __MEMORY__H
#define __MEMORY__H


#include <stddef.h>
#include "intrinsic.h"
//#include <windows.h>

struct Memory_Struct
{
	int size;
	int line;
	char name[1028];
	struct Memory_Struct* prev;
	struct Memory_Struct* next;

};



void* vAlloc(size_t size, int line, char* filename);
void vFree(i8* chunk, int line, char* filename);
void vMemoryResults();

#define DEBUG 1

#define Alloc(size)  vAlloc(size, __LINE__, __FILE__);

//#ifdef DEBUG
#define Free(chunk) vFree(chunk, __LINE__, __FILE__);
//#else
//#define Free(chunk) free(chunk);
//#endif
#endif

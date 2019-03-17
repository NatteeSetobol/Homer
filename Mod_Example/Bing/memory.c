//#include <string.h>
#include "string.h"
#include "memory.h"
#include "linkedlist.h"
#include "intrinsic.h"

struct list memoryContainer = {};
int* Allocate( int size);
//void FreeMemory(void* address);

//struct carray memoryList={};

struct Memory_Struct *headMem=NULL;
int FindMemoryStruct(struct list* memoryContainer, int* address);

void *vAlloc(size_t size, int line, char* filename)
{
	#if DEBUG
	struct Memory_Struct *m  = (struct Memory_Struct*)  Allocate(size+sizeof(*m) );

	m->line = line;
	m->size = size;
	strcpy(m->name,filename);
	if (headMem == NULL )
	{
		headMem = m;
		headMem->next=NULL;
		headMem->prev=NULL;
	} else {
		headMem->prev =m;
		m->next = headMem;
		headMem = m;
	}

	return (i8*) m+sizeof(*m);
	#else
	
	void *m = Allocate(size);

	return m;
	#endif
}


void vFree(i8* chunk, int line, char* filename)
{
	#if DEBUG
	if (chunk)
	{
		struct Memory_Struct *mi= (i8*)chunk-sizeof(struct Memory_Struct);
		
		if (mi)
		{
			if (mi->prev == NULL)
			{
				headMem = mi->next;
			} else 
				mi->prev->next = mi->next;
			if (mi->next)
				mi->next->prev = mi->prev;
			if (mi == headMem)
			{
				headMem = headMem->next;
				if (headMem)
				{
					headMem->prev=NULL;
				} else {
					headMem = NULL;
				}
			} else {
				if (mi->prev)
				{
					mi->prev->next = mi->next;
					if (mi->next)
					{
						if (mi->next->prev)
						{
							mi->next->prev = mi->prev;
						}
					} else {
				
					}
				} else {
					
				}
			}
		
		}
		FreeMemory(mi,mi->size);
	}
	#else 
	FreeMemory(chunk,sizeof(chunk));
	#endif
}

int FindMemoryStruct(struct list* memoryContainer, int* address)
{
	return -1;
}



void vMemoryResults()
{
	#if DEBUG
	struct Memory_Struct *m = NULL;

	m = headMem;

	while (m)
	{
		struct Memory_Struct  *chunk = m;
		printf("leaked in %s, line %i\n", m->name,m->line);
		m=m->next;
		if (chunk)
		{
			FreeMemory(chunk, chunk->size);
			chunk=NULL;
		}
	}
	#endif
}

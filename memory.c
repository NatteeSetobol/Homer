//#include <string.h>

#include "memory.h"

#define DEBUG 1

struct Memory_Struct *headMem=NULL;
int FindMemoryStruct(struct list* memoryContainer, int* address);


void *vAlloc(size_t size, int line, char* filename)
{
	#if DEBUG
	struct Memory_Struct *m  = (struct Memory_Struct*)  malloc(size+sizeof(*m) );
	memset(m,0,size+sizeof(*m));

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
	
	void *m = malloc(size);

	memset(m,0,size);
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
		free(mi);
	}
	#else 
	free(chunk);
	#endif
}
/*
int FindMemoryStruct(struct list* memoryContainer, int* address)
{
	return -1;
}
*/



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
			free(chunk);
			chunk=NULL;
		}
	}
	#endif
}


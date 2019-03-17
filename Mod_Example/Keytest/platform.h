#ifndef __PLATFORM__
#define __PLATFORM__
#include "memory.h"
#include "intrinsic.h"

char* StripToHostName(char* host);

struct File
{
	char *name;
	i8 *content;
	int size;
	ui32 compressedSize;
};


void* Allocate( size_t size);
void FreeMemory(void* address, size_t size);
int ReadEntireFile(struct File* file);

#include "unix_platform.h"
#endif

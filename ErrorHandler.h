#ifndef __ERROR_HANDLER__
#define __ERROR_HANDLER__
#include "string.h"

struct Error_Handler
{
	char* error;
	int line;
	char filename[256];
};


struct Error_Handler* HandleError(int line, char* filename);

inline struct Error_Handler* HandleError(int line, char* filename)
{
	struct Error_Handler* results = (struct Error_Handler*) Alloc(sizeof(struct Error_Handler));

	results->line = line;
	StrCpy(results->filename, filename);

	return results;
}

#define Error_Handler() HandleError(__LINE__,__FILE__)

#endif

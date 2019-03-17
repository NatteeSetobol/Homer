#ifndef ___ASSERT___
#define ___ASSERT___

#include "memory.h"

#define Assert(x) if(!(x)){printf("assertion in %s on line %i",__FILE__, __LINE__); vMemoryResults();  exit(0);}
#define assert(x) if(!(x)){printf("assertion in %s on line %i",__FILE__, __LINE__);  vMemoryResults(); exit(1);}
#endif

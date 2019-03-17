#include "carray.h"


int CreateCArray(struct carray *cArray)
{
	cArray->count = 0;
	cArray->size = 2;
	cArray->chunks =(int*) Alloc(cArray->size * sizeof(int));
}

int AddToCArray(struct carray *cArray, int *addr)
{
	
	if (cArray->chunks == NULL)
	{
		CreateCArray(cArray);
	}

	if (cArray->count < cArray->size)
	{
		cArray->chunks[cArray->count] = addr;
		cArray->count++;
	} else {

		int oldSize=0;
		int *tempChunk = cArray->chunks;
		oldSize = cArray->size;
		cArray->size *=2;
		cArray->chunks = (int*) Alloc( (cArray->size * sizeof(int)) );
		memcpy(cArray->chunks,tempChunk,oldSize*sizeof(int));

		cArray->chunks[cArray->count] = addr;
		cArray->count++;

		if (tempChunk)
		{
			Free(tempChunk);
			tempChunk=NULL;
		}
	}
}


int *GetFromCArray(struct carray *cArray, int index)
{
	return cArray->chunks[index];
}

int FreeCArray(struct carray *cArray)
{
	cArray->count = 0;
	cArray->size = 0;

	if (cArray->chunks)
	{
		Free(cArray->chunks);
		cArray->chunks=NULL;
	}
}


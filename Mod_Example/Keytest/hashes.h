#ifndef __HASHES_H__
#define __HASHES_H__

#ifndef MAX_HASH
#define MAX_HASH 1025
#endif

struct hash_struct
{
	char *key;
	int id;
	int count;
	int value;
	int index;
	int* pointer;
	struct hash_struct *next;
};

struct hash_struct hashTable[MAX_HASH]={};


int HashStringFunction(char *string)
{
	int result=0;
	char *addofStr = string;
	int totalValue = 0;
	int pos=0;

	for (addofStr; *addofStr !=  0; addofStr++,pos++)
	{
		totalValue += (int) (*addofStr);
	}

	result = totalValue;

	if (result < -1)
	{
		result *= -1;
	}


	return result;
}


void AddToHash(char* key, int* value)
{
	char tempChar[25] ={};

	int hashValue  = HashStringFunction(key);
	int hashIndex = 0;

	if (hashValue == 0)
	{
		hashIndex = 0;
	} else {
		hashIndex = MAX_HASH / hashValue ;

		if (hashIndex < 0)
		{
			hashIndex *= -1;
		}
	}

	if (hashIndex >= MAX_HASH)
	{
		hashIndex = MAX_HASH / hashIndex;
		if (hashIndex < 0)
		{
			hashIndex *= -1;
		}
	}

	//printf("%i\n", hashIndex);
	
	if (hashTable[hashIndex].pointer != NULL)
	{
		struct hash_struct *testing= Alloc(sizeof(struct hash_struct));
		testing->index = hashIndex;
		testing->pointer =  value;
		testing->next = NULL;

		hashTable[hashIndex].count++;
		testing->value = hashValue * hashTable[hashIndex].count;
		testing->next = hashTable[hashIndex].next;
		hashTable[hashIndex].next =testing;

	} else {
		hashTable[hashIndex].index = hashIndex;
		hashTable[hashIndex].pointer =  value;
		hashTable[hashIndex].next = NULL;
		hashTable[hashIndex].value = hashValue;
		hashTable[hashIndex].count = 0;
	}
}


int* GetFromHash(char* key)
{
	int *result=NULL;
	int hashValue =HashStringFunction(key);
	int index = MAX_HASH / hashValue;

	if (hashTable[index].pointer != NULL)
	{
		//struct LZ_Info *info = (struct LZ_Info*) hashTable[index]->pointer;
		//char temper[25] = {};
		//temper[0] = info->chr;

		if (hashTable[index].value == hashValue)
		{
			result = (char*) hashTable[index].pointer;
		} else {
			struct hash_struct *next = hashTable[index].next;
			int count = 0;

			while (next != NULL)
			{
				count++;
				if (next)
				{
					if (next->value == hashValue * count )
					{
						result = next->pointer;
						break;
					}

					next = next->next;
				}
			}
		}
	}

	return result;
}


#endif

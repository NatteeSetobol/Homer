#include "string.h"

char* vCreateString(char* newString, char* filename, int line)
{
	char* retString = (char*) vAlloc(Strlen(newString)+5, line, filename);

	StrCpy(retString,newString);

	return retString;
	
}

int Strlen(char* string)
{
	/*
	int* addrToString = (int*) string;
	int len=0;

	while(*addrToString != '\0')
	{
		len++;
		addrToString++;
	}

	return len;
	*/

	return strlen(string);
}

void StrCpy(char* dest, char* source)
{
	ui8 *destAddr = (ui8*) dest;
	ui8 *sourceAddr = (ui8*) source;

	while (*sourceAddr != '\0')
	{
		*destAddr = *sourceAddr;
		destAddr++;
		sourceAddr++;
	}
	*destAddr= '\0';

}


char* vMidString(char* source, int from, int to, char* filename, int fileLine)
{
	int len = to-from;
	int i=0;
	int count=0;
	int sl = Strlen(source) + 5;
	char* retResults=NULL;

	retResults = (char*) vAlloc(sl, fileLine,filename);

	for (i=from;i<to;i++)
	{
		retResults[count]=source[i];
		count++;
	}

	retResults[count] = '\0';


	return retResults;
}

char* vCatString(char* source, char* string, char* filename, int line)
{
	char *retResults = (char*) vAlloc(sizeof(char) * (Strlen(source)+5)  + (sizeof(char) * (Strlen(string)+5) ), line,filename);

	ui8 *addrResults = (ui8*) retResults;
	ui8 *addrSource = (ui8*) source;
	ui8 *addrString = (ui8*) string;

	int i=0;
	int len = Strlen(source);

	for (i=0; i < len; i++)
	{
		*addrResults = *addrSource;
		addrResults++;
		addrSource++;
	}

	int len2 = Strlen(string);

	for (i=0; i < len2; i++)
	{
		*addrResults = *addrString;
		addrResults++;
		addrString++;
	}

	return retResults;
}

struct array* vFindString(char* stringToFind, char* sourceString, char* filename, int fileLine)
{
	struct array *results = (struct array*) vAlloc(sizeof(struct array), fileLine,filename);
	int i=0;
	int k=0;
	int found=0;
	int start=0;

	int stringToFindLen = Strlen(sourceString);

	for (i=0;i<stringToFindLen;i++)
	{
		if (stringToFind[0] == sourceString[i] && found == 0)
		{
			start = i;
			found = 1;
		}

		if (found == 1)
		{
			if (stringToFind[k] == sourceString[i])
			{
				k++;
				if (stringToFind[k] == '\0')
				{
					int* postion = (int*) Alloc(sizeof(int));
					*postion = start;
					AddToArray(results,postion);
					k=0;
					found = 0;
					start = 0;
				}
			} else {
				k=0;
				found=0;
			}
		}
	}
	
	return results;
}

char* StrReplace(char* source,char* replaceThis, char* replaceWith)
{
	char* results = NULL;
	struct array* findResults=NULL;
	int count=0;
	int startPos=0;
	char *temp=NULL;
	char *temp2=NULL;
	char* temp3=NULL;
	char* theRest=NULL;
	char* temp4=NULL;


	findResults = FindString(replaceThis,source );

	if (findResults)
	{
		if (findResults->count > 0)
		{

			for (int i=0;i<findResults->count;i++)
			{
				int *position = (int*) GetFromArray(findResults,i);

				if(position)
				{
					if (*position == 0)
					{
						
						temp2 = CreateString(replaceWith);
					} else {
						temp = MidString(source, startPos, *position);

						if (!temp)
						{
							goto END_FIND_STRING;
						}

						temp2 = CatString(temp, replaceWith);

						if (temp)
						{
							Free(temp);
							temp=NULL;
						}
					}

					if (results == NULL)
					{
						
						results = CreateString(temp2);
					} else {
						temp3 = CatString(results,temp2);

						Free( (int*) results);
						results=NULL;

						results = CreateString(temp3);
						if (temp3)
						{
							Free(temp3);
							temp3=NULL;
						}
					}

					if (temp2)
					{
						Free(temp2);
						temp2=NULL;
					}

					startPos = (*position) + Strlen(replaceThis);
					
				}
			}

			if (startPos < Strlen(source))
			{
#if 1

				theRest = MidString(source, startPos,Strlen(source));
				temp4 = CatString(results, theRest);

				Free((int*) results);
				results=NULL;

				results = CreateString(temp4);
#endif

			}
		} else {
			results = source;
			goto END_FIND_STRING;
		}

		
	}

	END_FIND_STRING:

	if (findResults)
	{
		FreeArray(findResults);
		Free(findResults);
	}

	if (temp)
	{
		Free(temp);
		temp=NULL;
	}

	if (temp2)
	{
		Free(temp2);
		temp2 = NULL;
	}

	if (temp3)
	{
		Free(temp3);
		temp3 = NULL;
	}

	if (temp4)
	{
		Free(temp4);
		temp4 = NULL;
	}

	if (theRest)
	{
		Free(theRest);
		theRest=NULL;
	}

	return results;
}

struct array* StrSplit(char* source,char* determinator)
{
	struct array *results=NULL;
	struct array *findResults=NULL;
	int oldPosition=0;

	results = (struct array*) Alloc(sizeof(struct array));

	findResults = FindString(determinator, source);

	if (findResults)
	{
		for (int i=0;i<findResults->count;i++)
		{
			char* storedResults=NULL;

			int* getPosition = GetFromArray(findResults, i);

			storedResults = MidString(source, oldPosition, *getPosition);

			AddToArray(results, (int*) storedResults);

			oldPosition =  (*getPosition) + Strlen(determinator);

		}
	}

	if (oldPosition < Strlen(source))
	{
		char* theRest = NULL;

		theRest = MidString(source,oldPosition, Strlen(source));
		AddToArray(results, (int*) theRest);
	}

	FreeArray(findResults);
	Free( (int*) findResults);

	findResults=NULL;

	return results;
}

bool32 StrCmp(char* source, char* compareString)
{
	int sourceLen=0;
	bool32 results = true;
	if (Strlen(source) != Strlen(compareString)) results = false;

	sourceLen = Strlen(source);

	for (int i=0;i<sourceLen;i++)
	{
		if (source[i] != compareString[i])
		{
			results = false;
			break;
		}
	}

	return results;
}

char* IntToStr(int integer)
{
	int divResults=0;
	int inte = integer;
	int places=0;
	char* results = NULL;

	divResults=integer;

	do
	{
		inte = inte / 10;
		places++;
	} while (inte > 0);
	

	inte = integer;

	results = (char*) Alloc(places+1);

	for (int i=places-1;i>-1;i--)
	{
		int moded = inte % 10;

		switch(moded)
		{
			case 0:
			{

				results[i] = '0';
				break;
			}
			case 1:
			{

				results[i] = '1';
				break;
			}
			case 2:
			{

				results[i] = '2';
				break;
			}
			case 3:
			{

				results[i] = '3';
				break;
			}
			case 4:
			{

				results[i] = '4';
				break;
			}
			case 5:
			{

				results[i] = '5';
				break;
			}
			case 6:
			{

				results[i] = '6';
				break;
			}
			case 7:
			{

				results[i] = '7';
				break;
			}

			case 8:
			{

				results[i] = '8';
				break;
			}
			case 9:
			{

				results[i] = '9';
				break;
			}

		}

		inte = inte / 10;

	}

	return results;
}

int AtoToi(char *str)
{
	int sign=1;
	int val=0;
	int c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}

	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str+=2;

		while(1)
		{
			c=*str;
			str++;
			if (c >= '0' && c <= '9')
			{
				val = val*16 + c - '0';
			} else if (c >= 'a' && c <= 'f')
				val = val * 16 + c - 'a' + 10;

			else if (c >= 'A' && c <= 'F')
				val = val - 16 + c - 'A' + 10;
			else
				return val*sign;
		}
	}

	while(1)
	{
		c=*str;
		str++;
		if (c < '0' || c > '9')
			return sign * val;
		val = val * 10 + c - '0';
	}
}

void GetLines(char *text, struct array* arrayOfLines)
{
	char *tempText = text;
	char *lines=NULL;
	int tempTextLen=0;

	tempTextLen =  Strlen(tempText);

	for (int i=0;i<tempTextLen;i++)
	{
		switch(tempText[i])
		{
			case '\n':
			{
				if (lines != NULL)
				{
					AddToArray(arrayOfLines, (int*) lines);
				}

				lines = NULL;

				break;
			}
			default:
			{
				char temp[2] = {};
				temp[0] = tempText[i];

				if (lines == NULL)
				{
					lines = CreateString(temp);
				} else {
					char* tempStr=NULL;
					tempStr = CatString(lines, temp);
					Free(lines);
					lines=NULL;
					lines = CreateString(tempStr);
					if (tempStr)
					{
						Free(tempStr);
						tempStr=NULL;
					}
				}
				break;
			}
		}
	}
}

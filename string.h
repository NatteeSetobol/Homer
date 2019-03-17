#ifndef __STRING__
#define __STRING__
#include <string.h>
#include "memory.h"
#include "array.h"
#include "intrinsic.h"

char* vCatString(char* source, char* string, char* filename, int line);
char* vCreateString(char* newString, char* filename, int line);
int Strlen(char* string);
void StrCpy(char* dest, char* source);
char* vMidString(char* source, int from, int to, char* filename, int fileLine);
struct array* vFindString(char* stringToFind, char* sourceString, char* filename, int fileLine);
char* StrReplace(char* source,char* replace, char* replaceWith);
struct array* StrSplit(char* source,char* determinator);
bool32 StrCmp(char* source, char* compareString);
char* IntToStr(int integer);
int AtoToi(char *str);
void GetLines(char *text, struct array* arrayOfLines);

#define CatString(source,string) vCatString(source,string,__FILE__,__LINE__);
#define FindString(stringToFind, sourceString)  vFindString(stringToFind, sourceString, __FILE__,__LINE__)
#define MidString(source,from,to) vMidString(source,from,to,__FILE__,__LINE__) 
#define CreateString(newstring)  vCreateString(newstring,__FILE__,__LINE__);
#endif

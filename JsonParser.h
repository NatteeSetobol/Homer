#ifndef JSON_PARSER
#define JSON_PARSER
#include "assertion.h"

struct JSON_Tree;

enum Json_Type{ JNONE,JLIST, JDICTIONARY, JARRAY };

struct JSON_Branch
{
	int type;
	char* text;
	char* value;
	struct JSON_Branch *prev;
	struct JSON_Branch *next;
	struct JSON_Tree *subTree;
};


struct JSON_Tree
{
	int type;
	struct JSON_Branch *root;
	struct JSON_Branch *current;
	struct JSON_Tree *prevTree;
	struct JSON_Tree *subTree;
};

struct Json_Branch
{
	Json_Type type;
	char *key;
	char *value;
	struct Json_Branch *prev;
	struct Json_Branch *next;

	struct Json_Branch *head;
	struct Json_Branch *current;

	struct Json_Branch *prevBranch;
	struct Json_Branch *subBranch;
};

/*
struct JSON_Branch
{
	int type;
	char* text;
	char* value;
	struct JSON_Branch* root;
	struct JSON_Branch* current;
	struct JSON_Branch* next;
	struct JSON_Branch* branch;
};
*/


enum JSON_Token_Type
{
	DATA,
	LIST_START,
	LIST_END,
	ARRAY_START,
	ARRAY_END,
	IDENTIFIER,
	SPLITTER,
	NEXT,
	SPACE
};

struct JSON_Token
{
	size_t length;
	enum JSON_Token_Type type;
	char *JsonData;
	char token;
};

struct JSON_Tokenizer
{
	char *at;
};

void JSON_Parse(char *json, struct Json_Branch* jBranch, struct Error_Handler *error);
struct Json_Branch* Json_Find(struct Json_Branch* branch,struct array* arrayOfSearch);
struct Json_Branch *Json_DelTree(struct Json_Branch* branch);
char*  JSON_GetValueFromKey(struct JSON_Tree* tree, struct array* list);
struct JSON_Branch *JSON_DelBranch(struct JSON_Branch* branch);
inline void ParseJson(char* Json, struct JSON_Tree* jsonTree, struct Error_Handler *error);
struct JSON_Token GetJSONToken(struct JSON_Tokenizer *jsonTokenizer);
void JSON_AddBranch(struct JSON_Tree *jsonTree,int type, char* text);
void JSON_AddTree(struct JSON_Branch* branch, int type);
void JSON_AddTree2(struct JSON_Tree* tree, int type);
void DelTree(struct JSON_Tree *tree);
struct Json_Branch* Json_DelBranch(Json_Branch* branch);
void DelPointerTree(struct JSON_Tree *tree);
void Json_AddBranch(struct Json_Branch* branch, struct Json_Type type, char* key, char* value);
void Json_Free(struct Json_Branch* branch);
#endif

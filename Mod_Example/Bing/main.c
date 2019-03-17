#include <stdio.h>
#include "platform.h"
#include "memory.h"
#include "string.h"
#include "carray.h"
#include "array.h"
//#include "bing.h"
#include "hashes.h"
#include "compression.h"
#include "intrinsic.h"

#if 1
#define functionPointer(name,varName,returnType, params) returnType  (*name)params  = varName;

struct hashIndex
{
	int index1;
	int index2;
};


//#define GetHash(hash,name) name[HashStringFunction(hash)]
//#define SetHash(hash,name,value) name[HashStringFunction(hash)] = value;
#endif




int main()
{
	#if 0
	struct LZ_Info l1 = {};
	l1.letters = CreateString("a");
	AddToHash("a", &l1);
	
	struct LZ_Info l2 = {};
	l2.letters = CreateString(" ");
	AddToHash(" ",&l2);
	
	struct LZ_Info l3 = {};
	l3.letters = CreateString("r");
	AddToHash("r",&l3);
	
	struct LZ_Info l4 = {};
	l4.letters = CreateString("o");
	AddToHash("o",&l4);

	struct LZ_Info l5 = {};
	l5.letters = CreateString("s");
	AddToHash("s",&l5);

	struct LZ_Info l6 = {};
	l6.letters = CreateString("e");
	AddToHash("e",&l6);
	
	struct LZ_Info l7 = {};
	l7.letters = CreateString(" ");
	AddToHash("motto",&l7);
	

	struct LZ_Info *lzHash = GetFromHash("e");


	if (lzHash)
	{
		printf("found %s\n", lzHash->letters);
	} else {
		printf("not found\n");
	}
	#endif


	#if 0
	//RunFunctionByParam(myfakefunction);
	char* homer = CreateString("Homer");
	//SetHash("a", name2, homer);

	AddToHash("a");

	char * t = GetHash("a");

	printf("%s\n", t);

	int i = HashStringFunction("bba");
	
	printf("index of the real function is %i\n",i);
	#endif

	Compress("./twitch.pcap", "none.txt");

	//vMemoryResults();

//	Decompress("./none.txt", "decompressed.txt");


	//vMemoryResults();

	/*
	char *name = Alloc(255);
	int x=2452;
	int y=1456;

	int hashFunc = x & 256;

	printf("%i\n", hashFunc);

	Free(name);
	name=NULL;
	*/
	#if 0
	char *trans = NULL;
	
	trans = Translation("民事再生手続き中");

	struct BingVoiceInfo bingVoiceInfo={};
	bingVoiceInfo.voice = FEMALE;
	bingVoiceInfo.lang = CreateString("ja-JP");
	bingVoiceInfo.mediatype = WAV;
	
	Speak("喫驚り", &bingVoiceInfo);

	if (trans)
	{
		Free(trans);
		trans=NULL;
	}
	
	#endif
	

	//vMemoryResults();
}



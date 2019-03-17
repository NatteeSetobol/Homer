#include "ssl.h"
#include "array.h"
#include "plugins.h"
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "string.h"
#include "bing.h"

void SendToChat(char *message,char* recv,struct irc_commands *commands );

ON_PRIVMSG(OnPrivMsg)
{
	struct array *quotation =  NULL;
	char *replacedText = NULL;
	char * trans=NULL;

	quotation = FindString("\"", msg);

	if (quotation->count > 0)
	{
		replacedText = StrReplace(msg,"\"","\\\"");
		
		if (quotation)
		{
			FreeArray(quotation);

			Free(quotation);
			quotation=NULL;
		}
	} else {
		replacedText =  CreateString(msg);
	}

	
	trans = Translation(replacedText);


	if (trans)
	{
		int charMatches = 0;
		int matchPercentage=0;
		int slen = 0;

		if (strlen(trans) > strlen(replacedText))
		{
			slen = strlen(replacedText);
		} else {
			slen = strlen(trans);
		}

		for (int i=1;i < slen -1;i++)
		{
			if (tolower(trans[i]) == tolower(replacedText[i-1]))
			{
				charMatches++;
			}
		}

		matchPercentage = strlen(msg) - charMatches;

		//printf("character matched: %i %i = %i\n", strlen(msg), charMatches, matchPercentage);
		if (matchPercentage > 5)
		{
			char translatedTextToSend[256]={};

			if (strlen(trans) < 128)
			{
				sprintf(translatedTextToSend,"Translation %s\n", trans);
				SendToChat(translatedTextToSend,from,commands);
			}
		}

		Free(trans);
		trans = NULL;
	}

	if (replacedText)
	{
		Free(replacedText);
		replacedText = NULL;
	}

	if (quotation)
	{
		FreeArray(quotation);
		Free(quotation);
		quotation = NULL;
	}
}


void SendToChat(char *message,char* recv,struct irc_commands *commands )
{
	char *fullCommand = NULL;
	char *temp = NULL;

	fullCommand = CreateString("PRIVMSG ");
	temp = CatString(fullCommand, recv);

	Free(fullCommand);
	fullCommand = NULL;

	fullCommand = CatString(temp, " :");

	Free(temp);
	temp=NULL;

	temp = CatString(fullCommand, message);

	Free(fullCommand);
	fullCommand=NULL;

	fullCommand = CatString(temp, "\r\n");

	Free(temp);
	temp=NULL;

	commands->SendMessage(fullCommand);

	Free(fullCommand);
	fullCommand = NULL;
}
	

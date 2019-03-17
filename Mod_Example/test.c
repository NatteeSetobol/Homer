#include <stdio.h>
#include "memory.h"
#include "string.h"
#include "plugins.h"
/*
gcc -Wall -fPIC -c example.c -I../../openssl-0.9.8zg/include -w -g
gcc -I../../openssl-0.9.8zg/include -shared -Wl,-install_name,myexampleso.1 -o myexample.so.1.0 ytsearch.o  -lssl -lcrypto -g
*/

void SendToChat(char *message,char* recv,struct irc_commands *commands );

ON_NICK(OnNick)
{
	printf("on Nick\n");
}

ON_NOTICE(OnNotice)
{
	printf("on notice\n");
}

ON_QUIT(OnQuit)
{
	printf("on quit\n");
}

ON_JOIN(OnJoin)
{
	printf("onJoin\n");
}

ON_KICK(OnKick)
{
	printf("onKick\n");
}


ON_PRIVMSG(OnPrivMsg)
{
	if (strcmp(msg,"!testing") == 0)
	{
		SendToChat("oh this is jaust a test", from,commands);
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

	printf("%s", fullCommand);
	commands->SendMessage(fullCommand);

	Free(fullCommand);
	fullCommand = NULL;
}



#include <unistd.h>
#include <termios.h>
#include "ssl.h"
#include "array.h"
#include "plugins.h"
#include "JsonParser.h"
#include "ErrorHandler.h"
#include "string.h"

//https://tmi.twitch.tv/group/user/popcorn0x90/chatters

struct messages
{
	char* message;
	char* nick;
};

struct messages storedMsg[9]={};
int storedMsgCount = 0;

int kbhit();
void SendToChat(char *message,char* recv,struct irc_commands *commands );
void gotoxy(int x,int y);
bool started = false;

int chatX=0;
int chatY=1;

ON_KEYPRESS(OnKeyPress)
{
	if (key == 'c')
	{
		system("clear");
	}
	if (key >= '0' && key <= '9')
	{
		gotoxy(100,3);
		printf("you pressed %c\n", key);
	}
}

ON_PRIVMSG(OnPrivMsg)
{
	char *nickUser=NULL;
	struct array *findNickUser = NULL;

	findNickUser = FindString("!",user);

	if (findNickUser)
	{
		if (findNickUser->count > 0)
		{
			int *endExc = GetFromArray(findNickUser,0);

			nickUser = MidString(user,0,*endExc);
		}

		FreeArray(findNickUser);
		Free(findNickUser);
		findNickUser=NULL;

	}

	storedMsgCount++;
	gotoxy(chatX,chatY);
	printf("%i. %s : %s\n",storedMsgCount+1, nickUser, msg);
	chatY++;

	//sleep(2);

	if (nickUser)
	{
		Free(nickUser);
		nickUser=NULL;
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

int kbhit()
{
	struct timeval tv = {};
	fd_set read_fd={};
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&read_fd);
	FD_SET(0,&read_fd);
	select(STDIN_FILENO+1,&read_fd,NULL,NULL,&tv);
	return FD_ISSET(STDIN_FILENO,&read_fd);
}

void gotoxy(int x,int y)
{
	printf("%c[%d;%df",0x1B,y,x);
}
	

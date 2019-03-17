#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include "memory.h"
#include "string.h"
#include "intrinsic.h"
#include "ErrorHandler.h"
#include "ssl.h"
#include "token.h"
#include "plugins.h"

struct Irc_Info
{
	char *server;
	int port;
	char *nick;
	char *name;
	char* pass;
	char* owner;
	struct array rooms;
	bool32 isLoaded;
};

struct Stack_Info
{
	i8 *pointer;
	struct Stack_Info *next;
	struct Stack_Info *prev;
};

struct Stack
{
	struct Stack_Info *top;
	struct Stack_Info *bottom;
};

struct Module
{
	bool isLoaded;
	char *name;
	struct stat lastWriteTime;
	void* handler;

	on_privmsg *On_PrivMsg;
	on_join *On_Join;
	on_notice *On_Notice;
	on_quit *On_Quit;
	on_nick *On_Nick;
	on_kick *On_Kick;
	on_mode *On_Mode;
	on_topic *On_Topic;
	on_invite *On_Invite;
	on_keypress *On_KeyPress;
};

struct Options
{
	struct Stack* stack;
	bool32 *running;
};

struct Update_Mod_Info
{
	char* dirName;
	struct stat dirLastMod;
	struct array* mods;
	bool *isRunning;
};


bool running = true;
struct Stack stack={};

struct stat NixGetLastWriteTime(char* file);
struct sslinfo sslInfo={};
bool32 IsAdding = false;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition=PTHREAD_COND_INITIALIZER;
struct Irc_Info ircInfo={};
struct array soList={};
struct irc_commands ircCommands={};

void LoadModCalls(struct Module *mod);
void UpdateMods(void* arg);
void LoadLibsFromDir(char *dirName, struct array * soList);
void GetInfoFromFile(struct Irc_Info *ircInfo);
void ParseSender(char* sentence, char* theRest);
void ParseCommand(char* sender,char* nextToParse,char* theRest);
void HandleMessages(void* args);
void Push(struct Stack *stack, i8* pointer);
struct Stack_Info *Pop(struct Stack *stack);
int Send(char* message, SSL *connectionState);
void ParseMessage(char *getMessage);
void DoCommands(char* sender,char* command, char* reciever, char* message);
void SendMessage(char* message);
void ChangeMode(int dir);
void gotoxy(int x,int y);
int kbhit();
int HandleKeys(bool32 *isRunning);


int main()
{
	struct Error_Handler *error=NULL;
	char* command=NULL;
	char* temp=NULL;
	pthread_t pthread=NULL;
	pthread_t modThread=NULL;
	pthread_t keyThread=NULL;
	char* sendPass=NULL;
	char* tempSendPass=NULL;
	struct Options options={};
	struct stat dirLastMod={};
	struct Update_Mod_Info updateModInfo = {};

	error = Error_Handler();

	ChangeMode(1);
	system("clear");

	printf("* Loading Configuration...\n");
	GetInfoFromFile(&ircInfo);

	if (ircInfo.isLoaded == false)
	{
		printf("* Error Please define a server in config.txt\n");
		goto done;
	}

	printf("* Loading Modules..\n");

	LoadLibsFromDir("./mods/",&soList);

	printf("* %i Modules found\n", soList.count);

	for (int modIndex=0; modIndex < soList.count;modIndex++)
	{
		struct Module* modFromArray = GetFromArray(&soList,modIndex);

		modFromArray->handler = dlopen(modFromArray->name, RTLD_LAZY);
		
		if (!modFromArray->handler)
		{
			printf("* Error can not load the module: %s\n",modFromArray->name);
			modFromArray->isLoaded = false;
		} else {

			modFromArray->lastWriteTime = NixGetLastWriteTime(modFromArray->name);
			modFromArray->isLoaded = true;

			LoadModCalls(modFromArray);

		}
	}

	printf("* Hooking Apis\n");
	/*
	NOTES(): Commands that are send to modules
	*/
	ircCommands.SendMessage = &SendMessage;
	ircCommands.GotoXY = &gotoxy;

	printf("* Connecting to %s on %i\n", ircInfo.server, ircInfo.port);

	if (SSLInit(&sslInfo,ircInfo.server,ircInfo.port, error) == false)
	{
		printf("* Error! can not init SSL.\n");
		goto done;
	}
	

	if (ircInfo.pass)
	{

		printf("* Sending Auth Token..\n");

		sendPass = CreateString("PASS ");

		tempSendPass = CatString(sendPass,ircInfo.pass);
		Free(sendPass);
		sendPass=NULL;

		sendPass = CatString(tempSendPass,"\r\n");

		Free(tempSendPass);
		tempSendPass=NULL;


		Send(sendPass, sslInfo.ssl);


	}

	temp = CreateString("NICK ");

	command = CatString(temp, ircInfo.nick);

	if (temp)
	{
		Free(temp);
		temp=NULL;
	}

	temp = CatString(command,"\r\n");

	if (command)
	{
		Free(command);
		command=NULL;
	}

	//NOTE(): Send to server
	Send(temp, sslInfo.ssl);

	command = CreateString("USER ");

	if (temp)
	{
		Free(temp);
		temp=NULL;
	}

	temp = CatString(command,ircInfo.name);

	if (command)
	{
		Free(command);
		command=NULL;
	}

	command = CatString(temp," one one one\r\n");

	Send(command,sslInfo.ssl);

	if (command)
	{
		Free(command);
		command=NULL;
	}



	ircCommands.ownerName = ircInfo.owner;
	options.stack = &stack;

	updateModInfo.dirName = CreateString("./mods/");
	updateModInfo.dirLastMod;

	updateModInfo.mods = &soList;

	pthread_create(&keyThread,NULL,&HandleKeys, NULL );
	pthread_create(&pthread,NULL,&HandleMessages,(void*) &options);
	pthread_create(&modThread,NULL,&UpdateMods,(void*) &updateModInfo);

	char c='\0';

	while(running)
	{
		if (kbhit())
		{
			c = getchar();

			for (int modIndex=0; modIndex < soList.count;modIndex++)
			{
				struct Module* modFromArray = GetFromArray(&soList,modIndex);
				if (modFromArray)
				{
					if (modFromArray->isLoaded)
					{
						if (modFromArray->On_KeyPress)
						{
							modFromArray->On_KeyPress(c,&ircCommands);
						}
					}
				}
			}

			switch(c)
			{
				case 'q':
				{
					printf("* Shutting down..Please wait@\n");
					running = false;
					pthread_join(modThread,NULL);
					pthread_join(pthread,NULL);

					//pthread_join(keyThread,NULL);
					break;
				}
			}
		}
	}

	done:
	ChangeMode(0);

	FreeArray(&ircInfo.rooms);
	//FreeArray(&soList);

	if (sendPass)
	{
		Free(sendPass);
		sendPass=NULL;
	}
	if (temp)
	{
		Free(temp);
		temp=NULL;
	}

	if (updateModInfo.dirName)
	{
		Free(updateModInfo.dirName);
		updateModInfo.dirName=NULL;
	}
	if (error)
	{
		if (error->error)
		{
			Free(error->error);
			error->error=NULL;
		}
		Free(error);
		error=NULL;
	}

	if (ircInfo.server)
	{
		Free(ircInfo.server);
		ircInfo.server=NULL;
	}

	if (ircInfo.nick)
	{
		Free(ircInfo.nick);
		ircInfo.nick=NULL;
	}
	
	if (ircInfo.name)
	{
		Free(ircInfo.name);
		ircInfo.name=NULL;
	}
	
	if (ircInfo.pass)
	{
		Free(ircInfo.pass);
		ircInfo.pass=NULL;
	}

	
	for (int modIndex=0; modIndex < soList.count;modIndex++)
	{
		struct Module* modFromArray = GetFromArray(&soList,modIndex);

		if (modFromArray)
		{
			if (modFromArray->name)
			{
				Free(modFromArray->name);
				modFromArray->name = NULL;
			}
			Free(modFromArray);
			modFromArray=NULL;
		}
	}



	FreeArrayStack(&soList);

	struct Stack_Info *oldPop=NULL;
	IsAdding=false;


	printf("* Clearing up Stack\n");

	do 
	{
		if (oldPop)
		{
			if (oldPop->pointer)
			{
				Free(oldPop->pointer);
				oldPop->pointer=NULL;
			}
			Free(oldPop);
			oldPop=NULL;
		}

		oldPop = Pop(&stack);
	} while (oldPop != NULL);


	printf("* See you next time!\n");

	vMemoryResults();
	
}


int Send(char* message, SSL *connectionState)
{
	if (SSL_write(connectionState,message,strlen(message)) == 0)
	{
		printf("* Error can't not send message\n");
	}
}

void Push(struct Stack *stack, i8* pointer)
{
	pthread_mutex_lock(&mutex);
	IsAdding=true;

	if (stack->top == NULL)
	{
		stack->top = (struct Stack_Info*) Alloc(sizeof(struct Stack_Info));
		stack->top->pointer = pointer;
		stack->top->prev = NULL;
		stack->top->next= NULL;
		stack->bottom = stack->top;
	} else {
		stack->bottom->next = (struct Stack_Info*) Alloc(sizeof(struct Stack_Info));
		stack->bottom->next->pointer = pointer;
		stack->bottom->next->prev = stack->bottom;
		stack->bottom->next->next = NULL;
		stack->bottom =  stack->bottom->next;

	}

	IsAdding=false;
	pthread_cond_signal(&condition);
	pthread_mutex_unlock(&mutex);

}

struct Stack_Info *Pop(struct Stack *stack)
{
	struct Stack_Info* topStack = NULL;

	while  (IsAdding)
	{
	}

	if (stack->top != NULL)
	{
		topStack = stack->top;
		if (topStack != NULL)
		{
			stack->top = topStack->next;
		}
	}

	return topStack;
}

void HandleMessages(void* args)
{
	char *tempMsg=NULL;
	char *command = NULL;
	struct Stack *stack=NULL;
	struct Options *options=NULL;

	options = (struct Options *) args;
	stack = (struct stack*) options->stack;


	while (running )
	{
		if (stack->top != NULL)
		{
			struct Stack_Info *popped = Pop(stack);
			bool32 found = false;
			struct array *findResults=NULL;

			if (popped)
			{
				char* message = (char*) popped->pointer;
				int msgLen = strlen(message);
				int startPoint = 0;
				int messageIndex=0;
				struct Tokenizer tokenizer={};
				bool32 isParsing = true;

				tokenizer.At = message;

				while(isParsing)
				{
					struct Token tken = {};

					tken = GetToken(&tokenizer);

					switch(tken.type)
					{
						case Null_Space:
						{
							isParsing=false;
							break;
						}
						case End_of_line:
						case New_Line:
						{
							if (command)
							{
								ParseMessage(command);

								Free(command);
								command=NULL;
								
							}
							break;
						}
						default:
						{
							char tempChar[2] = {};
							tempChar[0] = tken.tok;
							tempChar[1] = '\0';


							if (command == NULL)
							{
								command = CreateString(tempChar);
							} else {
								char *tempString=NULL;
								tempString = CatString(command,tempChar);

								Free(command);
								command=NULL;

								command = CreateString(tempString);

								Free(tempString);
								tempString=NULL;
							}
							break;
						}
					}
				}
			
				if (popped->pointer)
				{
					Free(popped->pointer);
					popped->pointer=NULL;
				}
				
				Free(popped);
				popped=NULL;
			}
			
		}
	}

	running = false;
	printf("* Message Handler thread stopped.\n");
	pthread_exit(0);
}

void ParseMessage(char *getMessage)
{
	struct Tokenizer tokenizer={};
	bool32 isParsing = true;

	tokenizer.At = getMessage;

	struct Token tken = {};
	int position=0;

	tken = GetToken(&tokenizer);

	switch(tken.type)
	{
		case Colon:
		{
			bool32 isColon=false;
			char *sentence=NULL;

			while(isColon == false)
			{
				tken = GetToken(&tokenizer);

				switch(tken.type)
				{
					case Null_Space:
					{
						ParseSender(sentence, NULL); 

						if (sentence)
						{
							Free(sentence);
						}


						isColon = true;
						break;
					}

					case Colon:
					{
						char* theRest = NULL;

						theRest = MidString(getMessage,position+2,strlen(getMessage));

						ParseSender(sentence, theRest); 

						Free(theRest);
						theRest=NULL;

						if (sentence)
						{
							Free(sentence);
						}


						isColon = true;
						break;
					}
					default:
					{
						position++;
						char tempChars[4] = {};

						tempChars[0] = tken.tok;

						if (sentence == NULL)
						{
							sentence= CreateString(tempChars);
						} else {
							char* tempSent = NULL;

							tempSent = CatString(sentence,tempChars);

							Free(sentence);
							sentence=NULL;

							sentence = CreateString(tempSent);

							Free(tempSent);
							tempSent=false;

						}
						break;
					}
				}
			}
			break;
		}
		default:
		{
			bool32 parsingOtherCommand=true;
			char *otherCommand=NULL;
			int ocCount=0;

			while(parsingOtherCommand)
			{

				switch(tken.type)
				{
					//case Colon:
					case Space:
					{
						if (strcmp(otherCommand, "PING") == 0)
						{
							char *pongMsg = NULL;
							char *pongMessage = NULL;
							char* tempPongMessage = NULL;

							pongMsg = MidString(getMessage, ocCount,strlen(getMessage));

							pongMessage = CreateString("PONG");
							tempPongMessage = CatString(pongMessage, pongMsg);

							Free(pongMessage);
							pongMessage = NULL;

							pongMessage = CatString(tempPongMessage, "\r\n");

							printf("* Ping Pong?!\n");

							Send(pongMessage, sslInfo.ssl);

							Free(pongMessage);
							pongMessage=NULL;

							Free(tempPongMessage);
							tempPongMessage=NULL;

							if (pongMsg)
							{
								Free(pongMsg);
								pongMsg = NULL;
							}
						} else {
							printf("* unknown command %s\n", otherCommand);
						}

						parsingOtherCommand = false;
						break;
					}
					default:
					{
						char tempOtherCommand[4] = {};

						tempOtherCommand[0] = tken.tok;
						ocCount++;
						if (otherCommand == NULL)
						{
							otherCommand = CreateString(tempOtherCommand);
						} else {
							char *tempOC = NULL;
							tempOC= CatString(otherCommand,tempOtherCommand);

							Free(otherCommand);
							otherCommand = NULL;

							otherCommand = CreateString(tempOC);

							Free(tempOC);
							tempOC = NULL;

							
						}
						break;
					}
				}

				tken = GetToken(&tokenizer);
			}
			/*Notes(): This might cause problems*/
			if (otherCommand)
			{
				Free(otherCommand);
				otherCommand=NULL;
			}
			
			break;
		}
	}
}


void ParseSender(char* sentence, char* theRest)
{
	struct Tokenizer tokenizer={};
	struct Token tken = {};
	bool32 parsingCommands = true;
	int position=0;
	char* sender= NULL;
	int senderCount=0;

	tokenizer.At = sentence;
	
	while (parsingCommands == true)
	{
		tken = GetToken(&tokenizer);

		switch(tken.type)
		{
			case Null_Space:
			case Space:
			{
				char *nextToParse = NULL;
				parsingCommands=false;	
			
				nextToParse = MidString(sentence, senderCount+1, strlen(sentence)-1);

				ParseCommand(sender,nextToParse,theRest);

				Free(sender);
				sender = NULL;

				Free(nextToParse);
				nextToParse = NULL;
				break;
			}
			default:
			{
				char* tempSender[5]={};

				senderCount++;

				tempSender[0] = tken.tok;

				if (sender == NULL)
				{
					sender = CreateString(tempSender);

				} else {
					char *senderCopy = CatString(sender,tempSender);

					Free(sender);
					sender = NULL;

					sender = CreateString(senderCopy);

					Free(senderCopy);
					senderCopy=NULL;

				}

				break;
			}
		}
	}
}


void ParseCommand(char* sender,char* nextToParse,char* theRest)
{
	struct Tokenizer tokenizer={};
	struct Token tken = {};
	bool32 parsingCommands = true;
	int position=0;
	char* ircCommand= NULL;
	int ircCommandCount=0;

	tokenizer.At = nextToParse;

	
	while (parsingCommands == true)
	{
		tken = GetToken(&tokenizer);

		switch(tken.type)
		{
			case Null_Space:
			case Space:
			{
				char *nextParse = NULL;
				parsingCommands=false;	
			
				nextParse = MidString(nextToParse, ircCommandCount+1, strlen(nextToParse));


				DoCommands(sender,ircCommand, nextParse, theRest);


				Free(ircCommand);
				ircCommand = NULL;

				Free(nextParse);
				nextParse = NULL;
				break;
			}
			default:
			{
				char* tempCommand[5]={};

				ircCommandCount++;

				tempCommand[0] = tken.tok;

				if (ircCommand == NULL)
				{
					ircCommand = CreateString(tempCommand);

				} else {
					char *ircCommandCopy = CatString(ircCommand,tempCommand);

					Free(ircCommand);
					ircCommand = NULL;

					ircCommand = CreateString(ircCommandCopy);

					Free(ircCommandCopy);
					ircCommandCopy=NULL;

				}

				break;
			}
		}
	}
}


void GetInfoFromFile(struct Irc_Info *ircInfo)
{
	FILE *configFile=NULL;
	int fileSize=0;

	configFile = fopen("config.txt","rw");

	if (configFile)
	{
		fseek(configFile,0,SEEK_END);
		fileSize = ftell(configFile);
		rewind(configFile);

		char fileInMemory[fileSize+1];
		int fileCount = 0;
		char *tempConfigString=NULL;

		fread(fileInMemory,1,fileSize,configFile);
		fileInMemory[fileSize]='\0';

		for (int configCounter=0;configCounter < fileSize;configCounter++)
		{
			switch(fileInMemory[configCounter])
			{
				case '\0':
				case '\n':
				{
					char *key = NULL;
					char *value = NULL;

					if (tempConfigString)
					{

						for (int i=0;i<strlen(tempConfigString);i++)
						{
							switch(tempConfigString[i])
							{
								case '=':
									{
										value = MidString(tempConfigString, i+1,strlen(tempConfigString));

										if (strcmp(key,"Owner") == 0)
										{
											ircInfo->owner= CreateString(value);
										}

										if (strcmp(key,"Nick") == 0)
										{
											ircInfo->nick = CreateString(value);
										}
										if (strcmp(key,"Realname") == 0)
										{
											ircInfo->name= CreateString(value);
										}
										if (strcmp(key,"Network") == 0)
										{
											ircInfo->server = CreateString(value);
										}
										if (strcmp(key,"Port") == 0)
										{
											ircInfo->port = atoi(value);
										}
										if (strcmp(key,"Pass") == 0)
										{
											ircInfo->pass= CreateString(value);
										}

										if (strcmp(key,"Rooms") == 0)
										{
											char *room=NULL;
											bool32 foundRooms = false;

											for (int roomIndex=0; roomIndex < strlen(value)+1;roomIndex++)
											{
												switch(value[roomIndex])
												{
													case '\0':
													case ',':
														{
															if (room)
															{
																char *newRoomString=NULL;
																foundRooms = true;

																newRoomString = CreateString(room);

																AddToArray(&ircInfo->rooms, newRoomString);

																Free(room);
																room = NULL;
															}
															break;
														}
													default:
														{
															char tempC[5] = {};

															tempC[0] = value[roomIndex];
															tempC[1] = '\0';

															if (room == NULL)
															{
																room = CreateString(tempC);
															} else {
																char *tempRoom = NULL;

																tempRoom = CatString(room, tempC);

																Free(room);
																room=NULL;

																room = CreateString(tempRoom);

																Free(tempRoom);
																tempRoom=NULL;

															}
															break;
														}
												}
											}

											if (foundRooms == false)
											{
												char *newRoomString=NULL;

												newRoomString = CreateString(value);
												AddToArray(&ircInfo->rooms, newRoomString);

											}
										}

									
										Free(value);
										value=NULL;
										break;
									}
								default:
									{

										char tempKey[5] = {};
										tempKey[0] = tempConfigString[i];
										tempKey[1] = '\0';

										if (key == NULL)
										{
											key = CreateString(tempKey);
										} else {
											char *tempK = NULL;

											tempK = CatString(key,tempKey);

											Free(key);
											key = NULL;

											key = CreateString(tempK);

											Free(tempK);
											tempK=NULL;
										}
										break;
									}
							}
						}
					}

					if (key)
					{
						Free(key);
						key=NULL;
					}

					Free(tempConfigString);
					tempConfigString = NULL;
					break;
				}
				default:
				{
					char smallTemp[3]={};
					smallTemp[0] = fileInMemory[configCounter];
					smallTemp[1] = '\0';

					if (tempConfigString == NULL)
					{
						tempConfigString = CreateString(smallTemp);
					} else {
						char *tempOld = NULL;
						tempOld = CatString(tempConfigString, smallTemp);

						Free(tempConfigString);
						tempConfigString=NULL;

						tempConfigString = CreateString(tempOld);

						Free(tempOld);
						tempOld=NULL;
					}
					break;
				}
			}
		}
		fclose(configFile);
	}

	if (ircInfo->server == NULL)
	{
		ircInfo->isLoaded = false;
	} else {
		ircInfo->isLoaded = true;
	}
}

void DoCommands(char* sender,char* command, char* reciever, char* message)
{
	if (strcmp(command, "376") == 0)
	{
		if (ircInfo.rooms.count > 0)
		{
			for (int arrayIndex=0;arrayIndex < ircInfo.rooms.count;arrayIndex++)
			{
				char* roomName = (char*) GetFromArray(&ircInfo.rooms,arrayIndex);
				
				if (roomName)
				{
					printf("* Joining %s\n", roomName);
					char *joinText = NULL;
					char *roomText = NULL;

					joinText = CreateString("JOIN ");
					roomText = CatString(joinText,roomName);

					Free(joinText);
					joinText=NULL;

					joinText = CatString(roomText,"\r\n");

					Free(roomText);
					roomText=NULL;


					Send(joinText,sslInfo.ssl);

					Free(joinText);
					joinText=NULL;
				}
			}
		}
	}

	for (int modIndex=0; modIndex < soList.count;modIndex++)
	{
		struct Module* modFromArray = GetFromArray(&soList,modIndex);
		if (modFromArray)
		{
			if (modFromArray->isLoaded)
			{
				if (modFromArray->On_PrivMsg && strcmp(command,"PRIVMSG") == 0)
				{
					modFromArray->On_PrivMsg(reciever,sender,message,&ircCommands);
				}

				/*NOTE(): User can not send NOTICE command on twitch*/
				if (modFromArray->On_Notice && strcmp(command, "NOTICE") == 0)
				{
					modFromArray->On_Notice(reciever,message,sender,&ircCommands);
				}

				if (modFromArray->On_Quit && strcmp(command, "QUIT") == 0)
				{
					modFromArray->On_Quit(sender,message,&ircCommands);
				}

				if (modFromArray->On_Nick && strcmp(command, "NICK") == 0)
				{
					modFromArray->On_Nick(sender,message,&ircCommands);
				}

				if (modFromArray->On_Join && strcmp(command, "JOIN") == 0)
				{
					modFromArray->On_Join(sender,reciever, &ircCommands);
				}

				if (modFromArray->On_Mode && strcmp(command, "MODE") == 0)
				{
					modFromArray->On_Mode(sender,reciever,message, &ircCommands);
				}

				if (modFromArray->On_Invite && strcmp(command,"INVITE") == 0)
				{
					modFromArray->On_Invite(sender,reciever,message, &ircCommands);
				}

				
			}
		}
	}
}

void LoadLibsFromDir(char *dirName, struct array * soList)
{
	DIR *dp = NULL;
	struct dirent *dirEnt=NULL;
	char *temp=NULL;

	if ((dp = opendir(dirName)) != NULL)
	{
		while ((dirEnt = readdir(dp)) != NULL)
		{
			if (dirEnt->d_name[0] != '.')
			{
				char *fullDirName=NULL;
				struct stat fileAtt={};

				temp = CreateString(dirName);
				fullDirName = CatString(temp,dirEnt->d_name);

				Free(temp);
				temp=NULL;

				if (stat(fullDirName, &fileAtt) == 0)
				{
					if (fileAtt.st_mode & S_IFREG)
					{
						struct array *findSoResults = NULL;

						findSoResults = FindString(".so", fullDirName);

						if (findSoResults)
						{
							if (findSoResults->count > 0)
							{
								struct Module* newMod = (struct Module*) Alloc(sizeof(struct Module));

								newMod->isLoaded = false;
								newMod->name = CreateString(fullDirName);

								AddToArray(soList,newMod);

								FreeArray(findSoResults);
								Free(findSoResults);
								findSoResults=NULL;
							}
						}
					}
				}

				if (fullDirName)
				{
					Free(fullDirName);
					fullDirName=NULL;
				}
			}
		}
	}
}

void SendMessage(char* message)
{
	Send(message,sslInfo.ssl);
}

void UpdateMods(void* arg)
{
	struct Update_Mod_Info *updateModInfo = NULL;
	struct stat currentDirTime = {};
	time_t startupTime = 0;


	updateModInfo = (struct Update_Mod_Info*) arg;

	while(running)
	{

		time(&startupTime);
		currentDirTime = NixGetLastWriteTime(updateModInfo->dirName);

		if (difftime(currentDirTime.st_mtime,updateModInfo->dirLastMod.st_mtime) > 0)
		{
			DIR *dp=NULL;
			struct dirent *dirEnt=NULL;

			printf("* changed in the mod folder\n");
			updateModInfo->dirLastMod = NixGetLastWriteTime(updateModInfo->dirName);

			if ((dp = opendir(updateModInfo->dirName)) != NULL)
			{
				while ((dirEnt = readdir(dp)) != NULL)
				{
					if (dirEnt->d_name[0] != '.')
					{
						char* dirName=NULL;
						struct stat fileAtt={};

						dirName = CatString(updateModInfo->dirName,dirEnt->d_name);

						if (stat(dirName,&fileAtt) == 0)
						{
							if (fileAtt.st_mode & S_IFREG)
							{
								struct array *arryOfSos=NULL;

								arryOfSos = FindString(".so", dirName);

								if (arryOfSos)
								{
									if (arryOfSos->count == 1)
									{
										bool isModFound=false;
										//NOTES(): Find if Mod is already loaded
										for (int modIndex=0;modIndex < updateModInfo->mods->count;modIndex++)
										{
											struct Module *oldMod = GetFromArray(updateModInfo->mods, modIndex);

											if (oldMod)
											{
												struct stat buffer={};
												if (strcmp(oldMod->name,dirName) == 0)
												{
													struct stat lastFT = NixGetLastWriteTime(oldMod->name);

													if(difftime(lastFT.st_mtime,oldMod->lastWriteTime.st_mtime) > 0)
													{
														printf("* Reloading %s \n", oldMod->name);
														dlclose(oldMod->handler);
														oldMod->handler = dlopen(oldMod->name,RTLD_LAZY);

														oldMod->lastWriteTime  = NixGetLastWriteTime(oldMod->name);

														if (oldMod->handler)
														{

															oldMod->isLoaded = true;
															LoadModCalls(oldMod);

														} else {
															printf("* Unable to reload Modules: %s\n", oldMod->name);
														}

													}
													isModFound = true;
													break;
												}
											}

										}

										if (isModFound == false)
										{
											struct Module *newMod = (struct Module*) Alloc(sizeof(struct Module));
											newMod->name = CreateString(dirName);
											printf("* Found Module..%s\n", dirName);

											newMod->handler = dlopen(newMod->name,RTLD_LAZY);
											newMod->lastWriteTime = NixGetLastWriteTime(newMod->name);

											if (newMod->handler)
											{
												newMod->isLoaded = true;
												
												printf("* Auto Loaded: %s\n", newMod->name);

												AddToArray(updateModInfo->mods,newMod);
												LoadModCalls(newMod);

											} else {
												printf("* Unable to Loaded: %s\n", newMod->name);
												newMod->isLoaded = false;
											}


										}
									}

									if (arryOfSos)
									{
										if (arryOfSos->count > 0)
										{
											FreeArray(arryOfSos);
										}
										Free(arryOfSos);
									
									}
								}

							}
						}

						if (dirName)
						{
							Free(dirName);
							dirName=NULL;
						}
					}
				}
			}
		}
	}

	running = false;
	printf("* Plugging Manger stopped.\n");
	pthread_exit(0);
}

struct stat NixGetLastWriteTime(char* file)
{
	struct stat fileAtti={};

	stat(file,&fileAtti);

	return fileAtti;
}

inline void LoadModCalls(struct Module *mod)
{
	mod->On_PrivMsg = (on_privmsg*) dlsym(mod->handler, "OnPrivMsg");

	if (mod->On_PrivMsg == NULL)
	{
		if (mod->On_PrivMsg== NULL)
		{
			mod->On_PrivMsg  = OnPrivMsgStub;
		}
	}

	mod->On_Join = (on_join*) dlsym(mod->handler, "OnJoin");

	if (mod->On_Join == NULL)
	{
		mod->On_Join = OnJoinStub;
	}

	/*
		NOTE(): 
		There is current no Error handling for errors such as not being able to connect to the target host or an OAUTH/Password error.
		In Order to do the Error we must get the server host name and go from there.
	*/

	mod->On_Notice = (on_notice *) dlsym(mod->handler, "OnNotice");

	if (mod->On_Notice == NULL)
	{
		mod->On_Notice = OnNoticeStub;
	}

	mod->On_Quit= (on_quit*) dlsym(mod->handler, "OnQuit");

	if (mod->On_Quit== NULL)
	{
		mod->On_Quit= OnQuitStub;
	}

	mod->On_Nick= (on_nick*) dlsym(mod->handler, "OnNick");

	if (mod->On_Nick== NULL)
	{
		mod->On_Nick= OnNickStub;
	}

	mod->On_Kick= (on_kick*) dlsym(mod->handler, "OnKick");

	if (mod->On_Kick== NULL)
	{
		mod->On_Kick= OnKickStub;
	}

	mod->On_Mode = (on_mode*) dlsym(mod->handler,"OnMode");
	
	if (mod->On_Mode == NULL)
	{
		mod->On_Mode= OnModeStub;
	}

	mod->On_Topic = (on_topic*) dlsym(mod->handler, "OnTopic");

	if (mod->On_Topic  == NULL)
	{
		mod->On_Topic  = OnModeStub;
	}

	mod->On_Invite = (on_invite*) dlsym(mod->handler,"OnInvite");

	if (mod->On_Invite == NULL)
	{
		mod->On_Invite = OnInviteStub;
	}

	mod->On_KeyPress= (on_keypress*) dlsym(mod->handler,"OnKeyPress");

	if (mod->On_KeyPress== NULL)
	{
		mod->On_KeyPress= OnKeyPressStub;
	} 
}

void ChangeMode(int dir)
{
	static struct termios oldT={};
	static struct termios newT={};

	if (dir == 1)
	{
		tcgetattr(STDIN_FILENO,&oldT);
		newT = oldT;
		newT.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO,TCSANOW,&newT);
	} else {
		tcsetattr(STDIN_FILENO,TCSANOW,&oldT);
	}

}

void gotoxy(int x,int y)
{
	printf("%c[%d;%df",0x1B,y,x);
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

int HandleKeys(bool32 *isRunning)
{
	/*
		NOTES(): For some reason this loop will not exit. Maybe it's because SSL_Read is blocking rather than non blocking.
		I am not sure how to fix this, but I should fix this soon.
	*/
	while (running)
	{

		char c='\0';
		char key=0;
		char buffer[256]={};
		int bytes  = 0;
		bytes  = SSL_read(sslInfo.ssl,buffer,254);

		if (bytes > 0)
		{
			char* chunk  = Alloc(bytes+1);
			memcpy(chunk,buffer,bytes);
			Push(&stack, chunk);
		}

	}

	printf("* Socket Handler Stopped.\n");
	pthread_exit(0);

	return 0;
	
}

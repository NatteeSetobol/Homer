#include <stdio.h>
#include "intrinsic.h"
#include "memory.h"
#include "string.h"
#include "plugins.h"
#include "ErrorHandler.h"
#include "http.h"
#include "JsonParser.h"
/*
gcc -Wall -fPIC -c example.c -I../../openssl-0.9.8zg/include -w -g
gcc -I../../openssl-0.9.8zg/include -shared -Wl,-install_name,myexampleso.1 -o myexample.so.1.0 ytsearch.o  -lssl -lcrypto -g
*/


enum Token_Type
{
	POLL_DATA,
	DASH,
	CREATE_POLL,
	POLL_ANSWERS,
	QUOTATION,
	END_OF_MSG,
	POLL_SPACE
};

struct Token
{
	size_t length;
	enum Token_Type type;
	char* text;
	char tok;
};

struct Tokenizer
{
	char* at;
};


struct Token GetToken(struct Tokenizer* tokenizer);
void SendToChat(char *message,char* recv,struct irc_commands *commands );

ON_PRIVMSG(OnPrivMsg)
{
	char *findPollTrigger = NULL;
	struct array *result = NULL;
	char* createdQuestion=NULL;
	struct array arrayOfAns={};
	char* nickname = NULL;
	struct array* atResults=NULL;
	bool authPass = false;

	result = FindString("!poll", msg);

	
	
	if (result )
	{

		if (result->count == 1)
		{

			int *pollPos = NULL;

			pollPos = GetFromArray(result, 0);

			if (*pollPos == 0)
			{
				atResults = FindString("!",user);

				if (atResults)
				{
					int* getPos = GetFromArray(atResults,0);
					if (getPos)
					{
						nickname = MidString(user,0,*getPos);
					}
				}
				if (nickname)
				{

					if (commands->ownerName)
					{
						if (strcmp(nickname,commands->ownerName) == 0)
						{
							authPass = true;
						} else {
							authPass = false;
						}
					} else {
						authPass = true;
					}

					if (authPass)
					{
						char *options = NULL;

						options = MidString(msg,6, strlen(msg));

						if (options)
						{
							struct Tokenizer tokenizer={};
							struct Token pollToken	= {};
							bool optionEnded = false;
							bool foundQuo=false;

							tokenizer.at = options;


							while ( optionEnded == false)
							{
								pollToken = GetToken(&tokenizer);

								switch(pollToken.type)
								{
									case END_OF_MSG:
										{
											optionEnded = true;
											break;
										}
									case DASH:
										{
											pollToken = GetToken(&tokenizer);

											switch (pollToken.type)
											{
												case POLL_ANSWERS:
													{
														bool doneAnswer=false;

														pollToken = GetToken(&tokenizer);


														switch(pollToken.type)
														{
															case POLL_SPACE:
																{
																	char* answers = NULL;

																	for(;doneAnswer==false;)
																	{
																		pollToken = GetToken(&tokenizer);
																		switch(pollToken.type)
																		{

																			case END_OF_MSG:
																				{
																					doneAnswer = true;
																					optionEnded = true;

																					break;
																				}
																			case QUOTATION:
																				{
																					if (foundQuo == true)
																					{
																						AddToArray(&arrayOfAns, answers);
																						answers = NULL;
																						foundQuo=false;
																					} else {
																						foundQuo = true;
																					}
																					break;
																				}
																			case POLL_SPACE:
																				{
																					if (foundQuo == false)
																					{
																						break;
																					}
																				}
																			default:
																				{
																					char letters[4] = {};

																					letters[0] = pollToken.tok;

																					if (answers)
																					{
																						char * tempAns = NULL;

																						tempAns = CatString(answers, letters);

																						Free(answers);
																						answers = NULL;

																						answers = CreateString(tempAns);

																						Free(tempAns);
																						tempAns=NULL;
																					} else {

																						answers = CreateString(letters);
																					}


																					break;
																				}
																		}
																	}

																	break;
																}

														}

														break;
													}
												case CREATE_POLL:
													{
														pollToken = GetToken(&tokenizer);

														switch(pollToken.type)
														{
															case POLL_SPACE:
																{

																	pollToken = GetToken(&tokenizer);

																	switch(pollToken.type)
																	{
																		case QUOTATION:
																			{
																				bool foundQuote=false;
																				for (; foundQuote == false ;)
																				{
																					pollToken = GetToken(&tokenizer);

																					switch(pollToken.type)
																					{
																						case END_OF_MSG: 
																							{
																								optionEnded = true;
																								foundQuote = true;
																								break;
																							}
																						case QUOTATION:
																							{
																								foundQuote = true;
																								break;
																							}
																						default:
																							{
																								char tempLetter[4]={};
																								tempLetter[0] = pollToken.tok;

																								if (createdQuestion)
																								{
																									char *tempQuestion  = NULL;

																									tempQuestion = CatString(createdQuestion, tempLetter);

																									Free(createdQuestion);
																									createdQuestion=NULL;

																									createdQuestion = CreateString(tempQuestion);

																									Free(tempQuestion);
																									tempQuestion=NULL;

																								} else {
																									createdQuestion = CreateString(tempLetter);
																								}


																							}
																					}


																				}
																				break;
																			}
																	}
																	break;
																}
															default:
																{
																	break;
																}
														}
														break;
													}
												default:
													{

														break;
													}
											}
											break;
										}
								}
							}


							Free(options);
							options = NULL;
						}

						if (createdQuestion)
						{
							if (arrayOfAns.count > 0)
							{
								struct dictionary httpAccept={};
								struct dictionary httpHost={};
								struct dictionary httpUserAgent={};
								struct dictionary httpContentType = {};
								struct dictionary httpConnection= {};
								struct array headers={};
								char *httpResults = NULL;
								struct  Error_Handler *httpError=NULL;
								char* combinedOptions = NULL;
								char* temp=NULL;
								char* jsonPost=NULL;
								struct http_packet packets={};
								struct Json_Branch branch={};
								struct  Error_Handler *jsonErrorHandler=NULL;

								for (int answerIndex=0;answerIndex < arrayOfAns.count; answerIndex++)
								{
									char* answr = (char*) GetFromArray(&arrayOfAns, answerIndex);

									if (answr)
									{
										if (combinedOptions == NULL)
										{
											temp = CreateString("\"");

											combinedOptions = CatString(temp,answr);

											Free(temp);
											temp=NULL;

											temp = CatString(combinedOptions, "\"");

											Free(combinedOptions);
											combinedOptions=NULL;

											combinedOptions = CreateString(temp);

											Free(temp);
											temp=NULL;


										} else {
											temp = CatString(combinedOptions, ",\"");

											Free(combinedOptions);
											combinedOptions = NULL;

											combinedOptions = CatString(temp, answr);

											Free(temp);
											temp=NULL;

											temp = CatString(combinedOptions, "\"");

											Free(combinedOptions);
											combinedOptions=NULL;

											combinedOptions = CreateString(temp);

											Free(temp);
											temp=NULL;

										}
									}
								}

								if (temp)
								{
									Free(temp);
									temp=NULL;
								}

								if (jsonPost == NULL)
								{
									jsonPost = CreateString("{\"title\":\"");

									temp = CatString(jsonPost, createdQuestion);

									Free(jsonPost);
									jsonPost=NULL;

									jsonPost = CatString(temp,"\",\"options\":[");

									Free(temp);
									temp=NULL;

									temp = CatString(jsonPost, combinedOptions);

									Free(jsonPost);
									jsonPost=NULL;

									jsonPost = CatString(temp,"],\"multi\":false}");

									Free(temp);
									temp=NULL;

								}

								httpAccept.key = CreateString("Accept");
								httpAccept.value = CreateString("html/text");

								httpHost.key = CreateString("Host");
								httpHost.value = CreateString("www.strawpoll.me");

								httpUserAgent.key = CreateString("User-Agent");
								httpUserAgent.value =  CreateString("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.12; rv:51.0) Gecko/20100101 Firefox/51.0");

								httpContentType.key = CreateString("Content-Type");
								httpContentType.value = CreateString("application/json; charset=utf-8");

								httpContentType.key = CreateString("Connection");
								httpContentType.value = CreateString("close");

								AddToArray(&headers, &httpAccept);
								AddToArray(&headers, &httpHost);
								AddToArray(&headers, &httpUserAgent);
								AddToArray(&headers, &httpContentType);
								AddToArray(&headers, &httpConnection);

								httpError = Error_Handler();

								httpResults = Http_Send("http://www.strawpoll.me/api/v2/polls",jsonPost,&headers, POST, httpError);

								if (httpResults)
								{
									char *pollId = NULL;
									struct array stuff={};
									struct Json_Branch *jsonNode=NULL;

									ParseHTTPPacket(httpResults,&packets);
									jsonErrorHandler = Error_Handler();
									JSON_Parse(packets.data,&branch,jsonErrorHandler); 

									AddToArray(&stuff,"id");

									jsonNode = Json_GetJsonNodeFromKey(&branch,&stuff);

									if (jsonNode)
									{
										char chatMessage[256]={};

										sprintf(chatMessage,"\"%s\" Vote Now! http://strawpoll.me/%s",createdQuestion, jsonNode->value);
										SendToChat(chatMessage,from,commands);

									}

									if (jsonErrorHandler)
									{
										Free(jsonErrorHandler);
										jsonErrorHandler=NULL;
									}

									FreeArrayStack(&stuff);
									Json_Free(&branch);

									if (packets.data)
									{
										Free(packets.data);
										packets.data = NULL;
									}

								}

								if (httpAccept.key)
								{
									Free(httpAccept.key);
									httpAccept.key=NULL;
								}

								if (httpAccept.value)
								{
									Free(httpAccept.value);
									httpAccept.value = NULL;
								}


								if (httpHost.key)
								{
									Free(httpHost.key);
									httpHost.key=NULL;
								}

								if (httpHost.value)
								{
									Free(httpHost.value); 
									httpHost.value = NULL;
								}

								if (httpUserAgent.key)
								{
									Free(httpUserAgent.key);
									httpUserAgent.key=NULL;
								}

								if (httpUserAgent.value)
								{
									Free(httpUserAgent.value);
									httpUserAgent.value=NULL;
								}

								if (httpContentType.key)
								{
									Free(httpContentType.key);
									httpContentType.key=NULL;
								}

								if (httpContentType.value)
								{
									Free(httpContentType.value);
									httpContentType.value=NULL;
								}

								if (httpConnection.key)
								{
									Free(httpConnection.key);
									httpConnection.key=NULL;
								}

								if (httpContentType.value)
								{
									Free(httpContentType.value);
									httpContentType.value=NULL;
								}

								FreeArrayStack(&headers);

								if (httpError)
								{
									if (httpError->error)
									{
										Free(httpError->error);
										httpError->error=NULL;
									}

									Free(httpError);
									httpError=NULL;
								}
							}
						}

						Free(pollPos);
						pollPos=NULL;
					}
				}
			}

			if (nickname)
			{
				Free(nickname);
				nickname=NULL;
			}

			if (atResults)
			{
				FreeArrayStack(atResults);
				atResults=NULL;
			}


			FreeArrayStack(result);
			result=NULL;
		}
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

struct Token GetToken(struct Tokenizer* tokenizer)
{
	struct Token tokens = {};

	char c=0;
	ui8* tokAddr=NULL;

	tokens.length = 1;
	tokens.text = tokenizer->at;
	c = tokenizer->at[0];
	tokens.tok = c;
	++tokenizer->at;

	switch(c)
	{
		case '-' : { tokens.type = DASH; } break;
		case 'c' : { tokens.type = CREATE_POLL; } break;
		case ' ' : { tokens.type = POLL_SPACE; } break;
		case '\"' : { tokens.type = QUOTATION; } break;
		case 'a' : { tokens.type = POLL_ANSWERS; } break;
		case '\0' : { tokens.type = END_OF_MSG; } break;
		default: { tokens.type = POLL_DATA; } break;
	}

	return tokens;
}


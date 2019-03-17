#include "ssl.h"
#include "array.h"
#include "plugins.h"
#include "JsonParser.h"


void SendToChat(char *message,char* recv,struct irc_commands *commands );

ON_PRIVMSG(OnPrivMsg)
{
	struct array *quotation = NULL;
	char *replacedText=NULL;
	char *bingToken=NULL;
	struct sslinfo sslInfo={};
	struct sslinfo sslPostInfo={};
	struct http_packet packets={};
	char *finalCookie=NULL;

	quotation = FindString("\"",msg);

	if (quotation)
	{
		if (quotation->count > 0)
		{

			replacedText = StrReplace(msg,"\"","\\\"");
			
			//Free
			for (int i=0;i<quotation->count;i++)
			{
				int *pointer = GetFromArray(quotation,i);

				if (pointer)
				{
					Free(pointer);
					pointer=NULL;
				}
			}

			FreeArrayStack(quotation);
			
			Free(quotation);
			quotation=NULL;

			if (replacedText)
			{
				Free(replacedText);
				replacedText=NULL;
			}
		}

		bingToken = SSLGet(&sslInfo,"http://www.bing.com/translator",NULL);

		if (bingToken)
		{
			ParseHTTPPacket(bingToken,&packets);
			
			for (int packetHeaderIndex=0; packetHeaderIndex < packets.headers.count; packetHeaderIndex++)
			{
				char* wholeCookie=NULL;

				wholeCookie = GetFromArray(&packets.headers,packetHeaderIndex);

				if (wholeCookie)
				{
					struct array *colonResults = NULL;

					colonResults = FindString(":",wholeCookie);

					if (colonResults)
					{
						if (colonResults->count > 0)
						{

							for (int colonIndex=0;colonIndex < colonResults->count;colonIndex++)
							{
								char *cookieKey=NULL;
								char *cookieValue=NULL;
								int *colonPos=NULL;
								int newColonPos=0;

								colonPos = GetFromArray(colonResults,colonIndex);

								if (colonPos)
								{
									cookieKey = MidString(wholeCookie,0,*colonPos);
									cookieValue = MidString(wholeCookie,(*colonPos)+2,strlen(wholeCookie));

									if (strcmp(cookieKey,"Set-Cookie") == 0)
									{
										struct array *semiColonResults = FindString(";",cookieValue);

										if (semiColonResults)
										{
											for (int semiColonIndex=0;semiColonIndex < semiColonResults->count;semiColonIndex++)
											{
												int *colonPos = NULL;
												char* cookiePieces = NULL;

												colonPos = (int*) GetFromArray(semiColonResults,semiColonIndex);

												if (colonPos)
												{
													if (*colonPos > 0 && *colonPos < strlen(cookieValue))
													{
														int* equalPos = 0;
														struct array *equalResults=NULL;

														cookiePieces = MidString(cookieValue,newColonPos,*colonPos);

														if (cookiePieces)
														{
															equalResults = FindString("=", cookiePieces);

															if (equalResults)
															{
																if (equalResults->count == 1)
																{
																	char *cookieKey=NULL;
																	char *cookieValue=NULL;

																	equalPos = GetFromArray(equalResults,0);

																	if (equalPos)
																	{
																		cookieKey = MidString(cookiePieces,0, *equalPos);
																		cookieValue = MidString(cookiePieces,(*equalPos)+1, strlen(cookiePieces));

																		if (cookieKey && cookieValue)  
																		{
																			if (strcmp(cookieKey,"mtstkn") == 0)
																			{
																				if (finalCookie == NULL)
																				{
																					finalCookie = CatString(cookiePieces,";");
																				} else {
																					char *tempCookie = CatString(finalCookie,cookiePieces);
																					finalCookie = CatString(tempCookie,";");

																					if (tempCookie)
																					{
																						Free(tempCookie);
																						tempCookie=NULL;
																					}

																				}
																			}

																			if (strcmp(cookieKey,"MUID") == 0)
																			{
																				if (finalCookie == NULL)
																				{
																					finalCookie = CatString(cookiePieces,";");
																				} else {
																					char *tempCookie = CatString(finalCookie,cookiePieces);
																					finalCookie = CatString(tempCookie,";");

																					if (tempCookie)
																					{
																						Free(tempCookie);
																						tempCookie=NULL;
																					}

																				}

																			}

																			if (cookieKey)
																			{
																				Free(cookieKey);
																				cookieKey = NULL;

																				Free(cookieValue);
																				cookieValue=NULL;
																			}
																		}

																	}
																}

																for (int equalResultsIndex=0;equalResultsIndex < equalResults->count;equalResultsIndex++)
																{
																	int* ePos = GetFromArray(equalResults,equalResultsIndex);

																	if (ePos)
																	{
																		Free(ePos);
																		ePos=NULL;
																	}
																}

																FreeArrayStack(equalResults);
																Free(equalResults);
																equalResults=NULL;
															}

														}


														if (cookiePieces)
														{
															Free(cookiePieces);
															cookiePieces = NULL;
														}

														newColonPos= *colonPos;
													}

													Free(colonPos);
													colonPos=NULL;
												}

											}

										}

										FreeArrayStack(semiColonResults);

										Free(semiColonResults);
										semiColonResults=NULL;
									}

									if (cookieKey)
									{
										Free(cookieKey);
										cookieKey=NULL;
									}

									if (cookieValue)
									{
										Free(cookieValue);
										cookieValue=NULL;
									}

								}
							}

							//NOTES(): Free colonResults
							for (int colonResultsIndex=0;colonResultsIndex < colonResults->count;colonResultsIndex++)
							{
								int *pointerToColon = GetFromArray(colonResults,colonResultsIndex);

								if (pointerToColon)
								{
									Free(pointerToColon);
									pointerToColon=NULL;
								}
							}

							FreeArrayStack(colonResults);
							Free(colonResults);
							colonResults=NULL;
						}
					}
				}
			}
		}



		if (packets.headers.count > 0)
		{
			for (int headerIndex=0;headerIndex < packets.headers.count; headerIndex++)
			{
				char *header = GetFromArray(&packets.headers, headerIndex);

				if (header)
				{
					Free(header);
					header=NULL;
				}
			}
		}

		FreeArrayStack(&packets.headers);

		if (packets.data)
		{
			Free(packets.data);
			packets.data=NULL;
		}

		if (bingToken)
		{
			Free(bingToken);
			bingToken=NULL;
		}

		Free(quotation);
		quotation=NULL;
	}

	#if 1

	struct dictionary hostParam={};
	struct dictionary userAgentParam={};
	struct dictionary acceptParam={};
	struct dictionary acceptLang={};
	struct dictionary contentTypeParam={};
	struct dictionary cookieParam={};
	struct dictionary refererParam={};
	struct dictionary connectParam={};
	char* payload=NULL;
	char* tempPayload=NULL;
	struct array transParam={};
	struct Error_Handle *postError=NULL;
	char *transResults=NULL;
	struct Json_Branch jsonBranch={};
	struct Error_Handler *jsonError=NULL;
	struct Json_Branch *translatedText=NULL;
	struct array translatedTextArray={};
	struct Json_Branch *toLang=NULL;

	hostParam.key = CreateString("Host");
	hostParam.value =CreateString("www.bing.com");

	userAgentParam.key = CreateString("User-Agent");
	userAgentParam.value = CreateString("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:44.0) Gecko/20100101 Firefox/44.0");

	acceptParam.key=CreateString("Accept");
	acceptParam.value = CreateString("application/json, text/javascript, */*; q=0.01");

	acceptLang.key=CreateString("Accept-Language");
	acceptLang.value = CreateString("en-US;q=0.7,en;q=0.3");

	contentTypeParam .key = CreateString("Content-Type");
	contentTypeParam.value = CreateString("application/json; charset=utf-8");

	cookieParam.key = CreateString("Cookie");
	cookieParam.value = CreateString(finalCookie); 

	refererParam.key = CreateString("Referer");
	refererParam.value = CreateString("http://www.bing.com/translator");

	connectParam.key = CreateString("Connection");
	connectParam.value = CreateString("close");

	payload = CreateString("[{\"id\":1060490900,\"text\":\"");
	tempPayload = CatString(payload,msg);

	Free(payload);
	payload=NULL;

	payload = CatString(tempPayload,"\"}]");

	Free(tempPayload);
	tempPayload=NULL;

	AddToArray(&transParam,&hostParam);
	AddToArray(&transParam,&userAgentParam);
	AddToArray(&transParam,&acceptParam);
	AddToArray(&transParam,&acceptLang);
	AddToArray(&transParam,&contentTypeParam);
	AddToArray(&transParam,&cookieParam);
	AddToArray(&transParam,&refererParam);
	AddToArray(&transParam,&connectParam);

	postError =  Error_Handler();
	transResults=SSLPostH(&sslPostInfo,"https://www.bing.com/translator/api/Translate/TranslateArray?from=-&to=en",payload,&transParam,postError);

	ParseHTTPPacket(transResults,&packets);

	jsonError= Error_Handler();
	JSON_Parse(packets.data,&jsonBranch, jsonError);

	if (jsonError)
	{
		if (jsonError->error)
		{
			printf("Error parsing Json\n");
		} else {
			printf("no errors\n");
		}
	} else {
	}
	AddToArray(&translatedTextArray,"from");

	toLang = Json_GetJsonNodeFromKey(&jsonBranch,&translatedTextArray);

	if (toLang)
	{

		if (strcmp(toLang->value,"\"en\"") != 0)
		{
			FreeArrayStack(&translatedTextArray);
			AddToArray(&translatedTextArray,"items");
			AddToArray(&translatedTextArray,"text");

			translatedText = Json_GetJsonNodeFromKey(&jsonBranch,&translatedTextArray);

			if (translatedText)
			{

				int charMatches=0;
				int  matchPercentage=0;

				for (int i = 1; i < strlen(translatedText->value) - 1;i++)
				{
					if (tolower(translatedText->value[i]) == tolower(msg[i-1]))
					{
						charMatches++;
					}
				}

			
				matchPercentage =   strlen(msg) - charMatches;

				printf("character matched: %i %i = %i\n", strlen(msg), charMatches, matchPercentage);
				if (matchPercentage > 5)
				{

					char translatedTextToSend[256]={};

					if (strlen(translatedText->value) < 128)
					{
						sprintf(translatedTextToSend,"Translation %s\n", translatedText->value);
						SendToChat(translatedTextToSend,from,commands);
					}
				}

				FreeArrayStack(&translatedTextArray);

			}
		}
	}

	Json_Free(&jsonBranch);

	for (int packetIndex=0;packetIndex < packets.headers.count;packetIndex++)
	{
		char* packet = (char*) GetFromArray(&packets.headers, packetIndex);

		if (packet)
		{
			Free(packet);
			packet=NULL;
		}
	}

	FreeArrayStack(&packets.headers);

	if (payload)
	{
		Free(payload);
		payload=NULL;
	}
	if (packets.data)
	{
		Free(packets.data);
		packets.data=NULL;
	}

	if (hostParam.key)
	{
		Free(hostParam.key); 
		hostParam.key=NULL;
	}

	if (hostParam.value)
	{
		Free(hostParam.value);
		hostParam.value=NULL;
	}

	if (userAgentParam.key)
	{
		Free(userAgentParam.key); 
			userAgentParam.key=NULL;
		}

		
		if (userAgentParam.value )
		{
			Free(userAgentParam.value);
			userAgentParam.value=NULL;
		}

		if (acceptParam.key)
		{
			Free(acceptParam.key);
			acceptParam.key=NULL;
		}

		if (acceptParam.value)
		{
			Free(acceptParam.value); 
			acceptParam.value=NULL;
		}

		if (acceptLang.key)
		{
			Free(acceptLang.key);
			acceptLang.key=NULL;
		}


		if (acceptLang.value) 
		{
			Free(acceptLang.value);
			acceptLang.value=NULL;
		}

		if (contentTypeParam .key)
		{
			Free(contentTypeParam .key);
			contentTypeParam .key=NULL;
		}

		if (contentTypeParam.value)
		{
			Free(contentTypeParam.value);
			contentTypeParam.value=NULL;
		}

		if (cookieParam.key)
		{
			Free(cookieParam.key);
			cookieParam.key=NULL;
		}

		if (cookieParam.value)
		{
			Free(cookieParam.value);
			cookieParam.value=NULL;
		}

		if (refererParam.key)
		{
			Free(refererParam.key);
			refererParam.key=NULL;
		}
	
		if (refererParam.value)
		{
			Free(refererParam.value);
			refererParam.value=NULL;
		}

		if (connectParam.key)
		{
			Free(connectParam.key);
			connectParam.key=NULL;
		}

		if (connectParam.value)
		{
			Free(connectParam.value);
			connectParam.value=NULL;
		}

		FreeArrayStack(&transParam);
		
	

	#endif
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
	

#ifndef _BING_H
#define _BING_H

#include "ssl.h"
#include "array.h"
#include "JsonParser.h"

enum voices 
{
	MALE,
	FEMALE
};

enum mediaType
{
	WAV,
	MP3
};


//NOTE(): Default media and voice is WAV and MALE
struct BingVoiceInfo
{
	enum voices voice;
	char* outputFile;
	enum mediaType mediatype;
	char* lang;
};

char* GetBingCookie();

inline char* GetBingCookie()
{
	struct array *quotation = NULL;
	char *replacedText=NULL;
	char *bingToken=NULL;
	struct sslinfo sslInfo={0};
	struct sslinfo sslPostInfo={0};
	struct http_packet packets={0};
	char *finalCookie=NULL;
	char* results = NULL;
	struct array cookieHeaders ={};

	struct dictionary acceptParam = {};

	acceptParam.key = CreateString("Accept");
	acceptParam.value = CreateString("html/text");
	AddToArray(&cookieHeaders,(int*) &acceptParam);

	struct dictionary acceptHost = {};
	acceptHost.key = CreateString("Host");
	acceptHost.value = CreateString("www.bing.com");
	AddToArray(&cookieHeaders,(int*) &acceptHost);

	struct dictionary acceptCE = {};
	acceptCE.key = CreateString("Accept-Encoding");
	acceptCE.value = CreateString("gzip,deflate");
	AddToArray(&cookieHeaders, &acceptCE);

	struct dictionary acceptConnect = {};
	acceptConnect.key = CreateString("Connection");
	acceptConnect.value = CreateString("close");
	AddToArray(&cookieHeaders,(int*) &acceptConnect);

	bingToken =  SSLGetHRaw(&sslInfo, "http://www.bing.com/translator", &cookieHeaders,NULL);


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

	if (acceptHost.key)
	{
		Free(acceptHost.key);	
		acceptHost.key=NULL;
	}

	if (acceptHost.value)
	{
		Free(acceptHost.value);	
		acceptHost.value=NULL;
	}

	if (acceptConnect.key)
	{
		Free(acceptConnect.key);	
		acceptConnect.key=NULL;
	}

	if (acceptConnect.value)
	{
		Free(acceptConnect.value);	
		acceptConnect.value=NULL;
	}

	if (acceptCE.key)
	{
		Free(acceptCE.key);	
		acceptCE.key=NULL;
	}

	if (acceptCE.value)
	{
		Free(acceptCE.value);	
		acceptCE.value=NULL;
	}

	FreeArrayStack(&cookieHeaders);

	
	//bingToken = SSLGet(&sslInfo,"http://www.bing.com/translator",NULL);

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

																				if (finalCookie)
																				{
																					Free(finalCookie);
																				}
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
																				if (finalCookie)
																				{
																					Free(finalCookie);
																				}
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
					}
					Free(colonResults);
					colonResults=NULL;
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

	if (quotation)
	{
		Free(quotation);
		quotation=NULL;
	}

	return finalCookie;
}

char* Translation(char* textToTranslate)
{
	struct sslinfo sslPostInfo={0};
	struct http_packet packets={0};
	char* results = NULL;
	char *finalCookie = NULL;
	struct dictionary hostParam={0};
	struct dictionary userAgentParam={0};
	struct dictionary acceptParam={0};
	struct dictionary acceptLang={0};
	struct dictionary contentTypeParam={0};
	struct dictionary cookieParam={0};
	struct dictionary refererParam={0};
	struct dictionary connectParam={0};
	char* payload=NULL;
	char* tempPayload=NULL;
	struct array transParam={0};
	struct Error_Handle *postError=NULL;
	char *transResults=NULL;
	struct Json_Branch jsonBranch={0};
	struct Error_Handler *jsonError=NULL;
	struct Json_Branch *translatedText=NULL;
	struct array translatedTextArray={0};
	struct Json_Branch *toLang=NULL;

	finalCookie = GetBingCookie();

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
	tempPayload = CatString(payload,textToTranslate);

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

	//printf("%s\n", packets.data);
	ParseHTTPPacket(transResults,&packets);

	jsonError= Error_Handler();
	JSON_Parse(packets.data,&jsonBranch, jsonError);


	if (jsonError)
	{
		if (jsonError->error)
		{
			printf("Error parsing Json\n");
		} else {
			//printf("no errors\n");
		}
	} else {
	}
	AddToArray(&translatedTextArray,"from");

	toLang = Json_GetJsonNodeFromKey(&jsonBranch,&translatedTextArray);

	if (toLang)
	{

		//if (strcmp(toLang->value,"\"en\"") != 0)
		{
			FreeArrayStack(&translatedTextArray);
			AddToArray(&translatedTextArray,"items");
			AddToArray(&translatedTextArray,"text");

			translatedText = Json_GetJsonNodeFromKey(&jsonBranch,&translatedTextArray);

			if (translatedText)
			{
				results = CreateString(translatedText->value);
				/*
				int charMatches=0;
				int  matchPercentage=0;

				for (int i = 1; i < strlen(translatedText->value) - 1;i++)
				{
					if (tolower(translatedText->value[i]) == tolower(textToTranslate[i-1]))
					{
						charMatches++;
					}
				}


				matchPercentage =   strlen(textToTranslate) - charMatches;

				printf("character matched: %i %i = %i\n", strlen(textToTranslate), charMatches, matchPercentage);
				if (matchPercentage > 5)
				{

					char translatedTextToSend[256]={0};

					if (strlen(translatedText->value) < 128)
					{
						sprintf(translatedTextToSend,"Translation %s\n", translatedText->value);
					}
				}
				
				*/
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

	if (postError)
	{
		Free(postError);
	}
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

	if (transResults)
	{
		Free(transResults);
	}

	if (finalCookie)
	{
		Free(finalCookie);
	}

	if (jsonError)
	{
		if (jsonError->error)
		{
			Free(jsonError->error);
		}

		Free(jsonError);
	}
	return results;
}


void Speak(char* textToTranslate, struct BingVoiceInfo* bingVoiceInfo)
{
	struct sslinfo sslPostInfo={0};
	struct http_packet packets={0};
	char *finalCookie = NULL;
	struct dictionary hostParam={0};
	struct dictionary userAgentParam={0};
	struct dictionary acceptParam={0};
	struct dictionary acceptLang={0};
	struct dictionary contentTypeParam={0};
	struct dictionary cookieParam={0};
	struct dictionary refererParam={0};
	struct dictionary connectParam={0};
	char* payload=NULL;
	char* tempPayload=NULL;
	struct array transParam={0};
	struct Error_Handle *postError=NULL;
	char *transResults=NULL;
	struct Json_Branch jsonBranch={0};
	struct Error_Handler *jsonError=NULL;
	struct Json_Branch *translatedText=NULL;
	struct array translatedTextArray={0};
	struct Json_Branch *toLang=NULL;
	char *text = NULL;
	char* spaceReplace=NULL;
	char *translationURL = NULL;
	char* tempTransURL=NULL;

	if (bingVoiceInfo)
	{
		if (bingVoiceInfo->lang == NULL)
		{
			bingVoiceInfo->lang = CreateString("en-US");
		}
	}

	#if 1
	tempTransURL = CreateString("http://www.bing.com/translator/api/language/Speak?locale=");

	if (bingVoiceInfo)
	{
		translationURL = CatString(tempTransURL,bingVoiceInfo->lang);
	} else {
		translationURL = CatString(tempTransURL,"en-US");
	}
	
	if (tempTransURL)
	{
		Free(tempTransURL);
		tempTransURL=NULL;
	}
	

	tempTransURL = CatString(translationURL,"&gender=");
	
	if (translationURL )
	{
		Free(translationURL );
	}

	if (bingVoiceInfo)
	{
		if (bingVoiceInfo->voice == MALE)
		{
			translationURL = CatString(tempTransURL,"male");
		} else 
		if (bingVoiceInfo->voice == FEMALE)
		{
			translationURL = CatString(tempTransURL,"female");
		}
	} else {
		translationURL = CatString(tempTransURL,"male");
	}


	if (tempTransURL)
	{
		Free(tempTransURL);
		tempTransURL=NULL;
	}
	

	if (bingVoiceInfo)
	{
		switch (bingVoiceInfo->mediatype)
		{
			case WAV:
			{
				tempTransURL = CatString(translationURL, "&media=audio/wav");
				break;
			}
			case MP3:
			{
				tempTransURL = CatString(translationURL, "&media=audio/mp3");
				break;
			}
		}
	} else {
		tempTransURL = CatString(translationURL, "&media=audio/wav");
	}

	
	if (translationURL)
	{
		Free(translationURL);
		translationURL = NULL;
	}


	translationURL = CatString(tempTransURL,"&text=");

	if (tempTransURL)
	{
		Free(tempTransURL);
		tempTransURL=NULL;
	}

	#endif
	

	//printf("%s\n", translationURL);

	//translationURL = CreateString("http://www.bing.com/translator/api/language/Speak?locale=en-US&gender=male&media=audio/mp3&text=");

	finalCookie = GetBingCookie();

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

	if (finalCookie)
	{
		cookieParam.value = CreateString(finalCookie); 
	} else {
		cookieParam.value = CreateString("no code");
	}

	refererParam.key = CreateString("Referer");
	refererParam.value = CreateString("http://www.bing.com/translator");

	connectParam.key = CreateString("Connection");
	connectParam.value = CreateString("close");

	payload = CreateString("[{\"id\":1060490900,\"text\":\"");
	tempPayload = CatString(payload,textToTranslate);

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

	spaceReplace =  StrReplace(textToTranslate," ","+");

	text = CatString(translationURL, spaceReplace);
	
	transResults= SSLGetHRaw(&sslPostInfo,text,&transParam, postError);

	ParseHTTPPacketRaw(transResults,&packets, &sslPostInfo);

	FILE* testWav = fopen("test.wav", "w");

	for (int i=0;i<packets.dataLen;i++)
	{
		fputc(packets.data[i], testWav);
	}

	fclose(testWav);


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

	if (postError)
	{
		Free(postError);
	}

	if (bingVoiceInfo->lang)
	{
		Free(bingVoiceInfo->lang);
	}

	if (sslPostInfo.realURL)
	{
		Free(sslPostInfo.realURL);
	}
	if (translationURL)
	{
		Free(translationURL);
		translationURL=NULL;
	}

	/*
	if (spaceReplace)
	{
		Free(spaceReplace);
		spaceReplace=NULL;
	}
	*/

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

	if (text)
	{
		Free(text);
		text=NULL;
	}

	if (finalCookie)
	{
		Free(finalCookie);
	}

	if (transResults)
	{
		Free(transResults);
	}
	FreeArrayStack(&transParam);

}

#endif

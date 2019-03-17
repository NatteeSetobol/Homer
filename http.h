#ifndef __HTTP__
#define __HTTP__
//#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "array.h"
#include "string.h"
#include "ErrorHandler.h"

enum Methods { GET, POST,OPTION };


struct dictionary
{
	char* key;
	char* value;
};

struct Http
{
	struct timeval timeout;
	struct sockaddr_in my_addr;
	struct hostent *h;
	int sockfd;
};

#ifndef http_packet
struct http_packet
{
	struct array headers;
	char* data;
};
#endif

char* Http_StripToHostName(char* host);
char* Http_GetHttpVersion(char* url, struct Error_Handler* error);

int Http_Init(char urlString[], int port, struct Error_Handler* error)
{
	int sock=0;
	char *tmp_ptr=NULL;
	struct sockaddr_in dest_addr={}; 
	struct hostent * host=NULL;
	char* hostname = NULL;
	int rv;
	struct addrinfo hints={}, *servinfo=NULL, *p=NULL;
	
	hostname = Http_StripToHostName(urlString);
	/*
	host = gethostbyname(hostname);

	if (host == NULL)
	{
		error->error = CreateString("Can not get host name");
		goto Http_Init_End;
	}
	 */
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, "http", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}


	sock = socket(servinfo->ai_family, servinfo->ai_socktype,servinfo->ai_protocol);


	if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		if (error->error)
		{
			error->error = CreateString("Can't not create socket!");
			sock =  0;
		}
	}

	Http_Init_End:
	/*
	if (host)
	{
		Free(host);
		host=NULL;
	}
	*/
	if (hostname)
	{
		Free(hostname);
		hostname=NULL;
	}
	return sock;
}

/*Crashes if you use WWW*/
char* Http_StripToHostName(char* host)
{
	struct array* results=NULL;
	char* findThis=NULL;
	char* tempResults=NULL;
	char* stringResults=NULL;
	char* retResults=NULL;
	char* tmpStr=NULL;

	findThis = CreateString("http://");

	results = FindString(findThis,host);

	if (results->count > 0)
	{
		int* firstItem=0;
		firstItem = GetFromArray(results,0);
		if ( (*firstItem) == 0)
		{
			tempResults = MidString(host,(*firstItem) + Strlen(findThis), Strlen(host));
		}
	}

	if (results)
	{
		if (results->head)
		{
			FreeArray(results);
		}
		Free(results);
	}

	if (findThis)
	{
		Free(findThis);
		findThis=NULL;
	}
	
	if (tempResults == NULL)
	{
		tempResults = CreateString(host);
	}

	findThis = CreateString("http://");
	results = FindString(findThis,tempResults);

	if (results->count > 0)
	{
		int* firstItem=0;
		
		firstItem = GetFromArray(results,0);

		if ( (*firstItem) == 0) {
			tmpStr = MidString(tempResults,(*firstItem) + Strlen(findThis),Strlen(host));

			Free(tempResults);
			tempResults = CreateString(tmpStr);
			Free(tmpStr);
		}
	}

	if (results)
	{
		FreeArray(results);
		Free(results);
	}

	if (findThis)
	{
		Free(findThis);
	}

	results = FindString("/",tempResults);
	
	if (results->count > 0)
	{
		int *slashPlacement = GetFromArray(results,0);
		if (*slashPlacement > 0)
		{
			char* tempStr3=NULL;
			tempStr3 = MidString(tempResults,0,*slashPlacement);
			
			if (tempResults)
			{
				Free(tempResults);
			}

			tempResults = CreateString(tempStr3);

			Free(tempStr3);
			tempStr3=NULL;
		}
	}

	if (results)
	{
		FreeArray(results);
		Free(results);
	}
	
	return tempResults;
}

char* Http_Send(char* url, char* payload,struct array *headers,enum Methods method,  struct Error_Handler *error)
{
	char *results=NULL;
	char* sendQuery=NULL;
	char* sendMethod = NULL;
	char* httpVer = NULL;
	int socket=0;
	struct Error_Handler *handleError=NULL;
	struct Error_Handler *handleVError=NULL;

	char* temp1=NULL;
	char* temp2=NULL;


	if (method == GET)
	{
		sendQuery = CreateString("GET ");
	} else 
	if (method == POST)
	{
		sendQuery = CreateString("POST ");
	} else {
		error->error = CreateString("method not found");
		goto http_send_done;
	}

	handleError = Error_Handler();


	socket = Http_Init(url, 80, handleError);


	if (handleError->error)
	{
		error->error  = CreateString(handleError->error);
		Free(handleError->error);
		Free(handleError);
		goto http_send_done;
	}


	if (socket != 0)
	{
		handleVError = Error_Handler();
		httpVer =  Http_GetHttpVersion(url,handleVError);


		if (handleVError->error)
		{
			error->error = CreateString(handleVError->error);
		} else {
			
			temp1 = CatString(sendQuery, url);
			if (sendQuery)
			{
				Free(sendQuery);
				sendQuery=NULL;
			}
			
			sendQuery = CatString(temp1, " ");

			Free(temp1);
			temp1=NULL;

			temp1 = CatString(sendQuery, httpVer);
			
			Free(sendQuery);
			sendQuery=NULL;
	
			sendQuery = CatString(temp1, "\r\n");

			Free(temp1);
			temp1=NULL;
			
			char *hostName = NULL;

			#if 1
			 hostName = Http_StripToHostName(url);

			 if (hostName)
			 {
				temp1 = CatString(sendQuery, "Host: ");
				Free(sendQuery);
				sendQuery=NULL;

				sendQuery = CatString(temp1, hostName);
				Free(temp1);
				temp1 = NULL;

				temp1 = CatString(sendQuery, "\r\n");

				Free(sendQuery);
				sendQuery=NULL;

				sendQuery = CreateString(temp1);

				Free(temp1);
				temp1 = NULL;

				Free(hostName);
				hostName=NULL;
			

			 } else {
				printf("error\n");
				return 0;
			 }
			 #endif


			if (headers != NULL)
			{
				int i=0;

				for (i=0;i<headers->count;i++)
				{
					struct dictionary *header=NULL;
					char* temp = NULL;

					header = (struct dictionary*) GetFromArray(headers,i);
					temp = CatString(sendQuery, header->key);

					Free(sendQuery);
					sendQuery=NULL;

					sendQuery = CatString(temp, ": ");

					Free(temp);
					temp=NULL;
					
					temp = CatString(sendQuery,header->value);

					Free(sendQuery);
					sendQuery=NULL;

					sendQuery = CatString(temp, "\r\n");

					Free(temp);
					temp=NULL;

				}
			}

			if (payload != NULL)
			{
				int payloadLen = Strlen(payload);
				char* temp=NULL;
				char* strlenStr = IntToStr(payloadLen);

				temp = CatString(sendQuery, "Content-Length: ");

				Free(sendQuery);
				sendQuery=NULL;

				sendQuery = CatString(temp,strlenStr);

				Free(temp);
				temp=NULL;

				temp = CatString(sendQuery, "\r\n\r\n");

				Free(sendQuery);
				sendQuery=NULL;
				
				sendQuery = CatString(temp,payload);

				Free(temp);
				temp=NULL;
			} 

			char *temp = NULL;
			temp = CatString(sendQuery, "\r\n");

			Free(sendQuery);
			sendQuery=NULL;

			sendQuery = CreateString(temp);

			Free(temp);
			temp=NULL;
			
			printf("%s\n", sendQuery);
			send(socket,sendQuery,strlen(sendQuery)+1,0);

			Free(sendQuery);
			sendQuery=NULL;

			
			int count=5;
			int totalCount=0;

			while(count > 0)
			{
				char buffer[1025] = {};

				count = recv(socket,buffer,1024,0);

				if (count > 0)
				{
					if (results )
					{
						char *newMemory = NULL;
						int oldCount = totalCount;

						totalCount += count;

						newMemory = (char*) Alloc(totalCount+1);
						memcpy(newMemory,results,oldCount);
						memcpy(newMemory+oldCount,buffer,count);

						Free(results);
						results=NULL;

						results=newMemory;

					} else {
						results = (char*) Alloc(count+1);
						memcpy(results,buffer,count);
						totalCount+=count;
					}
				}
			}
		}
	} else {
		if (handleError->error)
		{
			error->error = CreateString(handleError->error);
		}
	}

http_send_done:
	if (handleVError)
	{
		if (handleVError->error)
		{
			Free(handleVError->error);
			handleVError=NULL;
		}

		Free(handleVError);
		handleVError=NULL;
	}
	if (handleError)
	{
		if (handleError->error)
		{
			Free(handleError->error);
			handleError->error=NULL;
		}
		Free(handleError);
		handleError=NULL;
	}
	if (httpVer)
	{
		Free(httpVer);
		httpVer=NULL;
	}
	if (sendQuery)
	{
		Free(sendQuery);
		sendQuery=NULL;
	}

	if (socket)
	{
		close(socket);
	}
	return results;
}

char* Http_GetHttpVersion(char* url, struct Error_Handler* error)
{
	int readSize = 0;
	int socket=0;
	char* httpVersion=NULL;
	char* httpQuery=NULL;
	char *hostname = Http_StripToHostName(url);
	char buffer[256]={};
	struct array* results=NULL;
	struct Error_Handler *handleError=NULL;

	handleError = Error_Handler();

	socket = Http_Init(hostname, 80, handleError);

	if (socket)
	{
		char* temp=NULL;
		char* temp2=NULL;

		temp = CreateString("HEAD / HTTP/1.0\r\nHOST: ");
		temp2 = CatString(temp,url);

		Free(temp);

		httpQuery = CatString(temp2, "\r\n\r\n");
	
		Free(temp2);

		send(socket,httpQuery,strlen(httpQuery),0);


		while (readSize = recv(socket,buffer,255,0) > 0)
		{
			results = FindString(" ", buffer);
			if (results)
			{
				if (results->count > 1)
				{
					int* firstItem = 0;

					firstItem = GetFromArray(results,0);
					httpVersion = MidString(buffer,0,(*firstItem));
					FreeArray(results);
					Free(results);
					break;
				}
			}

			Free(results);
		}
		close(socket);
	}

	if (handleError);
	{
		Free(handleError);
		handleError=NULL;
	}
	if (httpQuery )
	{
		Free(httpQuery);
		httpQuery =NULL;
	}
	if (hostname)
	{
		Free(hostname);
		hostname=NULL;
	}
	return httpVersion;
}



void ParseHTTPPacket(char* response, struct http_packet* packets)
{
	int headerMark=0;
	int i=0;
	char* rString=NULL;
	bool found=false;
	int responseLen = strlen(response);

	for (i=0;i < responseLen;i++)
	{
		if (response[i] == '\n')
		{
			if (i < responseLen)
			{
				if (response[i+1] == '\r')
				{
					found=true;
					break;
				} else {
					char* header = MidString(response,headerMark,i-1);
					AddToArray(&packets->headers,(int*) header);
					headerMark = i+1;
				}
			}
		}
		
	}

	if (found == true)
	{
		packets->data = MidString(response,i+2,responseLen);
	}
}
#endif

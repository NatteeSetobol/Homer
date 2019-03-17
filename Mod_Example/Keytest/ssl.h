#ifndef SSL_H
#define SSL_H
//#include <stdio.h>
#include "platform.h"
#ifndef ___ARRAY___
#include "array.h"
#endif

#ifndef STRING
#include "string.h"
#endif

#include "ErrorHandler.h"

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

struct httpinfo
{
	struct timeval timeout;
	struct sockaddr_in my_addr;
	struct hostent *h;
	int sockfd;
};

struct http_packet
{
	struct array headers;
	char* data;
	int dataLen;
};


char* StripToHostName(char* host)
{
	struct array* results=NULL;
	char* findThis=NULL;
	char* tempResults=NULL;
	char* stringResults=NULL;
	char* retResults=NULL;
	char* tmpStr=NULL;

	findThis = CreateString("https://");
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
		results=NULL;
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

			if (tempResults)
			{
				Free(tempResults);
				tempResults=NULL;
			}

			tempResults = CreateString(tmpStr);
			if (tmpStr)
			{
				Free(tmpStr);
				tmpStr=NULL;
			}
		}
	}

	if (results)
	{
		FreeArray(results);
		Free(results);
		results=NULL;
	}

	if (findThis)
	{
		Free(findThis);
		findThis=NULL;
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
			if (tempStr3)
			{
				Free(tempStr3);
				tempStr3=NULL;
			}
		}
	}

	if (results)
	{
		FreeArray(results);
		Free(results);
		results=NULL;
	}
	
	return tempResults;
}

char* GetHttpVersion(char* url, struct Error_Handler* error)
{
	int readSize = 0;
	int socket=0;
	char* httpVersion=NULL;
	char* httpQuery=NULL;
	char *hostname = StripToHostName(url);
	char buffer[256]={};
	struct array* results=NULL;

	socket = CreateSocket(hostname,NULL, 80, error);

	if (socket)
	{
		char* temp=NULL;
		char* temp2=NULL;

		temp = CreateString("HEAD / HTTP/1.0\r\nHOST: ");
		temp2 = CatString(temp,url);

		if (temp)
		{
			Free(temp);
			temp=NULL;
		}

		httpQuery = CatString(temp2, "\r\n\r\n");
	
		Free(temp2);
		temp2=NULL;

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
					if (results)
					{
						FreeArray(results);
						Free(results);
						results=NULL;
					}
					break;
				}
			}
			if (results)
			{
				Free(results);
				results=NULL;
			}
		}
		
	} else {
		if (error)
		{
			error->error = CreateString("Can't not create socket");
		}
	}
	
	if (httpQuery)
	{
		Free(httpQuery);
		httpQuery=NULL;
	}
	if (hostname)
	{
		Free(hostname);
		hostname=NULL;
	}

	if (socket)
	{
		close(socket);
	}
	return httpVersion;
}

bool CheckParams(struct array* headers, char* key )
{
	int i=0;
	for (i=0;i<headers->count;i++)
	{
		struct dictionary *dict=NULL;
		dict = (struct dictionary*) GetFromArray(headers,i);

		if (dict)
		{
			if (StrCmp(dict->key,key)) return true;
		}
	}

	return false;
}



char* SSLGetHRaw(struct sslinfo* sslinfo, char* url, struct array* headers, struct Error_Handler* error)
{
	char *sendQuery=NULL;
	char *httpVer=NULL;
	int count=5;
	char* recv = NULL;
	int totalSize=0;
	char* buffer=NULL;

	sslinfo->realURL = CreateString(url);
	sslinfo->host = StripToHostName(url);

	if (SSLInit(sslinfo, sslinfo->host,443,error))
	{
		char* tempQ = NULL;
		char* tempQ2 = NULL;
		
		tempQ = CreateString("GET ");
		tempQ2 = CatString(tempQ, url);

		httpVer = GetHttpVersion(url, error);
		
		Free(tempQ);
		tempQ = NULL;

		tempQ = CatString(tempQ2, " ");

		Free(tempQ2);
		tempQ2=NULL;

		if (httpVer)
		{
			tempQ2 = CatString(tempQ, httpVer);
		} else {
			tempQ2 = CatString(tempQ, "HTTP/1.1");
		}

		Free(tempQ);
		tempQ=NULL;

		tempQ = CatString(tempQ2, "\r\n");

		sendQuery = CreateString(tempQ);

		Free(tempQ);
		Free(tempQ2);
		tempQ=NULL;
		tempQ2=NULL;

		struct array newHeader={};

		if (headers == NULL)
		{
			headers = &newHeader;
		}
		#if 0
		if (!CheckParams(headers, "Accept"))
		{
			struct dictionary acceptParam = {};
			acceptParam.key = CreateString("Accept");
			acceptParam.value = CreateString("html/text");
			AddToArray(headers,(int*) &acceptParam);
		}

		if (!CheckParams(headers, "Host"))
		{
			struct dictionary acceptHost = {};
			acceptHost.key = CreateString("Host");
			acceptHost.value = CreateString(sslinfo->host);
			AddToArray(headers,(int*) &acceptHost);

		}
		/*
		if (!CheckParams(headers, "Accept-Encoding"))
		{
			struct dictionary acceptCE = {};
			acceptCE.key = CreateString("Accept-Encoding");
			acceptCE.value = CreateString("gzip,deflate");
			AddToArray(headers, &acceptCE);
		}
		*/

		if (!CheckParams(headers, "User-Agent"))
		{
			struct dictionary acceptUserAgent = {};
			acceptUserAgent.key = CreateString("User-Agent");
			acceptUserAgent.value = CreateString("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:44.0) Gecko/20100101 Firefox/44.0");
			AddToArray(headers,(int*) &acceptUserAgent);
		}


		if (!CheckParams(headers, "Connection"))
		{
			struct dictionary acceptConnect = {};
			acceptConnect.key = CreateString("Connection");
			acceptConnect.value = CreateString("close");
			AddToArray(headers,(int*) &acceptConnect);
		}
		#endif

		int i=0;

		for (i=0;i<headers->count;i++)
		{
			struct dictionary *dict=NULL;
			dict = (struct dictionary*) GetFromArray(headers,i);

			tempQ = CatString(sendQuery, dict->key);
			tempQ2 = CatString(tempQ, ": ");

			Free(tempQ);
			tempQ=NULL;
			
			tempQ = CatString(tempQ2, dict->value);
			if (tempQ2)
			{

				Free(tempQ2);
				tempQ2=NULL;
			}
			tempQ2 = CatString(tempQ, "\r\n");

			Free(sendQuery);
			sendQuery = CreateString(tempQ2);

			Free(tempQ);
			Free(tempQ2);

			tempQ=NULL;
			tempQ2=NULL;

		}

		tempQ = CatString(sendQuery, "\r\n");

		Free(sendQuery);
		sendQuery=NULL;

		sendQuery = CreateString(tempQ);

		Free(tempQ);
		tempQ=NULL;

		//`printf("%s\n", sendQuery);

		if (sslinfo)
		{
			if (sslinfo->ssl)
			{
				SSL_write(sslinfo->ssl, sendQuery,Strlen(sendQuery));
			}
		}

		while (count > 0)
		{
			char *tempBuffer = (char*) Alloc(535);

			count = SSL_read(sslinfo->ssl,tempBuffer,512);

			if (count != 0)
			{
				if (buffer)
				{
					char *newMemory=NULL;
					int oldCount = totalSize;

					totalSize += count;
					newMemory = (char*) Alloc(totalSize);
					memcpy(newMemory,buffer,oldCount);
					memcpy(newMemory+oldCount, tempBuffer, count);

					Free(buffer);
					buffer=NULL;

					buffer = newMemory;


				} else {
					if (count > 0)
					{
						buffer = (char*) Alloc(count);
						memcpy(buffer,tempBuffer,count);
						totalSize += count;
					}
				}

				if (tempBuffer)
				{
					Free(tempBuffer);
					tempBuffer=NULL;
				}

			} else {
				if (tempBuffer)
				{
					Free(tempBuffer);
					tempBuffer=NULL;
				}

			}

		}
		sslinfo->packetLen = totalSize;

		if (tempQ2)
		{
			Free(tempQ2);
		}
		if (tempQ)
		{
			Free(tempQ);
		}

	}
	if (sslinfo->host)
	{
		Free(sslinfo->host);
		sslinfo->host=NULL;
	}

	if (httpVer)
	{
		Free(httpVer);
		httpVer=NULL;
	}

	if (sendQuery)
	{
		Free(sendQuery);
	}
	

	if (sslinfo->realURL)
	{
		Free(sslinfo->realURL);
		sslinfo->realURL=NULL;
	}
	close(sslinfo->server);
	SSL_free(sslinfo->ssl);
	X509_free(sslinfo->cert);
	SSL_CTX_free(sslinfo->ctx);
	

	return buffer;
}

char* SSLGetH(struct sslinfo* sslinfo, char* url, struct array* headers, struct Error_Handler* error)
{
	char *sendQuery=NULL;
	char *httpVer=NULL;
	int count=5;
	char* recv = NULL;
	struct dictionary acceptUserAgent = {};
	struct dictionary acceptConnect = {};

	sslinfo->realURL = CreateString(url);
	sslinfo->host = StripToHostName(url);

	if (SSLInit(sslinfo, sslinfo->host,443,error))
	{
		char* tempQ = NULL;
		char* tempQ2 = NULL;
		
		tempQ = CreateString("GET ");
		tempQ2 = CatString(tempQ, url);

		httpVer = GetHttpVersion(url, error);
		
		Free(tempQ);
		tempQ = NULL;

		tempQ = CatString(tempQ2, " ");

		Free(tempQ2);
		tempQ2=NULL;

		if (httpVer)
		{
			tempQ2 = CatString(tempQ, httpVer);
		} else {
			tempQ2 = CatString(tempQ, "HTTP/1.0");
		}
		Free(tempQ);
		tempQ=NULL;

		tempQ = CatString(tempQ2, "\r\n");

		sendQuery = CreateString(tempQ);

		Free(tempQ);
		Free(tempQ2);
		tempQ=NULL;
		tempQ2=NULL;

		struct array newHeader={};

		if (headers == NULL)
		{
			headers = &newHeader;
		}

		if (!CheckParams(headers, "Accept"))
		{
			struct dictionary acceptParam = {};
			acceptParam.key = CreateString("Accept");
			acceptParam.value = CreateString("html/text");
			AddToArray(headers,(int*) &acceptParam);
		}

		if (!CheckParams(headers, "Host"))
		{
			struct dictionary acceptHost = {};
			acceptHost.key = CreateString("Host");
			acceptHost.value = CreateString(sslinfo->host);
			AddToArray(headers,(int*) &acceptHost);

		}
		/*
		if (!CheckParams(headers, "Accept-Encoding"))
		{
			struct dictionary acceptCE = {};
			acceptCE.key = CreateString("Accept-Encoding");
			acceptCE.value = CreateString("gzip,deflate");
			AddToArray(headers, &acceptCE);
		}
		*/

		if (!CheckParams(headers, "User-Agent"))
		{
			acceptUserAgent.key = CreateString("User-Agent");
			acceptUserAgent.value = CreateString("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:44.0) Gecko/20100101 Firefox/44.0");
			AddToArray(headers,(int*) &acceptUserAgent);
		}


		if (!CheckParams(headers, "Connection"))
		{
			acceptConnect.key = CreateString("Connection");
			acceptConnect.value = CreateString("close");
			AddToArray(headers,(int*) &acceptConnect);
		}

		int i=0;

		for (i=0;i<headers->count;i++)
		{
			struct dictionary *dict=NULL;
			dict = (struct dictionary*) GetFromArray(headers,i);

			tempQ = CatString(sendQuery, dict->key);
			tempQ2 = CatString(tempQ, ": ");

			Free(tempQ);
			tempQ=NULL;

			tempQ = CatString(tempQ2, dict->value);

			Free(tempQ2);
			tempQ2=NULL;

			tempQ2 = CatString(tempQ, "\r\n");

			Free(sendQuery);
			sendQuery = CreateString(tempQ2);

			Free(tempQ);
			Free(tempQ2);

			tempQ=NULL;
			tempQ2=NULL;

		}

		tempQ = CatString(sendQuery, "\r\n");

		Free(sendQuery);
		sendQuery=NULL;

		sendQuery = CreateString(tempQ);

		Free(tempQ);
		tempQ=NULL;



		
		for (int arrayIndex=0;arrayIndex < headers->count;arrayIndex++)
		{
				struct dictionary *item = GetFromArray(headers,arrayIndex);

				if (item)
				{
					if (item->key)
					{
						Free(item->key);
					}
					if (item->value)
					{
						Free(item->value);
					}
				}

		}
		
		FreeArrayStack(headers);

			//`printf("%s\n", sendQuery);

		if (sslinfo)
		{
			if (sslinfo->ssl)
			{
				SSL_write(sslinfo->ssl, sendQuery,Strlen(sendQuery));
			}
		}

		while (count > 0)
		{
			char buffer[525]={};
			count = SSL_read(sslinfo->ssl,buffer,512);

			if (count != 0)
			{
				#if 1
				if (recv == NULL)
				{
					recv = CreateString(buffer);

				} else {
					char* tempRecv = NULL;
					tempRecv = CatString(recv, buffer);

					Free(recv);

					recv = CreateString(tempRecv);

					Free(tempRecv);
				}
				#endif

			}
		}

	}

	
	if (sendQuery)
	{
		Free(sendQuery);
	}
	if (sslinfo->host)
	{
		Free(sslinfo->host);
		sslinfo->host=NULL;
	}

	if (sslinfo->realURL)
	{
		Free(sslinfo->realURL);
	}

	if (httpVer)
	{
		Free(httpVer);
		httpVer=NULL;
	}

	close(sslinfo->server);
	SSL_free(sslinfo->ssl);
	X509_free(sslinfo->cert);
	SSL_CTX_free(sslinfo->ctx);
	

	return recv;
}


char* SSLPostH(struct sslinfo* sslinfo, char* url, char * payload, struct array* headers, struct Error_Handler* error)
{
	char *sendQuery=NULL;
	char *httpVer=NULL;
	int count=5;
	char* recv = NULL;

	sslinfo->realURL = CreateString(url);
	sslinfo->host = StripToHostName(url);

	if (SSLInit(sslinfo, sslinfo->host,443,error))
	{
		char* tempQ = NULL;
		char* tempQ2 = NULL;
		
		tempQ = CreateString("POST ");
		tempQ2 = CatString(tempQ, url);

		httpVer = GetHttpVersion(url, error);

		Free(tempQ);
		tempQ=NULL;

		tempQ = CatString(tempQ2, " ");

		Free(tempQ2);
		tempQ2=NULL;

		tempQ2 = CatString(tempQ, httpVer);

		if (httpVer)
		{
			Free(httpVer);
			httpVer=NULL;
		}

		Free(tempQ);
		tempQ=NULL;

		free(httpVer);
		httpVer=NULL;

		tempQ = CatString(tempQ2, "\r\n");

		sendQuery = CreateString(tempQ);

		Free(tempQ);
		tempQ=NULL;
		Free(tempQ2);
		tempQ2=NULL;

		struct array newHeader={};

		if (headers == NULL)
		{
			headers = &newHeader;
		}

		if (!CheckParams(headers, "Accept"))
		{
			struct dictionary acceptParam = {};
			acceptParam.key = CreateString("Accept");
			acceptParam.value = CreateString("application/json, text/javascript, */*; q=0.01");
			AddToArray(headers,(int*) &acceptParam);
		}

		if (!CheckParams(headers, "Host"))
		{
			struct dictionary acceptHost = {};
			acceptHost.key = CreateString("Host");
			acceptHost.value = CreateString(sslinfo->host);
			AddToArray(headers,(int*) &acceptHost);
		}

		if (!CheckParams(headers, "User-Agent"))
		{
			struct dictionary acceptUserAgent = {};
			acceptUserAgent.key = CreateString("User-Agent");
			acceptUserAgent.value = CreateString("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:44.0) Gecko/20100101 Firefox/44.0");
			AddToArray(headers,(int*) &acceptUserAgent);
		}

		/*
		if (!CheckParams(headers, "Connection"))
		{
			struct dictionary acceptConnect = {};
			acceptConnect.key = CreateString("Connection");
			acceptConnect.value = CreateString("close");
			AddToArray(headers,(int*) &acceptConnect);
		}

		if (!CheckParams(headers, "Referer"))
		{
			struct dictionary refConnect = {};
			refConnect.key = CreateString("Referer");
			refConnect.value = CreateString("https://www.bing.com/translator");
			AddToArray(headers,(int*)  &refConnect);
		}
		*/


		int i=0;

		for (i=1;i<headers->count;i++)
		{
			struct dictionary *dict=NULL;
			dict = (struct dictionary*) GetFromArray(headers,i);

			tempQ = CatString(sendQuery, dict->key);
			tempQ2 = CatString(tempQ, ": ");

			if (sendQuery)
			{
				Free(sendQuery);
				sendQuery=NULL;
			}

			Free(tempQ);
			tempQ=NULL;

			tempQ = CatString(tempQ2, dict->value);

			Free(tempQ2);
			tempQ2=NULL;

			tempQ2 = CatString(tempQ, "\r\n");

			free(sendQuery);
			sendQuery=NULL;
			sendQuery = CreateString(tempQ2);

			Free(tempQ);
			tempQ=NULL;
			Free(tempQ2);
			tempQ2=NULL;

		}

			struct dictionary *dict=NULL;
			dict = (struct dictionary*) GetFromArray(headers,0);

			tempQ = CatString(sendQuery, dict->key);
			tempQ2 = CatString(tempQ, ": ");

			if (sendQuery)
			{
				Free(sendQuery);
				sendQuery=NULL;
			}

			Free(tempQ);
			tempQ=NULL;

			tempQ = CatString(tempQ2, dict->value);

			Free(tempQ2);
			tempQ2=NULL;

			tempQ2 = CatString(tempQ, "\r\n");

			free(sendQuery);
			sendQuery = CreateString(tempQ2);

			Free(tempQ);
			tempQ=NULL;

			Free(tempQ2);
			tempQ2=NULL;



		if (payload != NULL)
		{
			int contentLen = strlen(payload);

			tempQ = CatString(sendQuery, "Content-Length: ");

			if (sendQuery)
			{
				Free(sendQuery);
				sendQuery=NULL;
			}
			
			char tempNum[256] ={};

			sprintf(tempNum, "%i", contentLen);

			tempQ2 = CatString(tempQ, tempNum);
			
			Free(tempQ);
			tempQ=NULL;


			tempQ = CatString(tempQ2, "\r\n\r\n");
			Free(tempQ2);
			tempQ2=NULL;

			tempQ2 =CatString(tempQ, payload);
			Free(tempQ);
			tempQ=NULL;

			tempQ = CatString(tempQ2,"\r\n");
	
			sendQuery = CreateString(tempQ);

			Free(tempQ);
			Free(tempQ2);

			tempQ=NULL;
			tempQ2=NULL;
		} else {

			tempQ = CatString(sendQuery, "Content-Length: 0\r\n\r\n");

			Free(sendQuery);

			sendQuery = CreateString(tempQ);

			Free(tempQ);
			tempQ=NULL;
		}

		//printf("%s\n", sendQuery);

		if (sslinfo)
		{
			if (sslinfo->ssl)
			{
				SSL_write(sslinfo->ssl, sendQuery,Strlen(sendQuery));
			}
		}

		if (sendQuery)
		{
			Free(sendQuery);
			sendQuery=NULL;
		}

		while (count > 0)
		{
			char buffer[2056]={};
			count = SSL_read(sslinfo->ssl,buffer,2050);

			if (count != 0)
			{
				if (recv == NULL)
				{
					recv = CreateString(buffer);

				} else {
					char* tempRecv = NULL;
					tempRecv = CatString(recv, buffer);

					Free(recv);
					recv=NULL;

					recv = CreateString(tempRecv);

					Free(tempRecv);
					tempRecv=NULL;
				}

			}
		}

	}

	if (sslinfo->realURL)
	{
		Free(sslinfo->realURL);
		sslinfo->realURL=NULL;
	}

	if (sslinfo->host)
	{
		Free(sslinfo->host);
		sslinfo->host=NULL;
	}

	if (sslinfo->realURL)
	{
		Free(sslinfo->realURL);
		sslinfo->realURL=NULL;
	}

	

	close(sslinfo->server);
	SSL_free(sslinfo->ssl);
	X509_free(sslinfo->cert);
	SSL_CTX_free(sslinfo->ctx);

	return recv;

}

char* SSLGet(struct sslinfo* sslinfo, char* url, struct Error_Handler* error)
{
	return SSLGetH( sslinfo, url,NULL,error);
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


void ParseHTTPPacketRaw(char* response, struct http_packet* packets, struct sslinfo* sslinfo)
{
	int headerMark=0;
	int i=0;
	char* rString=NULL;
	int responseLen = sslinfo->packetLen;

	for (i=0;i < responseLen;i++)
	{
		if (response[i] == '\n')
		{
			if (i < responseLen)
			{
				if (response[i+1] == '\r')
				{
					int dataCount = 0;

					packets->data = (char*) Alloc(responseLen);

					for (int j=i+3;j<responseLen;j++)
					{
						packets->data[dataCount] = response[j];

						dataCount++;
						packets->dataLen++;
					}
					break;
				} else {
				}
			}
		}
		
	}
}

#endif

#ifndef SSL_H
#define SSL_H
//#include <stdio.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <string.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

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

struct dictionary
{
	char* key;
	char* value;
};

struct sslinfo
{
	float httpVer;
	char* url;
	char* host;
	char* realURL;
	BIO *certBio;
	X509 *cert;
	X509_NAME *certName;
	SSL_METHOD *method;
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *outBio;
	int server;
	char* httpVersion;
};

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
};


int CreateSocket(char urlString[], BIO *out, int portNumber, struct Error_Handler* error);

bool SSLInit(struct sslinfo* sslInfo,char* url, int pnumber, struct Error_Handler* error)
{
	//struct String findThis={0};
	//struct String findSlash={0};

	sslInfo->server = 0;

	sslInfo->url = CreateString(url);

	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	sslInfo->certBio = BIO_new(BIO_s_file());

	sslInfo->outBio  = BIO_new_fp(stdout, BIO_NOCLOSE);

	if(SSL_library_init() < 0)
	{
		error->error = CreateString("Could not initialize the OpenSSL library !\n");
	} else {
		
		sslInfo->method = (SSL_METHOD*) SSLv23_client_method();
		if ( (sslInfo->ctx = SSL_CTX_new(sslInfo->method)) == NULL)
		{
			if (error)
			{
				error->error = CreateString("Unable to create a new SSL context structure.\n");
			}
		} else {

			char* socketError=NULL;
			SSL_CTX_set_options(sslInfo->ctx, SSL_OP_NO_SSLv2);

			sslInfo->ssl = SSL_new(sslInfo->ctx);

			sslInfo->server = CreateSocket(url, sslInfo->outBio,pnumber,error );

			if(sslInfo->server == 0)
			{
				if (socketError!=NULL)
				{
					if (error)
					{
						error->error = CreateString(socketError);
					}
				}
			} else {
				SSL_set_fd(sslInfo->ssl,sslInfo->server);

				if ( SSL_connect(sslInfo->ssl) != 1 )
				{
					if (error)
					{
						error->error = CreateString("ssl can not connect\n");	
					}
				} else {
					sslInfo->cert = SSL_get_peer_certificate(sslInfo->ssl);

					if (sslInfo->cert == NULL)
					{
						if (error)
						{
							error->error = CreateString("error!\n");	
						}
					} else {
					}
				}
			}
		}
	}

	if (sslInfo->url)
	{
		Free(sslInfo->url);
		sslInfo->url=NULL;
	}

	return true;
}

int CreateSocket(char urlString[], BIO *out, int portNumber, struct Error_Handler* error)
{
	int sock=0;
	char *tmp_ptr=NULL;
	int port=0;
	struct hostent *host=NULL;
	struct sockaddr_in dest_addr={}; 
	port = portNumber;

	host = gethostbyname(urlString);

	if (host == NULL)
	{
		if (error)
		{
			error->error = CreateString("Unable to get host\n");
		}
	} else {

		//ASSERT(host != NULL);

		sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if (sock == -1)
		{
			if (error)
			{
				error->error = CreateString("Socket failed\n");
			}
		} else {

			//ASSERT(sock != NULL);

			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port=htons(port);
			dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

			memset(&(dest_addr.sin_zero),'\0', 8);

			tmp_ptr = inet_ntoa(dest_addr.sin_addr);

			if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) 
			{
				error->error = CreateString("could not connect to site!\n");
			}
		}
	}

	//if (error->error) return 0;
	return sock;
}

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
			results=NULL;
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
		
		close(socket);
	} else {
		error->error = CreateString("Can't not create socket");
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
	return httpVersion;
}

bool CheckParams(struct array* headers, char* key )
{
	int i=0;
	for (i=0;i<headers->count;i++)
	{
		struct dictionary *dict=NULL;
		dict = (struct dictionary*) GetFromArray(headers,i);

		if (StrCmp(dict->key,key)) return true;
	}

	return false;
}




char* SSLGetH(struct sslinfo* sslinfo, char* url, struct array* headers, struct Error_Handler* error)
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
		
		tempQ = CreateString("GET ");
		tempQ2 = CatString(tempQ, url);

		httpVer = GetHttpVersion(url, error);
		
		Free(tempQ);
		tempQ = NULL;

		tempQ = CatString(tempQ2, " ");

		Free(tempQ2);
		tempQ2=NULL;

		tempQ2 = CatString(tempQ, httpVer);

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

			free(sendQuery);
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
			char buffer[2056]={};
			count = SSL_read(sslinfo->ssl,buffer,2050);

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
	
	if (sslinfo)
	{
		if (sslinfo->ssl)
		{
			close(sslinfo->ssl);
		}

		if (sslinfo->server)
		{
			close(sslinfo->server);
		}
	}
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
		#if 0
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
		#endif


		int i=0;
		char *temp=NULL;

		for (i=0;i<headers->count;i++)
		{
			struct dictionary *dict=NULL;
			dict = (struct dictionary*) GetFromArray(headers,i);

			if (dict)
			{
				temp = CatString(sendQuery,dict->key);

				Free(sendQuery);
				sendQuery=NULL;

				sendQuery = CatString(temp,":");

				Free(temp);
				temp=NULL;

				temp = CatString(sendQuery, dict->value);

				Free(sendQuery);
				sendQuery=NULL;

				sendQuery = CatString(temp,"\r\n");

				Free(temp);
				temp=NULL;
			}


		}




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

		printf("%s\n", sendQuery);

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

	if (sslinfo)
	{
		if (sslinfo->server)
		{
			close(sslinfo->server);
		}

		if (sslinfo->ssl)
		{
			close(sslinfo->ssl);
		}
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

#endif

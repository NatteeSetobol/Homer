#ifndef __NIX_PLATFORM__
#define __NIX_PLATFORM__
#include <stdlib.h>

#define OPSSL 1

#ifdef OPSSL
#include <sys/mman.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#endif

#include "ErrorHandler.h"

#ifndef sdictionary
#define sdictionary
struct dictionary
{
	char* key;
	char* value;
};
#endif

#ifdef OPSSL
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
	int packetLen;
};


int CreateSocket(char urlString[], BIO *out, int portNumber, struct Error_Handler* error);

#endif

void* Allocate( size_t size)
{
	void* memoryChunk =malloc(size+1);
	//void* memoryChunk = mmap(0, size ,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
	memset(memoryChunk,0,size);
	return memoryChunk;
}

void FreeMemory(void* address, size_t size)
{
	/*
	if (munmap(address, size) == -1)
	{
		printf("error deallocating\n");
	}*/

	if (address)
	{
		free( address);
		address=NULL;
	}

}

int ReadEntireFile(struct File* file)
{
	FILE *fHandler = fopen(file->name,"rb");
	int fileSize = 0;

	if (fHandler)
	{
		fseek(fHandler,0,SEEK_END);
		fileSize  = ftell(fHandler);
		file->size = fileSize;
		fseek(fHandler,0,SEEK_SET);
		
	
		file->content = (unsigned int*) Alloc(file->size+1);

		fread(file->content,1,file->size,fHandler);
		fclose(fHandler);
	} else {
		printf("Can't not open file\n");
	}

	return fileSize;
}

#ifdef OPSSL
bool SSLInit(struct sslinfo* sslInfo,char* url, int pnumber, struct Error_Handler* error)
{
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
		if (error)
		{
			error->error = CreateString("Could not initialize the OpenSSL library !\n");
		}
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
				if (error)
				{
					error->error = CreateString("could not connect to site!\n");
				}
			}
		}
	}

	//if (error->error) return 0;
	return sock;
}
#endif

#endif

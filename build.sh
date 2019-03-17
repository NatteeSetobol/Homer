#!/bin/sh
#OSX
#gcc irc.c string.c memory.c array.c -I../openssl-0.9.8zg/include -lssl -lcrypto -fsanitize=address  -w
#linux
gcc irc.c string.c array.c memory.c  -lssl -lcrypto -fsanitize=address  -w -ldl -std=gnu99 -lpthread

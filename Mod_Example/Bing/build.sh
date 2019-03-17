#!/bin/sh
gcc -Wall -fPIC -c translator.c string.c array.c -w -g -lssl -lcrypto  -fsanitize=address  -w -ldl  -std=gnu99
#gcc -I/usr/local/opt/openssl/include -lssl -lcrypto -shared -Wl,-soname,myexampleso.3 -o myexample.so.1.0 translator.o   -g -fsanitize=address
gcc  -lssl -lcrypto -shared -Wl,-soname,myexampleso.3 -o myexample.so.1.0 translator.o   -g -fsanitize=address string.c memory.c array.c  -w -std=gnu99




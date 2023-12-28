CFLAGS = -lpthread

build: webserver.o errorwrappers.o connectsock.o requestmanager.o threadpool.o config.o
	gcc webserver.o errorwrappers.o connectsock.o requestmanager.o threadpool.o config.o $(CFLAGS) -o bin/webserver
	@rm *.o

webserver.o: src/webserver.c
	gcc -c src/webserver.c

errorwrappers.o: lib/errorwrappers.c
	gcc -c lib/errorwrappers.c

connectsock.o: lib/connectsock.c
	gcc -c lib/connectsock.c

requestmanager.o: lib/requestmanager.c
	gcc -c lib/requestmanager.c

config.o: lib/config.c
	gcc -c lib/config.c

threadpool.o: lib/threadpool.c 
	gcc $(CFLAGS) -c lib/threadpool.c
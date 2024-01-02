#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/connectsock.h"
#include "../lib/errorwrappers.h"
#include "../lib/threadpool.h"
#include "../lib/requestmanager.h"
#include "../lib/config.h"


#define DEBUG

#ifdef DEBUG

    #define DEB(x) printf("%s\n", x)
    #define DEBI(msg, i) printf("%s: %d\n", msg, i)

#endif

// constants from config file
const char* ROOT_DIR;
const char* SERVICE_PORT;


int main(int argc, char *argv[])
{

    struct sockaddr_in cli_addr; /* the address of client */
	socklen_t cli_len; /* address length */
	fd_set read_fd_set, active_fd_set; /* descriptor sets */

	int nfds, master_sock, client_sock;
    pthread_t threads[MAX_THREADS];

    PoolInit(threads); // initialize thread pool
    Init(); // initialize variables from config file

	nfds = FD_SETSIZE;
	FD_ZERO(&read_fd_set);
	FD_ZERO(&active_fd_set);

    master_sock = PassiveSockTCP(SERVICE_PORT); // creates a listening tcp socket on port SERVICE_PORT
    FD_SET(master_sock, &active_fd_set); // add the socket to the fd set

	for(;;) { // infinite loop for asynchronous input

		memcpy(&read_fd_set, &active_fd_set, sizeof(active_fd_set));

		Select(nfds, &read_fd_set, NULL, NULL, (struct timeval*)0);
		
        if (FD_ISSET(master_sock, &read_fd_set))
        {
            // when we have a connection -> accept it and give the request to a thread
            client_sock = Accept(master_sock, (struct sockaddr*) &cli_addr, (socklen_t*) &cli_len);

            Task request;
            request.taskFunction = ManageRequest;
            request.arg = client_sock;
            AddTask(request);
		
		}

	}

    return 0;
}



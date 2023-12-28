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

const char* ROOT_DIR;
const char* SERVICE_PORT;


int main(int argc, char *argv[])
{
    Init(); // get values from config file

    struct sockaddr_in cli_addr; /* the from address of a client */
	socklen_t cli_len; /* from-address length */
	fd_set read_fd_set, active_fd_set; /* read file descriptor set */
	int fd, nfds, master_sock, client_sock;
    pthread_t threads[MAX_THREADS];

    PoolInit(threads);
	nfds = FD_SETSIZE;
	FD_ZERO(&read_fd_set);
	FD_ZERO(&active_fd_set);

    master_sock = PassiveSockTCP(SERVICE_PORT);
    FD_SET(master_sock, &active_fd_set);

	for(;;) {

		memcpy(&read_fd_set, &active_fd_set, sizeof(active_fd_set));

		Select(nfds, &read_fd_set, NULL, NULL, (struct timeval*)0);
		
        if (FD_ISSET(master_sock, &read_fd_set))
        {
            client_sock = Accept(master_sock, (struct sockaddr*) &cli_addr, (socklen_t*) &cli_len);
           
            Task request;
            request.taskFunction = ManageRequest;
            request.arg = client_sock;
            AddTask(request);
		
		}

	}

    return 0;
}



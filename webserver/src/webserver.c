#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
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
const char* MAX_THREADS;


int main(int argc, char *argv[])
{

    struct sockaddr_in cli_addr; /* the address of client */
	socklen_t cli_len; /* address length */
	fd_set read_fd_set, active_fd_set; /* descriptor sets */
	int nfds, master_sock, client_sock;
    
    Init(); // initialize variables from config file

    pthread_t threads[atoi(MAX_THREADS)];
    PoolInit(threads); // initialize thread pool

	nfds = FD_SETSIZE;
	FD_ZERO(&read_fd_set);
	FD_ZERO(&active_fd_set);

    master_sock = PassiveSockTCP(SERVICE_PORT); // creates a listening tcp socket on port SERVICE_PORT
    FD_SET(master_sock, &active_fd_set); // add the socket to the fd set

    printf("Serving HTTP\nDirectory: %s\nAddress: 0.0.0.0\nPort: %s\nExample: http://0.0.0.0:%s/\nNumber of threads: %s\n\n", ROOT_DIR, SERVICE_PORT, SERVICE_PORT, MAX_THREADS);
    int cnt = 0;
	for(;;) { // infinite loop for asynchronous input

		memcpy(&read_fd_set, &active_fd_set, sizeof(active_fd_set));

		Select(nfds, &read_fd_set, NULL, NULL, (struct timeval*)0);
		
        if (FD_ISSET(master_sock, &read_fd_set))
        {
            // when we have a connection -> accept it and give the request to a thread
            client_sock = Accept(master_sock, (struct sockaddr*) &cli_addr, (socklen_t*) &cli_len);
            char ip_str[INET_ADDRSTRLEN];

            // Convert the binary address to a human-readable string
            if (inet_ntop(AF_INET, &(cli_addr.sin_addr), ip_str, sizeof(ip_str)) == NULL) {
                perror("inet_ntop");
                exit(EXIT_FAILURE);
            }

            time_t currentTime;
            time(&currentTime);

            // Format the time 
            struct tm *timeInfo = localtime(&currentTime);
            char dateString[50];  

            strftime(dateString, sizeof(dateString), "%d/%b/%Y %H:%M:%S", timeInfo);


            printf("%s -- [%s] reqest no. %d\n", ip_str, dateString, ++cnt);
            Task request;
            request.taskFunction = ManageRequest;
            request.arg = client_sock;
            AddTask(request);
		}

	}

    return 0;
}


#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H

#include "errorwrappers.h"
#include "config.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


#define INV_METHOD -1
#define INV_URI -2
#define INV_VERSION -3
#define INV_REQ -4



#define DEBUG


struct request {
    char method[MAX_LEN_METHOD];
    char uri[MAX_LEN_PATH];
    char version[MAX_LEN_VERSION];
};

/**
    @brief It parses the request (req) and it returns the method, uri, and version from the first line of the request
        into the parameters.

    @param method The method is returned here.
    @param uri URI is returned here.
    @param version Version is returned here.
    @param req The request to be parsed.

    @return 0 on success, -1 on error.
*/

int GetRequestLine(char* method, char* uri, char* version, const char* req);

/**
    @brief Parses the request from req and returns the result into the result. It also verifies the result to be a valid
        http request. 
        Valid request:
            * method (GET, ...to be added)
            * uri (a valid and accessible file on the server folder) - vulnerable to path traversal (need to patch)
            * version (only HTTP/1.1 is accepted)

    
*/

int ParseRequest(const char* req, struct request* result);


void ManageRequest(int sock);



#endif
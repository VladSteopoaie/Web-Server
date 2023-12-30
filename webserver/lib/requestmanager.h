#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H

#include "errorwrappers.h"
#include "config.h"
#include "threadpool.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>


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


/////////////////////////////////////
/////// VALIDATION FUNCTIONS ////////
/////////////////////////////////////

int ValidateMethod(const char* method);
int ValidateURI(const char* uri);
int ValidateVersion(const char* version);
int ValidateRequest(struct request* req);

/////////////////////////////////////
////// CONCATENATION FUNCTIONS //////
/////////////////////////////////////
char* AppendString(const char* str1, const char* str2, size_t size);
int AppendIndex(char* uri);
void AppendContentType(char* header, const char* extension);
void AppendContentLength(char* header, size_t size);
void* ConcatData(const void* data1, size_t size1, const void* data2, size_t size2);

////////////////////////////////
/////// OTHER FUNCTIONS ////////
////////////////////////////////

int IsDir(const char* path);
char* GetExtension(const char* path);
size_t GetFile(const char* path, char* body);

//////////////////////////////////////
////////// RESPONSE RELATED //////////
//////////////////////////////////////

char* CreateHeader(const char* code, const char* header_title);
char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message);
char* Http_404();
char* Http_400();
void SendResponse(int sock, char* resp, size_t size);


/////////////////////////////////////
////////// REQUEST RELATED //////////
/////////////////////////////////////

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
            * uri (a valid and accessible file on the server folder)
            * version (only HTTP/1.1 is accepted)

    
*/
int ParseRequest(const char* req, struct request* result);

char* ReadRequest(int sock);

void ManageRequest(int sock);

#endif
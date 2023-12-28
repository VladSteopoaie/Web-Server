#include "parserequest.h"
#include <stdlib.h>
#include <stdio.h>

int GetRequestLine(char* method, char* uri, char* version, const char* req)
{
    char request_copy[strlen(req) + 1];
    char *delim = " \n\r";

    strcpy(request_copy, req);
    
    char *p = strtok(request_copy, delim);
    if (p == NULL || MAX_LEN_METHOD < strlen(p))
        return -1;
    strcpy(method, p);

    p = strtok(NULL, delim);

    if (p == NULL || MAX_LEN_URI < strlen(p))
        return -1;
    strcpy(uri, p);

    p = strtok(NULL, delim);
    if (p == NULL || MAX_LEN_VERSION < strlen(p))
        return -1;
    strcpy(version, p);

    return 0;
}

int ValidateMethod(const char* method)
{
    int nr_methods = 1;
    char *allowed_methods[1] = {"GET"};
    int ret = -1;

    for (int i = 0; i < nr_methods; i ++)
        if (strcmp(method, allowed_methods[i]) == 0){
            ret = 0;
            break;
        }

    return ret;
}

int ValidateURI(const char* uri)
{
    if (access(uri, F_OK) != 0)
        return -1;
    
    return 0;
}

int ValidateVersion(const char* version)
{
    if (strcmp(version, "HTTP/1.1") == 0)
        return 0;
    return -1;
}

int ValidateRequest(struct request* req)
{
    if (ValidateURI(req->uri) != 0)
        return INV_URI;
    if (ValidateMethod(req->method) != 0)
        return INV_METHOD;
    if (ValidateVersion(req->version) != 0)
        return INV_VERSION;

    return 0;
}

int AppendAbsolutePath(char* uri)
{
    char pwd[MAX_LEN_URI];
    if (getcwd(pwd, sizeof(pwd)) == NULL)
    {
        return -1;
    } 

    if (strlen(pwd) + strlen(uri) >= MAX_LEN_URI) // verify if there is enough space to append
        return -1;

    strcat(pwd, uri);
    strcpy(uri, pwd);
    if(uri[strlen(uri) - 1] == '/')
        uri[strlen(uri) - 1] = '\0';
    return 0;
}

int ParseRequest(const char* req, struct request* result)
{
    if (GetRequestLine(result->method, result->uri, result->version, req) < 0)
        return INV_REQ;

    if (AppendAbsolutePath(result->uri) < 0)
        return INV_URI; 

    return ValidateRequest(result);
}



#include "requestmanager.h"
#include "config.h"


#ifdef DEBUG

    #define DEB(x) printf("%s\n", x)
    #define DEBI(msg, i) printf("%s: %d\n", msg, i)

#endif




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

    if (p == NULL || MAX_LEN_PATH < strlen(p))
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
    char aux[MAX_LEN_PATH];

    sprintf(aux, "%s%s", ROOT_DIR, uri);
    strcpy(uri, aux);
    if (uri[strlen(uri) - 1] == '/' || uri[strlen(uri) - 1] == '\\')
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

char* GetExtension(const char* path)
{
    char* ext = malloc(MAX_LEN_PATH);
    bzero(ext, MAX_LEN_PATH);
    for (int i = strlen(path) - 1; i > 0; i --)
        {
            if (path[i] == '.'){
                strcpy(ext, (path + i + 1));
                break;
            }
        }
    return ext;
}



char* CreateHeader(const char* code, const char* header_title)
{
    char *header = malloc(MAX_LEN_HEADER);
    bzero(header, MAX_LEN_HEADER);
    sprintf(header, "HTTP/1.1 %s %s\n", code, header_title);
    return header;
}

char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message)
{
    char *response = malloc(MAX_LEN_FILE);
    bzero(response, MAX_LEN_FILE);
    
    char* header = CreateHeader(code, header_title);

    sprintf(response, 
    "%s \
            \n \
            <html><head><title>%s</title></head>\
            <body><h1>%s</h1><p>%s</p></body></html>\n", header, body_title, body_title, message);

    free(header);
    return response;
}

char* Http_404()
{
    return CreateSimpleResponse("404", "Not Found", "404 Not Found", "This resource can not be found.");
}

char* Http_400()
{
    return CreateSimpleResponse("400", "Bad Request", "400 Bad Request", "The server can not parse this request.");
}

char *ReadRequest(int sock)
{
    char *text = malloc(MAX_LEN_FILE);
    bzero(text, MAX_LEN_FILE);
    int nbytes = Read(sock, text, MAX_LEN_FILE);
    DEB(text);
    return text;
}

void SendResponse(int sock, char* resp)
{
    Write(sock, resp, strlen(resp));
}

void ManageRequest(int sock)
{
    int nbytes, req_result; 
    struct request req;
    char *response;
    

    char* text_received = ReadRequest(sock);
    

    req_result = ParseRequest(text_received, &req);

    switch(req_result)
    {
        case INV_METHOD:
        case INV_VERSION:
        case INV_REQ:
            response = Http_400();
            break;
        case INV_URI:
            response = Http_404();
            break;
    }

    if (req_result == 0)
    {
        char* ext = GetExtension(req.uri);
        char *header = CreateHeader("200", "OK");
        strcat(header, "Content-Type: text/html; charset=utf-8\n");

        char *body;
        if (strcmp(ext, "php") == 0)
        {

        }
        else {
            
        }

        //size_t size = GetFile(uri, body);

        sprintf(response, "%s\n%s", header,body);
        free(header);
        free(body);
        free(ext);
    }

    SendResponse(sock, response);

    free(text_received);
    free(response);
    close(sock);
}
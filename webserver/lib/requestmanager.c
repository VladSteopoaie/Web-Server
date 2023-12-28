#include "requestmanager.h"


#ifdef DEBUG

    #define DEB(x) printf("%s\n", x)
    #define DEBI(msg, i) printf("%s: %d\n", msg, i)

#endif


char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message)
{
    char *response = malloc(MAX_LEN_REQUEST);
    bzero(response, MAX_LEN_REQUEST);

    sprintf(response, 
    "HTTP/1.1 %s %s \
            \n\n \
            <html><head><title>%s</title></head>\
            <body><h1>%s</h1><p>%s</p></body></html>\n", code, header_title, body_title, body_title, message);

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
    char *text = malloc(MAX_LEN_REQUEST);
    bzero(text, MAX_LEN_REQUEST);
    int nbytes = Read(sock, text, MAX_LEN_REQUEST);
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
        response = CreateSimpleResponse("200", "OK", "TEST", "This is just a test.");
    }

    SendResponse(sock, response);

    free(text_received);
    free(response);
    close(sock);
}
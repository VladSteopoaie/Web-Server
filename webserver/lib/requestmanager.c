#include "requestmanager.h"

#ifdef DEBUG

    #define DEB(x) printf("%s\n", x)
    #define DEBI(msg, i) printf("%s: %d\n", msg, i)

#endif

/////////////////////////////////////
/////// VALIDATION FUNCTIONS ////////
/////////////////////////////////////

int ValidateMethod(const char* method)
{
    int nr_methods = 1;
    char* allowed_methods[1] = {"GET"};
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

/////////////////////////////////////
////// CONCATENATION FUNCTIONS //////
/////////////////////////////////////

void AppendAbsolutePath(char* uri)
{
    char aux[MAX_LEN_PATH];

    sprintf(aux, "%s%s", ROOT_DIR, uri);
    strcpy(uri, aux);
    if (uri[strlen(uri) - 1] == '/' || uri[strlen(uri) - 1] == '\\')
        uri[strlen(uri) - 1] = '\0';
}

int AppendIndex(char* uri)
{
    struct stat st;
    size_t file_size;
    char final_path[MAX_LEN_PATH];
    if (stat(uri, &st) < 0)
    {
        perror("stat");
        return -1;
    }

    if (S_ISDIR(st.st_mode))
    {
        snprintf(final_path, MAX_LEN_PATH, "%s/index.php", uri);

        if (stat(final_path, &st) == 0 && S_ISREG(st.st_mode))
            file_size = st.st_size;
        else
        {
            snprintf(final_path, MAX_LEN_PATH, "%s/index.html", uri);

            if(stat(final_path, &st) == 0 && S_ISREG(st.st_mode))
                file_size = st.st_size;
            else 
                return -1;
        }

    }
    else if (S_ISREG(st.st_mode)){
        snprintf(final_path, MAX_LEN_PATH, "%s", uri);
    }
    else {
        return -1;
    }
    strcpy(uri, final_path);
    return 0;
}

void AppendContentType(char* header, const char* extension)
{
    char type[MAX_LEN_PATH], content_type[MAX_LEN_PATH];
    if (strcmp(extension, "css") == 0)
    {
        strcpy(type, "text/css");
    }
    else if (strcmp(extension, "js") == 0)
    {
        strcpy(type, "application/javascript");
    }
    else {
        strcpy(type, "text/html");
    }

    sprintf(content_type, "Content-Type: %s; charset=utf-8\n", type);
    strcat(header, content_type);
}

void AppendContentLength(char* header, size_t size)
{
    char length[MAX_LEN_PATH];
    sprintf(length, "Content-Length: %ld\n", size);
    strcat(header, length);
}

void* ConcatData(const void* data1, size_t size1, const void* data2, size_t size2)
{
    void* concat_data = malloc(size1 + size2);
    memcpy(concat_data, data1, size1);
    memcpy(concat_data + size1, data2, size2);
    return concat_data;
}

////////////////////////////////
/////// OTHER FUNCTIONS ////////
////////////////////////////////


char* GetExtension(const char* path)
{
    char* ext = malloc(MAX_LEN_PATH);
    bzero(ext, MAX_LEN_PATH);
    for (int i = strlen(path) - 1; i >= 0; i --)
        {
            if (path[i] == '.'){
                strcpy(ext, (path + i + 1));
                break;
            }
        }
    return ext;
}

size_t GetFile(const char* path, char* body)
{
    struct stat st;
    if (stat(path, &st) < 0)
    {
        perror("stat");
        return -1;
    }

    if (st.st_size > MAX_LEN_FILE){
        fprintf(stderr, "File too large.\n");
        return -1;
    }

    int fd = Open(path, O_RDONLY);
    char* addr = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    memcpy(body, addr, st.st_size);
    body[st.st_size] = '\0';

    munmap(addr, st.st_size);
    return st.st_size;
}



//////////////////////////////////////
////////// RESPONSE RELATED //////////
//////////////////////////////////////


char* CreateHeader(const char* code, const char* header_title)
{
    char* header = malloc(MAX_LEN_HEADER);
    bzero(header, MAX_LEN_HEADER);
    sprintf(header, "HTTP/1.1 %s %s\n", code, header_title);
    return header;
}

char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message)
{
    char* response = malloc(MAX_LEN_FILE + MAX_LEN_HEADER);
    bzero(response, MAX_LEN_FILE);
    char* body = malloc(MAX_LEN_FILE);
    bzero(body, MAX_LEN_FILE);
    
    char* header = CreateHeader(code, header_title);

    sprintf(body, 
    "<html><head><title>%s</title></head>\
            <body style=\"text-align: center;\"><h1>%s</h1><p>%s</p></body></html>\n", body_title, body_title, message);
    
    AppendContentType(header, "html");
    AppendContentLength(header, strlen(body));

    strcat(header, "\n");
    response = ConcatData(header, strlen(header), body, strlen(body));

    free(header);
    free(body);
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


void SendResponse(int sock, char* resp, size_t size)
{
    Write(sock, resp, size);
}

/////////////////////////////////////
////////// REQUEST RELATED //////////
/////////////////////////////////////

int GetRequestLine(char* method, char* uri, char* version, const char* req)
{
    char request_copy[strlen(req) + 1];
    char* delim = " \n\r";

    strcpy(request_copy, req);
    
    char* p = strtok(request_copy, delim);
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

int ParseRequest(const char* req, struct request* result)
{
    if (GetRequestLine(result->method, result->uri, result->version, req) < 0)
        return INV_REQ;

    AppendAbsolutePath(result->uri);
    if (AppendIndex(result->uri) < 0)
        return INV_URI;

    return ValidateRequest(result);
}

char* ReadRequest(int sock)
{
    char* text = malloc(MAX_LEN_HEADER);
    bzero(text, MAX_LEN_HEADER);
    int nbytes = Read(sock, text, MAX_LEN_HEADER);
    DEB(text);
    return text;
}

void ManageRequest(int sock)
{
    size_t response_size;
    int req_result; 
    struct request req;
    char* response;

    char* text_received = ReadRequest(sock);
    

    req_result = ParseRequest(text_received, &req);

    switch(req_result)
    {
        case INV_METHOD:
        case INV_VERSION:
        case INV_REQ:
            response = Http_400(), response_size = strlen(response);
            break;
        case INV_URI:
            response = Http_404(), response_size = strlen(response);
            break;
    }

    if (req_result == 0)
    {
        LockRequestMutex();

        size_t body_size;
        char* ext;
        char* header;
        char* body = malloc(MAX_LEN_FILE);
        bzero(body, MAX_LEN_FILE);

        ext = GetExtension(req.uri);    
        
        header = CreateHeader("200", "OK");
        
        AppendContentType(header, ext);


        if (strcmp(ext, "php") == 0)
        {
            header[0] = '\0';
        }
        else {
            body_size = GetFile(req.uri, body);

            if (body_size <= 0)
            {
                header[0] = '\0';
            } 
            else {
                AppendContentLength(header, body_size);
            }
        }
        
        if (header[0] == '\0')
            response = Http_404(), response_size = strlen(response);
        else{
            strcat(header, "\n");
            response = ConcatData(header, strlen(header), body, body_size);
            response_size = strlen(header) + body_size;
        }

        free(header);
        free(body);
        free(ext);

        UnlockRequestMutex();
    }
    SendResponse(sock, response, response_size);

    free(text_received);
    free(response);
    close(sock);
}
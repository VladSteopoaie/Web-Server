#include "requestmanager.h"

#ifdef DEBUG

    #define DEB(x) printf("%s\n", x)
    #define DEBI(msg, i) printf("%s: %d\n", msg, i)
    #define DEBA(msg, i) printf("%s: %p\n", msg, i)

#endif

/////////////////////////////////////
/////// VALIDATION FUNCTIONS ////////
/////////////////////////////////////

int ValidateMethod(const char* method)
{
    int nr_methods = 3;
    char* allowed_methods[3] = {"GET", "HEAD", "POST"};
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
    char* path = AppendString(ROOT_DIR, uri, MAX_LEN_PATH);
    int result;

    result = access(path, F_OK);

    if (IsDir(path))
    {
        if (path[strlen(path) - 1] != '/')
            strcat(path, "/");
        if (AppendIndex(path) < 0)
            result = -1;
    }

    
    free(path);
    return result;
}

int ValidateVersion(const char* version)
{
    if ((strcmp(version, "HTTP/1.1") == 0) || (strcmp(version, "HTTP/1.0") == 0))
        return 0;
    return -1;
}

int ValidateRequest(struct request* req)
{
    if (ValidateMethod(req->method) != 0)
        return INV_METHOD;
    if (ValidateVersion(req->version) != 0)
        return INV_VERSION;
    if (ValidateURI(req->uri) != 0)
        return INV_URI;

    return 0;
}

/////////////////////////////////////
////// CONCATENATION FUNCTIONS //////
/////////////////////////////////////

char* AppendString(const char* str1, const char* str2, size_t size)
{
    char* result = malloc(size);
    bzero(result, size);

    snprintf(result, size, "%s%s", str1, str2);
    
    return result;
}

int AppendIndex(char* uri)
{
    struct stat st;
    char final_path[MAX_LEN_PATH];
    
    snprintf(final_path, MAX_LEN_PATH, "%sindex.html", uri);

    if (stat(final_path, &st) == 0 && S_ISREG(st.st_mode))
        strcpy(uri, final_path);
    else
    {
        snprintf(final_path, MAX_LEN_PATH, "%sindex.php", uri);

        if(stat(final_path, &st) == 0 && S_ISREG(st.st_mode))
            strcpy(uri, final_path);
        else 
            return -1;
    }

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
    else if (strcmp(extension, "jpeg") == 0)
    {
        strcpy(type, "image/jpeg");
    }
    else {
        strcpy(type, "text/html");
    }

    sprintf(content_type, "Content-Type: %s; charset=utf-8\n", type);
    strcat(header, content_type);
}

void AppendContentLength(char* header, size_t size)
{
    if (size == 0)
        return;
    char length[MAX_LEN_PATH];
    sprintf(length, "Content-Length: %ld\n", size);
    strcat(header, length);
}

void AppendLocation(char* header, const char* path)
{
    char location[MAX_LEN_PATH];
    sprintf(location, "Location: %s\n", path);
    strcat(header, location);
}

void* ConcatData(const void* data1, size_t size1, const void* data2, size_t size2)
{
    void* concat_data = malloc(size1 + size2);
    bzero(concat_data, size1 + size2);

    if (size1 != 0)
        memcpy(concat_data, data1, size1);
    if (size2 != 0)
        memcpy(concat_data + size1, data2, size2);
    return concat_data;
}

////////////////////////////////
/////// OTHER FUNCTIONS ////////
////////////////////////////////

void GetPostAttributes(const char* request, char* attributes)
{   
    size_t size = 0;
    char* content_length = "Content-Length: ";
    char* start = strstr(request, "\r\n\r\n");
    char* length = strstr(request, content_length);

    if (start == NULL || length == NULL)
    {
        attributes[0] = '\0';
        return;
    }
    length += strlen(content_length);
    start += 4;

    for(char* i = length; isdigit(*i); i ++)
    {
        size = 10 * size + (*i) - '0';
    }

    memcpy(attributes, start, size);
    attributes[size] = '\0';


}

int IsDir(const char* path)
{
    struct stat st;
    if (stat(path, &st) < 0)
    {
        return 0;
    }

    if (S_ISDIR(st.st_mode))
    {
        return 1;
    }

    return 0;
}


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

ssize_t GetFile(const char* path, char* body)
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
    if (Read(fd, body, st.st_size) < 0)
        return -1;
    close(fd);
    
    return st.st_size;
}



//////////////////////////////////////
////////// RESPONSE RELATED //////////
//////////////////////////////////////


char* CreateHeader(const char* code, const char* header_title)
{
    char* header = malloc(MAX_LEN_HEADER);
    bzero(header, MAX_LEN_HEADER);
    
    time_t currentTime;
    struct tm* timeInfo; // convert the current time to the HTTP date format
    char dateString[50];  // final date
    
    time(&currentTime);
    timeInfo = gmtime(&currentTime);  // GMT time
    strftime(dateString, sizeof(dateString), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    sprintf(header, "HTTP/1.1 %s %s\nServer: DancingGhost/1.0\nConnection: close\nDate: %s\n", code, header_title, dateString);
    return header;
}

char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message)
{
    char* response;
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

char* Http_500()
{
    return CreateSimpleResponse("500", "Internal Sever Error", "500 Internal Server Error", "There has been a problem on the server.");
}

char* Http_501()
{
    return CreateSimpleResponse("501", "Not Implemented", "501 Not Implemented", "Request method not recoginsed by the server.");
}

char* Http_505()
{
    return CreateSimpleResponse("505", "HTTP Version Not Supported", "505 HTTP Version Not Supported", "Only HTTP/1.1 is supported.");
}

char* Http_301(const char* new_path)
{
    char* header = CreateHeader("301", "Moved Permanently");
    AppendLocation(header, new_path);
    return header;
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

    return ValidateRequest(result);
}

char* ReadRequest(int sock)
{
    char* text = malloc(MAX_LEN_HEADER);
    bzero(text, MAX_LEN_HEADER);
    int nbytes = Read(sock, text, MAX_LEN_HEADER);
    return text;
}

void ManageRequest(int sock)
{
    // usleep(50); // just for speed tests
    size_t response_size;
    int req_result; 
    struct request req;
    char* response = NULL;

    char* text_received = ReadRequest(sock);
    req_result = ParseRequest(text_received, &req);

    switch(req_result)
    {
        case INV_METHOD:
            response = Http_501();
            break;
        case INV_REQ:
            response = Http_400();
            break;
        case INV_URI:
            response = Http_404();
            break;
        case INV_VERSION:
            response = Http_505();
            break;
    }

    if (req_result == 0)
    {

        ssize_t body_size;
        int command_result = 0;
        char command[MAX_LEN_PATH];
        char* ext, *header, *path;
        char* body = malloc(MAX_LEN_FILE);
        bzero(body, MAX_LEN_FILE);
        
        path = AppendString(ROOT_DIR, req.uri, MAX_LEN_PATH);
        if (IsDir(path))
        {
            if (path[strlen(path) -1] != '/') {
                strcat(req.uri, "/");
                strcat(path, "/");
                header = Http_301(req.uri);
            }
            else {
                header = CreateHeader("200", "OK");
            }
            AppendIndex(path);
        }
        else {
            header = CreateHeader("200", "OK");
        }

        ext = GetExtension(path);    
        
        AppendContentType(header, ext);

        
        if (strcmp(ext, "php") == 0)
        {
            char post_attributes[MAX_LEN_PATH];
            
            if (strcmp(req.method, "GET") == 0)
            {
                post_attributes[0] = '\0';
            }
            else if (strcmp(req.method, "POST") == 0)
            {
                GetPostAttributes(text_received, post_attributes);
            }

            snprintf(command, MAX_LEN_PATH,
            "php \"%s/preprocess.php\" \"%s\" \"%s\" > \"%s/processed.html\"", 
                    CONFIG_DIR, post_attributes, path, ROOT_DIR);
            LockRequestMutex();
            command_result = system(command);

            if (command_result == 0)
            {
                snprintf(path, MAX_LEN_PATH, "%s/processed.html", ROOT_DIR);
                body_size = GetFile(path, body);
                unlink(path);
            }
            else {
                body_size = -1;
            }
            UnlockRequestMutex();
        }
        else
            body_size = GetFile(path, body);
        
        body_size < 0 ? header[0] = '\0' : AppendContentLength(header, body_size);

        
        if (header[0] == '\0') {
            response = (command_result == 0) ? Http_404() : Http_500();
            response_size = strlen(response);
        }
        else{
            strcat(header, "\n");

            response = ConcatData(header, strlen(header), body, body_size);
            response_size = strlen(header) + body_size;

        }

        free(header);
        free(ext);
        free(path);
        free(body);      
    }
    else {
        response_size = strlen(response);
    }

    if (strcmp(req.method, "HEAD") == 0)
    {
        char* header_end = strstr(response, "\n\n");
        response_size = strlen(response) - strlen(header_end);
    }

    SendResponse(sock, response, response_size);

    free(text_received);
    free(response);
    close(sock);
}
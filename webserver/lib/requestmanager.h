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
#include <ctype.h>
#include <time.h>

// Error macros

#define INV_METHOD -1
#define INV_URI -2
#define INV_VERSION -3
#define INV_REQ -4

struct request {
    char method[MAX_LEN_METHOD];
    char uri[MAX_LEN_PATH];
    char version[MAX_LEN_VERSION];
};


/////////////////////////////////////
/////// VALIDATION FUNCTIONS ////////
/////////////////////////////////////

/**
 * @brief Checks if 'method' is GET, POST or HEAD
 *
 * @param method the string to be checked 
 * @return 0 on success, -1 on failure
 */

int ValidateMethod(const char* method);

/**
 * @brief Checks if the provided path is accessible, it also checks (if the path is a directory) if an
 * index page is available
 * @param uri the path to a resource on the server
 * @return 0 on success, -1 on failure
 */

int ValidateURI(const char* uri);

/**
 * @brief Checks if the string is HTTP/1.1 or HTTP/1.0
 * 
 * @param version the string to be checked
 * @return 0 on success, -1 on failure
 */

int ValidateVersion(const char* version);

/**
 * @brief Puts together all the validation functions for validating a request
 * 
 * @param req the request to be validated
 * @return 0 on success 
 * INV_METHOD if method invalid
 * INV_URI if path invalid
 * INV_VERSION if version invalid
 */

int ValidateRequest(struct request* req);

/////////////////////////////////////
////// CONCATENATION FUNCTIONS //////
/////////////////////////////////////

/**
 * @brief Concatenates 'str2' to 'str1' and returns a new string of size 'size'
 * 
 * @param str1 
 * @param str2 
 * @param size 
 * @return The resulting string
 */

char* AppendString(const char* str1, const char* str2, size_t size);

/**
 * @brief Checks if "index.html" or "index.php" ca be appended to the path and if the final string is a valid file,
 * if successful 'uri' will be modified
 * @param uri the initial path to check
 * @return 0 on success, -1 on failure
 */

int AppendIndex(char* uri);

/**
 * @brief Appends the "Content-Type" to 'header' according to 'extension'
 * 
 * @param header the header string of a web request
 * @param extension the extension of the file to be sent
 */

void AppendContentType(char* header, const char* extension);

/**
 * @brief Appends the "Content-Length" to 'header' according to 'size'
 * 
 * @param header the header string of a web request
 * @param size the size of the file to be sent
 */

void AppendContentLength(char* header, size_t size);

/**
 * @brief Appends the "Location" to 'header' according to 'path
 * 
 * @param header the header string of a web request
 * @param path the path to be appended
 */

void AppendLocation(char* header, const char* path);

/**
 * @brief Concatenates two pieces of data
 * 
 * @param data1 first data piece
 * @param size1 sieze of the first data piece
 * @param data2 second data piece
 * @param size2 size of the second data piece
 * @return The resulting data piece (different from the first 2)
 */

void* ConcatData(const void* data1, size_t size1, const void* data2, size_t size2);

////////////////////////////////
/////// OTHER FUNCTIONS ////////
////////////////////////////////

/**
 * @brief Get the Post Attributes of a web request in a string
 * 
 * @param request the string containing the web request
 * @param attributes Post Attributes string (NULL if attributes could not be provided)
 */

void GetPostAttributes(const char* request, char* attributes);

/**
 * @brief Returns the string till \n or \0 is encountered
 * 
 * @param text the string to be searched
 * @param line The resulting string
 */

void GetFirstLine(const char* text, char* line);

/**
 * @brief Checks if 'path' is a valid directory
 * 
 * @param path the path to be checked
 * @return 0 on success, -1 on failure
 */

int IsDir(const char* path);

/**
 * @brief Gets the string from path from the last "." character till the end of the string
 * 
 * @param path the path to the file
 * @return The extension of the file as a string
 */

char* GetExtension(const char* path);

/**
 * @brief Gets the contents of the file from 'path'
 * 
 * @param path the path to the file
 * @param body the resulting data from the file
 * @return The size of data
 */

ssize_t GetFile(const char* path, char* body);

//////////////////////////////////////
////////// RESPONSE RELATED //////////
//////////////////////////////////////

/**
 * @brief Creates the header string
    Example of header:
        HTTP/1.1 'code' 'header_title'
        Server: DancingGhost
        Connection: close
        Date: (the date of creation)
 * @param code result code of the web response
 * @param header_title a message
 * @return The resulting header string
 */

char* CreateHeader(const char* code, const char* header_title);

/**
 * @brief Creates a simple response string
 * 
 * @param code response code
 * @param header_title response message in the header
 * @param body_title the title of the page
 * @param message the message to be shown on the page
 * @return The resulting response string
 */

char* CreateSimpleResponse(const char* code, const char* header_title, const char* body_title, const char* message);

// Http responses created with CreateSimpleResponse()
char* Http_301(const char* new_path);
char* Http_400();
char* Http_404();
char* Http_500();
char* Http_501();
char* Http_505();

/**
 * @brief Writes data to 'sock' file descriptor
 * 
 * @param sock file descriptor
 * @param resp the response string
 * @param size size of the response string
 */

void SendResponse(int sock, char* resp, size_t size);


/////////////////////////////////////
////////// REQUEST RELATED //////////
/////////////////////////////////////

/**
    @brief It parses the request (req) and it returns the method, uri, and version from the first line of the request
        into the parameters

    @param method The method is returned here
    @param uri URI is returned here
    @param version Version is returned here
    @param req The request to be parsed

    @return 0 on success, -1 on error
*/
int GetRequestLine(char* method, char* uri, char* version, const char* req);

/**
    @brief Parses the request from req and returns the result into the result. It also verifies the result to be a valid
        http request. 
        Valid request:
            * method (GET, POST, HEAD)
            * uri (a valid and accessible file on the server folder)
            * version (only HTTP/1.1 and HTTP/1.0 are accepted)

    @param req the request string to be parsed
    @param result the request object resulted 
    @return 0 on success 
    INV_METHOD if method invalid
    INV_URI if path invalid
    INV_VERSION if version invalid
    INV_REQ if request is invalid
*/
int ParseRequest(const char* req, struct request* result);

/**
 * @brief Reads data from 'sock'
 * 
 * @param sock the file descriptor
 * @return The resulting string after data was read
 */

char* ReadRequest(int sock);

/**
 * @brief This is the brain funcion of the web-server. It reads, parses, validates and respons to the requests. This is the 
 * task a thread on the webserver will perform.
 * @param conn the connection to be managed
 */

void ManageRequest(struct connection* conn);

#endif
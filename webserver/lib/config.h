#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "errorwrappers.h"

#define MAX_LEN_PATH 256
#define MAX_LEN_PORT 6
#define MAX_LEN_FILE 4098

// #define MAX_LEN_URI 256
#define MAX_LEN_METHOD 16
#define MAX_LEN_VERSION 16
#define MAX_LEN_HEADER 300
// #define MAX_LEN_REQUEST 4098

#define CONFIG_FILE "/home/hefaistos/Desktop/An II/Sem I/SO/Lab/Proiect/Web-Server/webserver/conf/webserver.conf"
extern const char* ROOT_DIR;
extern const char* SERVICE_PORT;

void Init();
char* GetFromConfigFile(const char* val, const char* default_val, size_t size);
char* GetConfigFile();


#endif
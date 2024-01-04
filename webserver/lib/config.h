#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "errorwrappers.h"

#define MAX_LEN_PORT 6
#define MAX_LEN_METHOD 16
#define MAX_LEN_VERSION 16
#define MAX_LEN_PATH 512
#define MAX_LEN_HEADER 4096
#define MAX_LEN_FILE 102400

#define DEFAULT_ROOT_DIR "/etc/webserver/http/home"
#define DEFAULT_SERVICE_PORT "8888"
#define CONFIG_FILE "/etc/webserver/webserver.conf"
#define CONFIG_DIR "/etc/webserver"

extern const char* ROOT_DIR;
extern const char* SERVICE_PORT;
extern const char* MAX_THREADS;

/**
    @brief Initialize ROOT_DIR and SERVICE_PORT with the values found in the configuraion
        file or with the default values (DEFAULT_ROOT_DIR and DEFAULT_SERVICE_PORT) if no configuration
        file is found.
*/
void Init();

/**
    @brief Searches for a property in the configuration file and returns it's value.
    
    @param val The string to be searched in the configuration file.
    @param default_val The default value to be returned in case of error.
    @param size The max size of the string to be returned

    @return Returns the string after the first encounter of 'val' till \n or \0. On error
        (no configuration file or there is no match with 'val') 'default_val' is returned.
*/
char* GetFromConfigFile(char* config_file, const char* val, const char* default_val, size_t size);

/**
    @brief Gets the contents of the file stored in CONFIG_FILE

    @return Returns the contents of the file or NULL on error.
*/
char* GetConfigFile();


#endif
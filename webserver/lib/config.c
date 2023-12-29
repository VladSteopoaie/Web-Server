#include "config.h"

void Init()
{
    SERVICE_PORT = GetFromConfigFile("port=", DEFAULT_SERVICE_PORT, MAX_LEN_PORT);
    ROOT_DIR = GetFromConfigFile("root=", DEFAULT_ROOT_DIR, MAX_LEN_PATH);
}

char* GetConfigFile()
{
    // initializing space for config file
    char* config_file = malloc(MAX_LEN_FILE);
    bzero(config_file, MAX_LEN_FILE);

    int fd = open(CONFIG_FILE, O_RDONLY);

    if (fd < 0)
    {
        fprintf(stderr, "[-] No config file found. Working with default values. Add the file %s for configuration.", CONFIG_FILE);
        return NULL;
    }
    
    int nbytes = Read(fd, config_file, MAX_LEN_FILE); 
    config_file[nbytes] = '\0';
    
    close(fd);
    return config_file;
}

char* GetFromConfigFile(const char* val, const char* default_val, size_t size)
{
    char* config_file = GetConfigFile();
    char* result = malloc(size);
    bzero(result, size);

    char* i = strstr(config_file, val);

    if (i == NULL)
    {
        // if no match is found, default_val will be returned
        strcpy(result, default_val);
    }
    else {
        i += strlen(val); // get the pointer to the end of the string
        int j;
        // copy the contents till \n or \0 encountered
        for (j = 0; *(i + j) != '\n' && *(i + j) != '\0'; j ++)
        {
            result[j] = *(i + j);

        }
        result[j] = '\0';
    }


    free(config_file);
    return result;
}

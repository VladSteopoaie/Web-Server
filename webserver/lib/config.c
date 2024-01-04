#include "config.h"

void Init()
{
    char* config_file = GetConfigFile();
 
    SERVICE_PORT = GetFromConfigFile(config_file, "port=", DEFAULT_SERVICE_PORT, MAX_LEN_PORT);
    ROOT_DIR = GetFromConfigFile(config_file, "root=", DEFAULT_ROOT_DIR, MAX_LEN_PATH);
    MAX_THREADS = GetFromConfigFile(config_file, "threads=", "8", MAX_LEN_PORT);
    free(config_file);
}

char* GetConfigFile()
{
    // initializing space for config file
    char* config_file = malloc(MAX_LEN_FILE);
    bzero(config_file, MAX_LEN_FILE);

    int fd = open(CONFIG_FILE, O_RDONLY);

    if (fd < 0)
    {
        fprintf(stderr, "[-] No config file found. Working with default values. Add the file %s for configuration.\n", CONFIG_FILE);
        return NULL;
    }
    
    int nbytes = Read(fd, config_file, MAX_LEN_FILE); 
    config_file[nbytes] = '\0';
    
    close(fd);
    return config_file;
}

char* GetFromConfigFile(char* config_file, const char* val, const char* default_val, size_t size)
{
    char* result = malloc(size);
    bzero(result, size);

    char* i = config_file == NULL ? NULL : strstr(config_file, val);

    if (i == NULL)
    {
        // if no match is found, default_val will be returned
        strcpy(result, default_val);
    }
    else {
        i += strlen(val); // get the pointer to the end of the string
        int j;
        // copy the contents till \n or \0 encountered
        for (j = 0; *(i + j) != '\n' && *(i + j) != '\0' && j < size; j ++)
        {
            result[j] = *(i + j);

        }
        result[j] = '\0';
    }


    return result;
}

#include "config.h"

void Init()
{
    SERVICE_PORT = GetFromConfigFile("port=", "8888", MAX_LEN_PORT);
    ROOT_DIR = GetFromConfigFile("root=", "~/http/home", MAX_LEN_PATH);
}

char* GetConfigFile()
{
    char* config_file = malloc(MAX_LEN_FILE);
    int fd = Open(CONFIG_FILE, O_RDONLY);
    
    int nbytes = Read(fd, config_file, MAX_LEN_FILE); 
    config_file[nbytes] = '\0';

    return config_file;
}

char* GetFromConfigFile(const char* val, const char* default_val, size_t size)
{
    char* config_file = GetConfigFile();
    char* result = malloc(size);

    char* i = strstr(config_file, val);

    if (i == NULL)
    {
        strcpy(result, default_val);
    }
    else {
        i += strlen(val);
        int j;
        for (j = 0; *(i + j) != '\n' && *(i + j) != '\0'; j ++)
        {
            result[j] = *(i + j);

        }
        result[j] = '\0';
    }


    free(config_file);
    return result;
}

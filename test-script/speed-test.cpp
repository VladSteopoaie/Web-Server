/*
    This script is only for testing purposes.
    
    It generates static (ex: html files, images, css ...) and dynamic (ex: php files) requests and sends them to the
    server using curl command.
    
    This is only for testing the webserver's performance. For better results usleep must be added in the servers code
    to simulate the processing of a bigger task.
    
    Execute the program with the command time, ex: "time ./test"
    
*/

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include <cstring>

using namespace std;


int main(int argc, char** argv) {
    
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <host> <port> static / dynamic\n";
        return -1;
    }
    
    string host = string(argv[1]);
    string port = string(argv[2]);
    vector<string> methods = {"GET", "POST", "HEAD", "WRONG"};
    vector<string> paths = {"", "pages", "newpages/index.php", "thisShouldNotExist", "newpages", "index.php",
        "just.jpeg", "newpages/styles.css", "newpages/script.js"};
    vector<string> versions = {"-1", "-0"};
    string post_attributes = "-d \"firstName=Gustav&lastName=Ferguson\""; // post attributes in curl command

    int request_number = 100;
    vector<pid_t> processes;


    for (int i = 0; i < request_number; ++i) {
        string method = methods[rand() % methods.size()];

        string path;
        if (method == "POST") {
            path = (rand() % 3 == 0) ? "newpages/index.php" : paths[rand() % paths.size()];
        } else {
            path = paths[rand() % paths.size()];
        }

        string version = (i % 10 == 0) ? versions[rand() % versions.size()] : "-1";
        string command;
        
        if (strcmp(argv[3], "dynamic") == 0)
        {
            command = "curl -i -X " + method + " " + ((method == "POST") ? post_attributes : "") + " " +
                                version + " http://" + host + ":" + port + "/" + path + " 1> /dev/null 2>&1";
        }
        else if (strcmp(argv[3], "static") == 0) {
            command = "curl -i -X GET -1 http://" + host + ":" + port + "/ 1> /dev/null 2>&1";
        }
        else {
            cerr << "Usage: " << argv[0] << " <host> <port> static / dynamic\n";
            return -1;
        }


        pid_t pid = fork();
        if (pid == 0) { // child process
            system(command.c_str()); // send dynamic and static requests randomly
            _exit(EXIT_SUCCESS);
        } else if (pid > 0) { // parent process
            processes.push_back(pid);
        } else { // fork failed
            cerr << "Fork failed!" << endl;
            perror("fork");
            return EXIT_FAILURE;
        }
    }

    for (pid_t pid : processes) {
        int status;
        waitpid(pid, &status, 0);
    }


    return 0;
}

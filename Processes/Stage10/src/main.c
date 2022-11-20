#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[], char* env[]) {
    const char* process = "Unknown";
    char* file_name = argv[1];
    printf("filename: %s\n", file_name);
    pid_t pid = fork();  

    if (pid == -1) {
        printf("Fork error");
    }

    if (pid == 0) {
        process = "Child";
        printf("%s: start working\n", process);
        execle("./Stage10/subprogramm/processes_worker", "processes_worker", file_name,  NULL, env);
    }

    if (pid > 0) {
        process = "Parent";
        printf("%s start working\n", process);
        int status;
        pid_t child_pid = wait(&status);
        printf("%s: terminated child pid=%d\n", process, child_pid);
    }

    //printf("%s environment:\n", process);
    //for(int i = 0; env[i] != NULL; ++i) {
    //    printf("%s\n", env[i]);
    //}

    return 0;
}
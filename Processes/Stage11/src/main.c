#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* env[]) {
    const char* process = "Unknown";
    pid_t pid = fork();  

    if (pid == -1) {
        printf("Fork error");
    }

    if (pid == 0) {
        process = "Child";
        printf("%s: start working\n", process);
        printf("System call:\n");
        system("ls ~/");
        printf("Exec call:\n");
        int res = execlp("ls", "ls", "/home/students/c/churkin.ki/", NULL);
        printf("%d\n", res);
    }

    if (pid > 0) {
        process = "Parent";
        printf("%s start working\n", process);
        int status;
        pid_t child_pid = wait(&status);
        printf("%s: terminated child pid=%d\n", process, child_pid);
    }

    return 0;
}
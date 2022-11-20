#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    const char* process = "Unknown";

    pid_t pid = fork();  

    if (pid == -1) {
        printf("Fork error");
    }

    if (pid == 0) {
        process = "Child";
        printf("%s: start working\n", process);
    }

    if (pid > 0) {
        process = "Parent";
        printf("%s start working\n", process);
        int status;
        pid_t child_pid = wait(&status);
        printf("%s: terminated child pid=%d\n", process, child_pid);
    }

    printf("%s: pid=%d\n", process, getpid());
    printf("%s: parent pid=%d\n", process, getppid());
    printf("%s: gid=%d\n", process, getgid());

    if (pid == 0) {
        setpgrp();
    }
    pause();

    return 0;
}
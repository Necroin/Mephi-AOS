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
        sleep(3);
        printf("%s: start working\n", process);
    }

    if (pid > 0) {
        process = "Parent";
        printf("%s start working\n", process);
    }

    printf("%s: pid=%d\n", process, getpid());
    printf("%s: parent pid=%d\n", process, getppid());
    printf("%s: gid=%d\n", process, getgid());

    return 0;
}
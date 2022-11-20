#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    
    pid_t pid = fork();  

    if (pid == -1) {
        printf("Fork error");
    }

    if (pid == 0) {
        printf("CHILD: start working with pid=%d, parent pid=%d\n", getpid(), getppid());
        for(int i=0; i<5; ++i){
            printf("CHILD: working\n");
        }
    }

    if (pid > 0) {
        printf("PARENT: start working with pid=%d, child pid=%d\n", getpid(), pid);
        for(int i=0; i<5; ++i){
            printf("PARENT: working\n");
        }
        int status;
        wait(&status);
    }

    return 0;
}
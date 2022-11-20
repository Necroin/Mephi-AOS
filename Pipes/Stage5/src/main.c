#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
int pipe_descriptors[2];

const int READ_PIPE = 0;
const int WRITE_PIPE = 1;


int main(int argc, char* argv[]) {
    if(pipe(pipe_descriptors) != 0) {
        perror("Error: create noname pipe failed\n");
        return 0;
    }

    int pid = fork();
    
    if (pid == -1) {
        perror("Fork failed\n");
        return 0;
    }

    if (pid == 0) {
    	close(pipe_descriptors[READ_PIPE]);
        dup2(pipe_descriptors[WRITE_PIPE], WRITE_PIPE);
        execlp("who", "who", NULL);
        printf("Exec failed\n");
        return 0;
    }

    close(pipe_descriptors[WRITE_PIPE]);
    dup2(WRITE_PIPE, pipe_descriptors[WRITE_PIPE]);
    dup2(pipe_descriptors[READ_PIPE], READ_PIPE);
    execlp("wc", "wc", "-l", NULL);
    return 0;
}
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

int threads_count = 0;

void wait_threads() {
    while(threads_count > 0) {
        sleep(1);
    }
}

void signal_handler(int value) {
    printf("SIGCHLD Handler\n");
    while(waitpid(-1, NULL, 0) != -1){
        --threads_count;
    }
}

void set_wait_childs() {
    struct sigaction signal_action;

    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGCHLD, &signal_action, NULL);
}

int main(int argc, char* argv[]) {
    set_wait_childs();

    if(pipe(pipe_descriptors) != 0) {
        perror("Error: create noname pipe failed\n");
        return 0;
    }

    ++threads_count;
    int pid = fork();
    
    if (pid == -1) {
        perror("Fork failed\n");
        return 0;
    }

    if (pid == 0) {
        printf("Child started\n");
        int write_descriptor = pipe_descriptors[WRITE_PIPE];
        dup2(write_descriptor, WRITE_PIPE);
        execl("./Stage4/message_pusher", "message_pusher", NULL);
        printf("Exec failed\n");
        return 0;
    }

    printf("Parent started\n");
    
    int read_descriptor = pipe_descriptors[READ_PIPE];
    close(pipe_descriptors[WRITE_PIPE]);
    
    const int buf_size = 80;
    char read_buf[buf_size + 1];
    int read_count = 0;
    
    while((read_count = read(read_descriptor, (void*)read_buf, buf_size)) != 0) {
    	read_buf[read_count] = '\0';    
    	printf("%s", read_buf);
    }

    close(read_descriptor);

    wait_threads();
    return 0;
}
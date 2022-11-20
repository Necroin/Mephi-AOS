#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

const int READ_PIPE = 0;
const int WRITE_PIPE = 1;

int threads_count = 0;

void wait_threads() {
    while(threads_count > 0) {
        sleep(1);
    }
}

void signal_handler(int value) {
    printf("SIGCHLD Handler");
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

int main() {
    set_wait_childs();

    int parent_pipes[2], child_pipes[2];
    
    const char child_message[] = "Message from Child\n";
    const char parent_message[] = "Message from Parent\n";

    const int buf_size = 1024;
    char read_buf[buf_size + 1];
    int read_count = 0;


    if (pipe(parent_pipes) != 0) { perror("Parent pipe create failed"); return 0; }
    if (pipe(child_pipes) != 0) { perror("Child pipe create failed"); return 0; }
    
    ++threads_count;
    int pid = fork();
    
    if (pid == -1) {
        perror("Fork failed\n");
        return 0;
    }

    if (pid == 0) {
        close(parent_pipes[WRITE_PIPE]);
        close(child_pipes[READ_PIPE]);

        int write_descriptor = child_pipes[WRITE_PIPE];
        int read_descriptor = parent_pipes[READ_PIPE];

        write(write_descriptor, child_message, strlen(child_message));

        sleep(2);

        read_count = read(read_descriptor, (void*)read_buf, buf_size);
		read_buf[read_count] = '\0';
    	printf("%s", read_buf);

        return 0;
    }

    close(parent_pipes[READ_PIPE]);
    close(child_pipes[WRITE_PIPE]);

    int write_descriptor = parent_pipes[WRITE_PIPE];
    int read_descriptor = child_pipes[READ_PIPE];

    write(write_descriptor, parent_message, strlen(parent_message));
    
    sleep(2);

    read_count = read(read_descriptor, (void*)read_buf, buf_size);
		read_buf[read_count] = '\0';
    	printf("%s", read_buf);
    
    wait_threads();

    return 0;
   
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int threads_count = 0;

void wait_threads() {
    while(threads_count > 0) {
        sleep(1);
    }
}

void signal_handler(int value) {
    while(waitpid(-1, NULL, 0) != -1){
        const char* message = "\nHandle signal SIGCHLD\n";
        write(1, message, 22);
        --threads_count;
    }
}

int main(int argc, char* argv[]) {
    struct sigaction signal_action;

    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGCHLD, &signal_action, NULL);

    for(int i = 0; i < 5; ++i) {
        ++threads_count;
        if(fork() == 0) {
            printf("Child started\n");
            sleep(3);
            return 0;
        }
    }
    wait_threads();
    return 0;
}
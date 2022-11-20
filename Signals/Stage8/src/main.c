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
    int status;
    while(waitpid(-1, &status, 0) != -1){
        printf("\nHandle signal SIGCHLD\n");
        printf("Exited status: %d", status);
        --threads_count;
    }
}


void fork_signal_handler(int value) {
    printf("usr1 singal handler\n");
}

int main(int argc, char* argv[]) {
    struct sigaction signal_action;

    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGCHLD, &signal_action, NULL);
    pid_t pid = fork();
    if (pid == -1) {
        return 0;
    }
    ++threads_count;
    if(pid == 0) {
        printf("Child started\n");
        signal(SIGUSR1, fork_signal_handler);
        for(int i = 0; i < 10; ++i) {
            sleep(1);
            printf("%d\n", i);
        }
        return 0;
    }
    sleep(5);
    kill(pid, SIGUSR1);
    wait_threads();
    return 0;
}
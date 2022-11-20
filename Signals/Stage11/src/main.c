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
        printf("Exited status: %d\n", status);
        --threads_count;
    }
}

void main_signal_handler(int value) {
    printf("main signal handler start\n");
    sleep(5);
    printf("main signal handler end\n");
}


void fork_signal_handler(int value) {
    printf("fork signal handler start\n");
    sleep(5);
    printf("fork signal handler end\n");
}

int main(int argc, char* argv[]) {
    struct sigaction signal_action;

    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGCHLD, &signal_action, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    pid_t pid = fork();
    if (pid == -1) {
        return 0;
    }
    ++threads_count;
    if(pid == 0) {
        printf("Child started\n");
        pid = getppid();
        signal_action.sa_handler = fork_signal_handler;
        sigemptyset(&signal_action.sa_mask);
        signal_action.sa_flags = 0;
        signal_action.sa_restorer = NULL;
        sigaction(SIGUSR1, &signal_action, NULL);
        
        for(int i = 0; i < 5; ++i) {
            sleep(2);
            kill(pid, SIGUSR2);
            sigsuspend(&signal_action.sa_mask);
        }
        return 0;
    }
    signal_action.sa_handler = main_signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;
    sigaction(SIGUSR2, &signal_action, NULL);

    for(int i = 0; i < 5; ++i) {
        sleep(2);
        sigsuspend(&signal_action.sa_mask);
        kill(pid, SIGUSR1);
    }

    wait_threads();
    return 0;
}
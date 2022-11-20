#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

bool work = true;
struct sigaction old_signal_action;

void signal_handler(int value) {
    const char* message = "\nHandle signal SIGINT\n";
    write(1, message, 22);
    sleep(1);
    sigaction(SIGINT, &old_signal_action, NULL);
    work = false;
}

int main(int argc, char* argv[]) {
    struct sigaction signal_action;

    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGINT, &signal_action, &old_signal_action);

    while(work) sleep(1);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

bool work = true;

void signal_handler(int value) {
    const char* message = "\nHandle signal SIGINT\n";
    write(1, message, 22);
    sleep(1);
    signal(SIGINT, SIG_DFL);
    work = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    while(work) sleep(1);
    return 0;
}
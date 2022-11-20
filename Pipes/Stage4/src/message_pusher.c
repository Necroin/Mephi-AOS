#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

const int READ_PIPE = 0;
const int WRITE_PIPE = 1;

int main(int argc, char* argv[]) {
    int write_descriptor = WRITE_PIPE;

    const char* message = "Message from Child\n";
    for(int i =0; i < 5; ++i) {
        write(write_descriptor, message, strlen(message));
    }

    close(write_descriptor);
    
    return 0;
}
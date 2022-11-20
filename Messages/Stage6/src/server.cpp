#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <cerrno>

#include "msg_types.h"


void thread_handler(int value) {
    while(waitpid(-1, NULL, 0) != -1){
        printf("Handle signal SIGCHLD\n");
    }
}

void set_handle_threads() {
    struct sigaction signal_action;

    signal_action.sa_handler = thread_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    signal_action.sa_restorer = NULL;

    sigaction(SIGCHLD, &signal_action, NULL);
}

int main(int argc, char* argv[]) {
    set_handle_threads();

    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage6";
    key_t listen_key = ftok(msg_key_string, 0);

    int lsten_id = msgget(listen_key, IPC_CREAT | 0660);
    if (lsten_id == -1) {
        printf("[SERVER] ERROR: can't create listen queue\n");
        return 0;
    }
    printf("[SERVER] Started listen with queue id %d\n", lsten_id);

    struct {
        long mtype;
        char mtext[1];
    } listen_buf;

    while(true) {
        int res = msgrcv(lsten_id, &listen_buf, 0, 0, 0);

        if (res >= 0) {
            int connect_id = listen_buf.mtype;

            int pid = fork();
            if (pid == -1) {
                printf("[SERVER] ERROR: can't create connection thread\n");
            }

            if (pid == 0) {
                printf("[SERVER] Start process connection with id %d\n", connect_id);

                const int BUFFFER_SIZE = 8192;

                struct {
                    long mtype;
                    char mtext[BUFFFER_SIZE];
                } msg_buf;

                msg_buf.mtype = (long)MsgTypes::ACCESS;

                memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
                strcpy(msg_buf.mtext, "Connection accepted");

                int res = msgsnd(connect_id, &msg_buf, strlen(msg_buf.mtext)+1, 0);
                if (res == -1) {
                    printf("[SERVER] ERROR: message send failed to queue %d\n (%s)", connect_id, strerror(errno));
                }
                return 0;
            }
        }
    }

    
    return 0;
}
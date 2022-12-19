#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <cerrno>
#include <sys/types.h>
#include <unistd.h>

#include "msg_types.h"

#include <set>
std::set<int> connections;


void thread_handler(int value) {
    int status;
    while(waitpid(-1, &status, 0) != -1){
        printf("Handle signal SIGCHLD\n");
        connections.erase(status);
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

    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage7";
    key_t listen_key = ftok(msg_key_string, 0);
    key_t mailing_key = ftok(msg_key_string, 1);

    int lsten_id = msgget(listen_key, IPC_CREAT | 0666);
    if (lsten_id == -1) {
        printf("[SERVER] ERROR: can't create listen queue\n");
        return 0;
    }
    printf("[SERVER] Started listen with queue id %d\n", lsten_id);

    int mailing_id = msgget(mailing_key, IPC_CREAT | 0666);
    if (mailing_id == -1) {
        printf("[SERVER] ERROR: can't create mailing queue\n");
        return 0;
    }

    struct {
        long mtype;
        char mtext[1];
    } listen_buf;

    
    const int BUFFFER_SIZE = 8192;

    struct {
        long mtype;
        char mtext[BUFFFER_SIZE];
    } mailing_buf;

    while(true) {
        int res = msgrcv(lsten_id, &listen_buf, 0, 0, IPC_NOWAIT | MSG_NOERROR);

        if (res >= 0) {
            int connect_id = listen_buf.mtype;

            connections.insert(connect_id);

            int pid = fork();
            if (pid == -1) {
                printf("[SERVER] ERROR: can't create connection thread\n");
            }

            if (pid == 0) {
                printf("[SERVER] Start process connection with id %d\n", connect_id);

                struct {
                    long mtype;
                    char mtext[BUFFFER_SIZE];
                } msg_buf;

                msg_buf.mtype = (long)MsgTypes::ACCESS;

                memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
                strcpy(msg_buf.mtext, "Connection accepted");

                int res = msgsnd(connect_id, &msg_buf, strlen(msg_buf.mtext)+1, 0);
                if (res == -1) {
                    printf(
                        "[SERVER] ERROR: message send failed to queue %d\n (%s)",
                        connect_id, 
                        strerror(errno)
                    );
                }

                while(true) {
                    memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
                    res = msgrcv(connect_id, &msg_buf, BUFFFER_SIZE, MsgTypes::QUERY, 0);

                    if (res == -1) {
                        printf(
                            "[SERVER] ERROR: message receive failed from queue %d (%s)\n", 
                            connect_id, 
                            strerror(errno)
                        );
                        break;
                    }

                    msg_buf.mtype = connect_id;

                    res = msgsnd(mailing_id, &msg_buf, strlen(msg_buf.mtext)+1, 0);
                    if (res == -1) {
                        printf(
                            "[SERVER] ERROR: message send failed to queue %d (%s)\n",
                            mailing_id, 
                            strerror(errno)
                        );
                    }
                }

                return connect_id;
            }
        }

        memset(mailing_buf.mtext, '\0', sizeof(mailing_buf.mtext));
        res = msgrcv(mailing_id, &mailing_buf, BUFFFER_SIZE, 0, IPC_NOWAIT | MSG_NOERROR);
        if (res >= 0) {
            int sender_id = mailing_buf.mtype;
            mailing_buf.mtype = (long)MsgTypes::RESPONSE;

            for (auto&& id: connections) {
                if (id != sender_id) {
                    res = msgsnd(id, &mailing_buf, strlen(mailing_buf.mtext)+1, 0);
                    if (res == -1) {
                        printf(
                            "[SERVER] ERROR: message send failed to queue %d (%s)\n",
                            id, 
                            strerror(errno)
                        );
                    }
                }
            }
        }
    }

    return 0;
}
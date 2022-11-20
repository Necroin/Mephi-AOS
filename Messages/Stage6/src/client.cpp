#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "msg_types.h"

int main(int argc, char* argv[]) {
    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage6";
    key_t server_key = ftok(msg_key_string, 0);

    int server_id = msgget(server_key, 0);
    if (server_id == -1) {
        printf("[CLIENT] ERROR: can't open server listen queue\n");
        return 0;
    }
    printf("[CLIENT] Started with server queue id %d\n", server_id);

    int self_id = msgget(IPC_PRIVATE, IPC_CREAT | 0660);

    if (self_id == -1) {
        printf("[CLIENT] ERROR: can't create connection queue\n");
        return 0;
    }
    printf("[CLIENT] Started with self queue id %d\n", self_id);

    const int BUFFFER_SIZE = 8192;

    struct {
        long mtype;
        char mtext[BUFFFER_SIZE];
    } msg_buf;

    msg_buf.mtype = self_id;

    int res = msgsnd(server_id, &msg_buf, 0, 0);
    if (res == -1) {
        printf("[CLIENT] ERROR: message send failed to server listen queue\n");
        msgctl(self_id, IPC_RMID, 0);
    }


    res = msgrcv(self_id, &msg_buf, BUFFFER_SIZE, MsgTypes::ACCESS, 0);
    if (res == -1) {
        printf("[CLIENT] ERROR: message read failed\n");
        msgctl(self_id, IPC_RMID, 0);
        return 0;
    }

    printf("Type: %d\n", msg_buf.mtype);;
    printf("Message text: %s\n", msg_buf.mtext);
    msgctl(self_id, IPC_RMID, 0);
    return 0;
}
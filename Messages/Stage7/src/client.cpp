#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "msg_types.h"

int main(int argc, char* argv[]) {
    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage7";
    key_t server_key = ftok(msg_key_string, 0);

    int server_id = msgget(server_key, 0);
    if (server_id == -1) {
        printf("[CLIENT] ERROR: can't open server listen queue\n");
        return 0;
    }
    printf("[CLIENT] Started with server queue id %d\n", server_id);

    int self_id = msgget(ftok(msg_key_string, 1 + std::stoi(argv[1])), IPC_CREAT | 0666);

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
        return 0;
    }


    res = msgrcv(self_id, &msg_buf, BUFFFER_SIZE, MsgTypes::ACCESS, 0);
    if (res == -1) {
        printf("[CLIENT] ERROR: message read failed\n");
        msgctl(self_id, IPC_RMID, 0);
        return 0;
    }

    printf("Type: %d\n", msg_buf.mtype);;
    printf("Message text: %s\n", msg_buf.mtext);

    int pid = fork();

    if (pid == 0) {
        while(true){
        res = msgrcv(self_id, &msg_buf, BUFFFER_SIZE, MsgTypes::RESPONSE, 0);
        if (res == -1) {
            printf("[CLIENT] ERROR: message read failed\n");
            msgctl(self_id, IPC_RMID, 0);
            return 0;
        }

        printf("Type: %d\n", msg_buf.mtype);;
        printf("Message text: %s\n", msg_buf.mtext);
        }
        return 0;
    }

    std::string line;
    while(true) {
        memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
        std::getline(std::cin, line);
        if (std::cin.eof()) { break; }

        msg_buf.mtype = MsgTypes::QUERY;
        strcpy(msg_buf.mtext, line.c_str());

        res = msgsnd(self_id, &msg_buf, strlen(msg_buf.mtext) + 1, 0);
        if (res == -1) {
            printf("[CLIENT] ERROR: message send failed to server listen queue\n");
            msgctl(self_id, IPC_RMID, 0);
            break;
        }

    }

    msgctl(self_id, IPC_RMID, 0);
    return 0;
}
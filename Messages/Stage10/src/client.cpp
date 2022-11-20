#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <iostream>

#include "msg_types.h"

int main(int argc, char* argv[]) {
    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage10";
    key_t server_key = ftok(msg_key_string, 0);

    int server_id = msgget(server_key, 0);
    if (server_id == -1) {
        printf("[CLIENT] ERROR: can't open server listen queue\n");
        return 0;
    }
    printf("[CLIENT] Started with server queue id %d\n", server_id);

    const int BUFFFER_SIZE = 8192;

    struct {
        long mtype;
        char mtext[BUFFFER_SIZE];
    } msg_buf;

    memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));

    long self_pid = getpid();

    long access_type = self_pid | ((long)MsgTypes::ACCESS_RESPONSE << ((sizeof(long) - 1) * 8));
    long query_type = self_pid | ((long)MsgTypes::QUERY << ((sizeof(long) - 1) * 8));
    long response_type = self_pid | ((long)MsgTypes::RESPONSE << ((sizeof(long) - 1) * 8));

    msg_buf.mtype = MsgTypes::ACCESS_REQUEST;

    strcpy(msg_buf.mtext, std::to_string(self_pid).c_str());
    printf("%s\n", msg_buf.mtext);
    int res = msgsnd(server_id, &msg_buf, strlen(msg_buf.mtext), 0);
    if (res == -1) {
        printf("[CLIENT] ERROR: message send failed to server listen queue\n");
        return 0;
    }


    res = msgrcv(server_id, &msg_buf, BUFFFER_SIZE, access_type, 0);
    if (res == -1) {
        printf("[CLIENT] ERROR: message read failed\n");
        return 0;
    }

    printf("Type: %d\n", msg_buf.mtype);
    printf("Message text: %s\n", msg_buf.mtext);

    int pid = fork();

    if (pid == 0) {
        while(true){
        res = msgrcv(server_id, &msg_buf, BUFFFER_SIZE, response_type, 0);
        if (res == -1) {
            printf("[CLIENT] ERROR: message read failed\n");
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

        msg_buf.mtype = query_type;
        strcpy(msg_buf.mtext, line.c_str());

        res = msgsnd(server_id, &msg_buf, strlen(msg_buf.mtext) + 1, 0);
        if (res == -1) {
            printf("[CLIENT] ERROR: message send failed to server listen queue\n");
            break;
        }

    }

    return 0;
}
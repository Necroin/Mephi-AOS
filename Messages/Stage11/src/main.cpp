#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    const char* msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages/Stage11";
    key_t queue_key = ftok(msg_key_string, 0);

    int queue_id = msgget(queue_key, IPC_CREAT | 0666);
    if (queue_id == -1) {
        printf("ERROR: can't open server listen queue\n");
        return 0;
    }

    struct {
        long mtype;
        char mtext[100];
    } msg_buf;

    memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));

    long self_pid = getpid();

    int pid = fork();

    if (pid == 0) {
        long parent_pid = self_pid;
        self_pid = getpid();

        for(int i = 0; i < 5; ++i) {
            int res = msgrcv(queue_id, &msg_buf, sizeof(msg_buf.mtext), parent_pid, 0);
            if (res == -1) {
                printf("[CHILD] ERROR: message read failed\n");
                return 0;
            }

            printf("Type: %d\n", msg_buf.mtype);;
            printf("Message text: %s\n", msg_buf.mtext);

            msg_buf.mtype = self_pid;
            memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
            strcpy(msg_buf.mtext, "Message from Child");
            res = msgsnd(queue_id, &msg_buf, strlen(msg_buf.mtext), 0);
            if (res == -1) {
                printf("[CHILD] ERROR: message send failed to server listen queue\n");
                return 0;
            }
        }
        return 0;
    }

    for(int i = 0; i < 5; ++i) {
        msg_buf.mtype = self_pid;
        memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
        strcpy(msg_buf.mtext, "Message from Parent");

        int res = msgsnd(queue_id, &msg_buf, strlen(msg_buf.mtext), 0);
        if (res == -1) {
            printf("[PARENT] ERROR: message send failed to server listen queue\n");
            return 0;
        }

        res = msgrcv(queue_id, &msg_buf, sizeof(msg_buf.mtext), pid, 0);
        if (res == -1) {
            printf("[PARENT] ERROR: message read failed\n");
            return 0;
        }

        printf("Type: %d\n", msg_buf.mtype);
        printf("Message text: %s\n", msg_buf.mtext);
    }

    wait(NULL);
    msgctl(queue_id, IPC_RMID, 0);
    return 0;
}
#include <functional>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string.h>


int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "usage: write <queue> <message type> <message>";
        return 0;
    }

    std::string type_arg = argv[2];
    char* message = argv[3];

    int message_type = std::stoi(type_arg);

    std::string msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages";
    key_t msg_queue_key = ftok(msg_key_string.c_str(), std::stoi(argv[1]));

    int msg_queue_id = msgget(msg_queue_key, IPC_CREAT | 0660);
    if (msg_queue_id == -1) {
        std::cout << "ERROR: message queue not exist" << std::endl;
        return 0;
    }

    const int buffer_size = 8192;
    struct {
        long mtype;
        char mtext[buffer_size];
    } msg_buf;

    msg_buf.mtype = (long)message_type;

    memset(msg_buf.mtext, '\0', sizeof(msg_buf.mtext));
    strcpy(msg_buf.mtext, message);

    int res = msgsnd(msg_queue_id, &msg_buf, strlen(msg_buf.mtext)+1, 0);
    if (res == -1) {
        std::cout << "ERROR: message send failed" << std::endl;
        return 0;
    }

    return 0;
}
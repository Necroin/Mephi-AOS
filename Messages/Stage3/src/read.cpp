#include <functional>
#include <string>
#include <vector>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    std::string msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages";
    key_t msg_queue_key = ftok(msg_key_string.c_str(), std::stoi(argv[1]));

    int msg_queue_id = msgget(msg_queue_key, 0);
    if (msg_queue_id == -1) {
        std::cout << "ERROR: message queue not exist" << std::endl;
        return 0;
    }

    const int buffer_size = 8192;
    struct {
        long mtype;
        char mtext[buffer_size];
    } msg_buf;

    int res = msgrcv(msg_queue_id, &msg_buf, buffer_size, std::stoi(argv[2]), IPC_NOWAIT);
    if (res == -1) {
        std::cout << "ERROR: message read failed" << std::endl;
        return 0;
    }

    std::cout << "Type: " << msg_buf.mtype << std::endl;
    std::cout << "Message text: " << std::string(msg_buf.mtext, res) << std::endl;
    
    return 0;
}
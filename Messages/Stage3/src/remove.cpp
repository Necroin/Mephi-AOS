#include <functional>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char* argv[]) {
    std::string msg_key_string = "/home/students/c/churkin.ki/Laboratories/AOS/Messages";
    key_t msg_queue_key = ftok(msg_key_string.c_str(), std::stoi(argv[1]));

    int msg_queue_id = msgget(msg_queue_key, 0);
    if (msg_queue_id == -1) {
        std::cout << "ERROR: message queue not exist" << std::endl;
        return 0;
    }
    
    int remove_res = msgctl(msg_queue_id, IPC_RMID, 0);
    if (remove_res == -1) {
        std::cout << "ERROR: remove message queue failed" << std::endl;
        return 0;
    }
    
    std::cout << "Remove message queue success" << std::endl;

    return 0;
}
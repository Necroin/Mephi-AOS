#include <functional>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string.h>


int main(int argc, char* argv[]) {
    int msg_queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0660);
    if (msg_queue_id == -1) {
        std::cout << "ERROR: message queue not exist" << std::endl;
        return 0;
    }

    return 0;
}
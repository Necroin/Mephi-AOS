#include <functional>
#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char* argv[]) {
    int remove_res = msgctl(std::stoi(argv[1]), IPC_RMID, 0);
    if (remove_res == -1) {
        std::cout << "ERROR: remove message queue failed" << std::endl;
        return 0;
    }
    
    std::cout << "Remove message queue success" << std::endl;

    return 0;
}
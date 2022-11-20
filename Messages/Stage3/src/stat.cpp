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
    struct msqid_ds queue_status;
    if(msgctl(msg_queue_id, IPC_STAT, &queue_status) < 0) {
	    perror("ERROR: get stat failed");
        return 0;
    }

    std::cout << "Real user id of the queue creator: " << queue_status.msg_perm.cuid << std::endl;
    std::cout << "Real group id of the queue creator: " << queue_status.msg_perm.cgid << std::endl;

    std::cout << "Effective user id of the queue creator: " << queue_status.msg_perm.uid << std::endl;
    std::cout << "Effective group id of the queue creator: " << queue_status.msg_perm.gid << std::endl;
    std::cout << "Permissions: " << queue_status.msg_perm.mode << std::endl;
    std::cout << "Message queue id: " << msg_queue_id << std::endl;
    std::cout << "Messages on queue: " << queue_status.msg_qnum << std::endl;
    std::cout << "Last message sent by process: " << queue_status.msg_lspid << " at " << queue_status.msg_stime << std::endl;

    std::cout << "Last message received by process : " << queue_status.msg_lrpid << " at " << queue_status.msg_rtime << std::endl;
    std::cout << "Current number of bytes on queue: " << queue_status.msg_cbytes << std::endl;
    std::cout << "Maximum number of bytes allowed on the queue: " << queue_status.msg_qbytes << std::endl;

    return 0;
}
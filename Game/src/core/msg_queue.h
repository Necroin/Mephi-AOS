#pragma once
#ifndef _MSGQUEUE_H_
#define _MSGQUEUE_H_

#include <string>
#include "logger.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

class MsgQueue {
public:
    inline const static int buffer_size = 200;

    struct Buffer{
        long mtype;
        char mtext[buffer_size];
    };
private:
    long queue_id;
    Buffer buffer;
    Logger logger;
public:
    MsgQueue(std::string key, int key_opt, Logger logger): logger(logger) {
        key_t queue_key = ftok(key.c_str(), key_opt);
        queue_id = msgget(queue_key, IPC_CREAT | 0666);
        msgctl(queue_id, IPC_RMID, 0);
        queue_id = msgget(queue_key, IPC_CREAT | 0666);
    	if (queue_id == -1) {
			throw std::runtime_error("[Server] [Error] game message queue open failed");
    	}
    }

    void send(long type, const char* text, int size) {
        memset(buffer.mtext, '\0', buffer_size);
        buffer.mtype = type;
        memcpy(buffer.mtext, text, size);
        int res = msgsnd(queue_id, &buffer, size, 0);
		if (res == -1) {
            throw std::runtime_error(std::string("message send failed from queue: ") + std::string(strerror(errno)));
        }
    }

    std::pair<long, char[buffer_size]> receive(int type) {
        memset(buffer.mtext, '\0', buffer_size);
        std::pair<long, char[buffer_size]> result;
        int res = msgrcv(queue_id, &buffer, buffer_size, type, 0);
		if (res == -1) {
            throw std::runtime_error(std::string("message receive failed from queue: ") + std::string(strerror(errno)));
        }
        result.first = buffer.mtype;
        memcpy(result.second, buffer.mtext, buffer_size);
        return result;
    }


};

#endif

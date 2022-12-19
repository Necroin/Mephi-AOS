#pragma once
#ifndef _SOCKETWRITER_H_
#define _SOCKETWRITER_H_

#include <vector>
#include <string>
#include "logger.h"
#include "message.h"


class SocketWriter {
private:
    int socket;
    Logger logger;
public:
    SocketWriter(int socket, Logger logger): socket(socket), logger(logger) {}
    void send(Message message) {
        auto bytes = message.to_bytes();
        int error = ::send(socket, bytes.data(), bytes.size(), 0);
        if (error == -1) {
            throw std::runtime_error("[SocketWriter] [" + std::to_string(socket) +"] invalid socket");
        }
        if (error == 0) {
            throw std::runtime_error("[SocketWriter] [" + std::to_string(socket) +"] unexpected socket closing");
        }
        logger.info("[SocketWriter] [", socket, "] Writed ", bytes.size(), " bytes");
        logger.info("[SocketWriter] [", socket, "] message:\n\ttype: ", message.type, "\n\tsize: ", message.size, "\n\ttext: ", message.text);
    }
};
#endif

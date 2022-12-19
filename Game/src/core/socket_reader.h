#pragma once
#ifndef _SOCKETREADER_H_
#define _SOCKETREADER_H_

#include <vector>
#include <string>
#include "logger.h"
#include "message.h"


class SocketReader {
private:
    int socket;
    std::vector<char> buffer;
    std::vector<char> read_buffer;
    Logger logger;
private:
    void read_socket() {
        int packet_size = recv(socket, read_buffer.data(), read_buffer.size() - 1, 0);
        logger.info("[SocketReader] [", socket, "] Received ", packet_size, " bytes");
        if (packet_size == -1) {
            throw std::runtime_error("[SocketReader] [" + std::to_string(socket) +"] invalid socket");
        }
        if (packet_size == 0) {
            throw std::runtime_error("[SocketReader] [" + std::to_string(socket) +"] unexpected socket closing");
        }
        buffer.insert(buffer.end(), read_buffer.begin(), read_buffer.begin() + packet_size);
    }
public:
    SocketReader(int socket, int buffer_size, Logger logger) : socket(socket), read_buffer(buffer_size, 0), logger(logger) {}
    Message receive() {
        while(true) {
            read_socket();
            size_t buffer_size = buffer.size();
            size_t message_begin_size = sizeof(size_t) * 2;
            if (buffer_size >= message_begin_size) {
                size_t type = ((size_t*)(buffer.data()))[0];
                size_t size = ((size_t*)(buffer.data()))[1];
                if ((buffer_size - message_begin_size) >= size) {
                    auto message = Message(type, std::string(buffer.begin() + message_begin_size, buffer.begin() + message_begin_size + size));
                    buffer.erase(buffer.begin(), buffer.begin() + message_begin_size + size);
                    logger.info("[SocketReader] [", socket, "] Readed message:\n\ttype: ", type, "\n\tsize: ", size, "\n\ttext: ", message.text);
                    return message;
                }
            }
        }
    }
};
#endif

#pragma once
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

enum MessageTypes {
    ACCESS_REQUEST = 1,
    ACCESS_RESPONSE = 2,
    REUQUEST = 3,
    RESPONSE = 4,
    CLOSE_REQUEST = 5,
    CLOSE_RESPONSE = 6,
};

class Message {
public:
    size_t type;
    size_t size;
    std::string text;
public:
    Message() {}
    Message(size_t type, std::string text): type(type), size(text.size()), text(text) {}
    std::vector<char> to_bytes() {
        std::vector<char> result;
        union { 
            size_t value;
            char bytes[sizeof(size_t)];
        } bytes_converter;

        bytes_converter.value = type;
        result.insert(result.end(), std::begin(bytes_converter.bytes), std::end(bytes_converter.bytes));

        bytes_converter.value = size;
        result.insert(result.end(), std::begin(bytes_converter.bytes), std::end(bytes_converter.bytes));

        result.insert(result.end(), text.begin(), text.end());

        return result;
    }
};
#endif
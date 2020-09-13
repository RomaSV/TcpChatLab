//
// Created by Roman Svechnikov on 12.09.2020.
//

#ifndef NETLABS_CONNECTIONREQUEST_H
#define NETLABS_CONNECTIONREQUEST_H


#include <cstddef>
#include <string>
#include "PacketHeaders.h"

class ConnectionRequest {

public:
    ConnectionRequest(char *data, size_t length);

    explicit ConnectionRequest(std::string name);

    [[nodiscard]] const std::string &getName() const;

    [[nodiscard]] std::pair<char *, size_t> serialize() const;

private:
    std::string username;
};


#endif //NETLABS_CONNECTIONREQUEST_H

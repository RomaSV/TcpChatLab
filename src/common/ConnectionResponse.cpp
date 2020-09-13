//
// Created by Roman Svechnikov on 12.09.2020.
//

#include <cstdlib>
#include <cstring>
#include "ConnectionResponse.h"

ConnectionResponse::ConnectionResponse(char *data, size_t length) {
    memcpy(&status, data + sizeof(char), sizeof(status));
}

ConnectionResponse::ConnectionResponse(ConnectionStatus status) : status(status) {

}

const ConnectionStatus &ConnectionResponse::getStatus() const {
    return status;
}

std::pair<char *, size_t> ConnectionResponse::serialize() const {
    size_t dataLength = sizeof(char) + sizeof(ConnectionStatus);
    auto *data = static_cast<char *>(malloc(dataLength));
    data[0] = headers::CONNECTION_RESPONSE;
    memcpy(data + sizeof(char), &status, sizeof(status));
    return std::pair<char *, size_t>(data, dataLength);
}

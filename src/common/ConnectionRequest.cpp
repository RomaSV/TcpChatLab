//
// Created by Roman Svechnikov on 12.09.2020.
//

#include "ConnectionRequest.h"


ConnectionRequest::ConnectionRequest(char *data, size_t length) {
    int nameLength;
    memcpy(&nameLength, data + sizeof(char), sizeof(int));
    char nameArray[nameLength + 1];
    memcpy(nameArray, data + sizeof(char) + sizeof(int), (size_t) nameLength);
    nameArray[nameLength] = '\0';
    username = std::string {nameArray};
}

ConnectionRequest::ConnectionRequest(std::string name) : username(std::move(name)) {

}

const std::string &ConnectionRequest::getName() const {
    return username;
}

std::pair<char *, size_t> ConnectionRequest::serialize() const {
    size_t dataLength = sizeof(char) + sizeof(int) + username.size();
    auto *data = static_cast<char *>(malloc(dataLength));
    data[0] = headers::CONNECTION_REQUEST;
    int nameLength = username.size();
    memcpy(data + sizeof(char), &nameLength, sizeof(int));
    memcpy(data + sizeof(char) + sizeof(int), username.data(), username.size());
    return std::pair<char *, size_t>(data, dataLength);
}

//
// Created by Roman Svechnikov on 12.09.2020.
//

#include "ChatMessage.h"

ChatMessage::ChatMessage(char *data) {
    int nameLength;
    memcpy(&nameLength, data + sizeof(char), sizeof(int));
    char nameArray[nameLength + 1];
    memcpy(nameArray, data + sizeof(char) + sizeof(int), (size_t) nameLength);
    nameArray[nameLength] = '\0';
    username = std::string{nameArray};

    int msgLength;
    memcpy(&msgLength, data + sizeof(char) + sizeof(int) + nameLength, sizeof(int));
    char msgArray[msgLength + 1];
    memcpy(msgArray, data + sizeof(char) + sizeof(int) * 2 + nameLength, (size_t) msgLength);
    msgArray[msgLength] = '\0';
    message = std::string{msgArray};
}

ChatMessage::ChatMessage(std::string name, std::string message)
        : username(std::move(name)), message(std::move(message)) {

}

const std::string &ChatMessage::getName() const {
    return username;
}

const std::string &ChatMessage::getMessage() const {
    return message;
}

std::pair<char *, size_t> ChatMessage::serialize() const {
    size_t dataLength = sizeof(char) + sizeof(int) * 2 + username.size() + message.size();
    auto *data = static_cast<char *>(malloc(dataLength));
    data[0] = headers::CHAT_MESSAGE;
    int nameLength = username.size();
    int msgLength = message.size();
    memcpy(data + sizeof(char), &nameLength, sizeof(int));
    memcpy(data + sizeof(char) + sizeof(int), username.data(), nameLength);
    memcpy(data + sizeof(char) + sizeof(int) + nameLength, &msgLength, sizeof(int));
    memcpy(data + sizeof(char) + sizeof(int) * 2 + nameLength, message.data(), msgLength);
    return std::pair<char *, size_t>(data, dataLength);
}

//
// Created by Roman Svechnikov on 10.09.2020.
//

#ifndef NETLABS_LAB1_CLIENT_H
#define NETLABS_LAB1_CLIENT_H

#include <iostream>
#include <cstdio>
#include <string>
#include <winsock2.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include "../common/Payload.h"
#include "../common/PacketHeaders.h"

class Client {

public:
    explicit Client(const std::string &username);

    ~Client();

    void connectTo(const std::string &address, uint16_t port);

    template<typename Packet, typename ...Args>
    void sendMessage(Args &&...args) const {
        auto [data, len] = Packet{std::forward<Args>(args)...}.serialize();
        send(connectionSocket, data, len, 0);
        free(data);
    }

    template<typename Packet>
    void sendMessage() const {
        auto [data, len] = Packet{}.serialize();
        send(connectionSocket, data, len, 0);
        free(data);
    }

    void stop();

    std::function<void (const ChatMessage &)> onMessageReceived = [](const ChatMessage &msg){};

private:
    static constexpr uint16_t bufferSize = 1024;

    char buffer[bufferSize] {};
    SOCKET connectionSocket = INVALID_SOCKET;

    std::atomic_bool shouldExit = false;

    const std::string &username;

    void handshake();

    void loop();

};


#endif //NETLABS_LAB1_CLIENT_H

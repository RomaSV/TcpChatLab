//
// Created by Roman Svechnikov on 10.09.2020.
//

#ifndef NETLABS_LAB1_SERVER_H
#define NETLABS_LAB1_SERVER_H


#include <cstdint>
#include <thread>
#include <string>
#include <iostream>
#include <mutex>
#include <atomic>
#include <winsock2.h>
#include <chrono>
#include <unordered_set>
#include <bits/unordered_map.h>
#include <queue>
#include "../common/Payload.h"

class Server {

public:

    explicit Server(uint16_t port);

    ~Server();

    void start();

    void stop();

private:

    static constexpr uint16_t bufferSize = 1024;

    uint16_t port;

    SOCKET listenSocket = INVALID_SOCKET;

    std::atomic_bool shouldExit = false;

    std::unordered_set<std::string> users = {};
    std::unordered_map<SOCKET, std::thread> handlerThreads = {};
    std::queue<SOCKET> clientsToFree = {};

    void loop();

    void handleClient(SOCKET clientSocket);

    void userInput();

    template<typename Packet, typename ...Args>
    void sendMessage(SOCKET socket, Args &&...args) {
        auto [data, len] = Packet{std::forward<Args>(args)...}.serialize();
        send(socket, data, len, 0);
        free(data);
    }

};


#endif //NETLABS_LAB1_SERVER_H

//
// Created by Roman Svechnikov on 10.09.2020.
//

#include "Client.h"

Client::Client(const std::string &username) : username(username) {
    WSADATA wsaData;
    if (WSAStartup(0x0202, &wsaData) != 0) {
        std::fprintf(stderr, "ERROR! Where: WSAStartup\n");
        exit(1);
    }
}

Client::~Client() {
    WSACleanup();
}

void Client::connectTo(const std::string &address, uint16_t port) {

    sockaddr_in addr{};
    addr.sin_addr.S_un.S_addr = inet_addr(address.c_str());
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    connectionSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (connectionSocket == INVALID_SOCKET) {
        std::fprintf(stderr, "ERROR! Where: socket init\n");
        exit(1);
    }

    if (connect(connectionSocket, (const sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        std::fprintf(stderr, "ERROR! Where: socket connect\n");
        exit(1);
    }
    handshake();
    loop();

    closesocket(connectionSocket);
}

void Client::handshake() {
    auto [data, len] = ConnectionRequest(username).serialize();
    send(connectionSocket, data, len, 0);
    free(data);

    recv(connectionSocket, buffer, bufferSize, 0);
    ConnectionStatus status = ConnectionResponse(buffer, bufferSize).getStatus();
    if (status == ConnectionStatus::BAD_USERNAME) {
        printf("Sorry, but that name is already taken!\n");
        stop();
    } else if (status == ConnectionStatus::SERVER_IS_FULL) {
        printf("Sorry, but the server is full right now!\n");
        stop();
    }
}

void Client::loop() {
    while(true) {
        if (shouldExit) break;
        recv(connectionSocket, buffer, bufferSize, 0);

        switch (buffer[0]) {
            case headers::CHAT_MESSAGE: {
                auto message = ChatMessage(buffer);
                onMessageReceived(message);
                break;
            }
            default:
                break;
        }
        memset(&buffer, 0, bufferSize);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void Client::stop() {
    shouldExit = true;
}
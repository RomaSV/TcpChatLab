//
// Created by Roman Svechnikov on 10.09.2020.
//

#include "Server.h"

Server::Server(uint16_t port) : port(port) {
    WSADATA wsaData;
    if (WSAStartup(0x0202, &wsaData) != 0) {
        std::fprintf(stderr, "ERROR! Where: WSAStartup\n");
        exit(1);
    }
}

Server::~Server() {
    WSACleanup();
}

void Server::start() {
    std::thread inputThread([this] { userInput(); });

    sockaddr_in address{};
    address.sin_addr.S_un.S_addr = INADDR_ANY;
    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET) {
        std::fprintf(stderr, "ERROR! Where: socket init\n");
        inputThread.join();
        stop();
        exit(1);
    }

    u_long mode = 1;
    ioctlsocket(listenSocket, FIONBIO, &mode);

    if (bind(listenSocket, (const sockaddr *) &address, sizeof(address)) == SOCKET_ERROR) {
        std::fprintf(stderr, "ERROR! Where: socket bind\n");
        inputThread.join();
        stop();
        exit(1);
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::fprintf(stderr, "ERROR! Where: socket listen\n");
        inputThread.join();
        stop();
        exit(1);
    }

    loop();

    inputThread.join();
    stop();
}

void Server::loop() {
    while (true) {
        if (shouldExit) break;

        sockaddr_in client_addr;
        SOCKET clientSocket = accept(listenSocket, (sockaddr *) &client_addr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            printf("[INFO] New connection\n");
            handlerThreads[clientSocket] = std::thread([=]{
                handleClient(clientSocket);
                clientsToFree.push(clientSocket);
            });
        }
        while (!clientsToFree.empty()) {
            auto client = clientsToFree.front();
            closesocket(client);
            handlerThreads.at(client).join();
            handlerThreads.erase(client);
            clientsToFree.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Server::stop() {
    printf("Stopping the server...\n");
    closesocket(listenSocket);
}

void Server::userInput() {
    while (true) {
        std::string in;
        std::getline(std::cin, in);

        if (in == "/stop") {
            shouldExit = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Server::handleClient(SOCKET clientSocket) {
    char buffer[bufferSize];
    std::string username;

    while (true) {
        recv(clientSocket, buffer, bufferSize, 0);

        switch (buffer[0]) {
            case headers::CONNECTION_REQUEST: {
                std::string name = ConnectionRequest(buffer, bufferSize).getName();
                ConnectionStatus status;
                if (users.find(name) == users.end()) {
                    users.insert(name);
                    username = name;
                    status = ConnectionStatus::SUCCESS;
                } else {
                    status = ConnectionStatus::BAD_USERNAME;
                }
                sendMessage<ConnectionResponse>(clientSocket, status);
                if (status != ConnectionStatus::SUCCESS) {
                    return;
                } else {
                    sendMessage<ChatMessage>(clientSocket, "Server", "Welcome to the chat! Praise for our dear leader!");
                    for (const auto &[otherClient, thread]: handlerThreads) {
                        if (otherClient != clientSocket) {
                            sendMessage<ChatMessage>(otherClient, "Server", username + " joined the chat. Don't forget to say hello!");
                        }
                    }
                }
                break;
            }
            case headers::DISCONNECTION_REQUEST: {
                printf("[INFO] %s disconnected.\n", username.c_str());
                for (const auto &[otherClient, thread]: handlerThreads) {
                    if (otherClient != clientSocket) {
                        sendMessage<ChatMessage>(otherClient, "Server", username + " leaved the chat. Bye!");
                    }
                }
                return;
            }
            case headers::CHAT_MESSAGE: {
                auto message = ChatMessage(buffer);
                std::printf("<%s> %s\n", message.getName().c_str(), message.getMessage().c_str());
                for (const auto &[otherClient, thread]: handlerThreads) {
                    if (otherClient != clientSocket) {
                        sendMessage<ChatMessage>(otherClient, message.getName(), message.getMessage());
                    }
                }
                break;
            }
            default:
                break;
        }
        memset(&buffer, 0, bufferSize);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


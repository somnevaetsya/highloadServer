#pragma once

#include <utility>
#include <vector>
#include <queue>
#include <mutex>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <condition_variable>


class Server {
public:
    Server() = delete;
    explicit Server(int cpuLim, int threadLim, std::string root)
    :poolSize(cpuLim), serverPort(80), simConnections(threadLim) {
        rootDirectory = std::move(root);
        for(size_t i = 0; i < this->poolSize; i++) {
            this->threadPool.emplace_back([this](){this->handleRequest();});
        }
    };
    bool init();

    [[noreturn]] void run();
private:
    [[noreturn]] void handleRequest();
    std::string rootDirectory;

    std::queue<int> requestQueue;

    int simConnections;

    int serverPort;
    std::vector<std::thread> threadPool;
    size_t poolSize;

    std::mutex queueMutex;
    std::condition_variable cv;

    int serverSocket{};
    int clientSocket{};
    struct sockaddr_in serverAddr{};
    struct sockaddr_in clientAddr{};
};


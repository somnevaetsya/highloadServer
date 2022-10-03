#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "server.h"
#include "handler.h"

bool Server::init() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        return false;
    }

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(this->serverPort);
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->serverSocket, (struct sockaddr *) &this->serverAddr, sizeof(this->serverAddr)) == -1) {
        return false;
    }

    listen(serverSocket, simConnections);

    return true;
}

[[noreturn]] void Server::run() {
    while (true) {
        socklen_t addr_size = sizeof(this->clientAddr);
        this->clientSocket = accept(serverSocket, (struct sockaddr *) &this->clientAddr, &addr_size);

        this->queueMutex.lock();
        this->requestQueue.push(clientSocket);
        this->cv.notify_one();
        this->queueMutex.unlock();
    }
}

[[noreturn]] void Server::handleRequest() {
    std::unique_lock<std::mutex> lock(this->queueMutex, std::defer_lock);
    int client_socket = -1;

    while (true) {
        lock.lock();

        this->cv.wait(lock, [this]() { return !this->requestQueue.empty(); });
        client_socket = this->requestQueue.front();
        this->requestQueue.pop();
        lock.unlock();

        char req[2 * 1024];
        recv(client_socket, req, sizeof(req), 0);
        std::string reply = Handler::handle(req, rootDirectory);

        send(client_socket, reply.c_str(), reply.size(), 0);

        close(client_socket);
    }
}

#include "server.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>

Server::Server(int port, const std::string &directory) : port(port), directory(directory) {}

bool Server::run()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverSocket);
        return false;
    }

    if (listen(serverSocket, 10) == -1)
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        close(serverSocket);
        return false;
    }

    pollfd ps;
    ps.fd = serverSocket;
    ps.events = POLLIN;
    ps.revents = 0;

    pollfd psClient;
    ps.events = POLLIN;
    ps.revents = 0;

    std::cout << "Server listening on port " << port << std::endl;
    int ready;

    while (true)
    {
        ready = poll(&ps, 1, 1000);
        while (ready-- && ps.revents == POLLIN)
        {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1)
            {
                std::cerr << "Failed to accept client." << std::endl;
                continue;
            }

            psClient.fd = clientSocket;
            handleClient(clientSocket);

            long int start_time = getMilliseconds();
            while ((getMilliseconds() - start_time) <= 2)
            {
                ready = poll(&psClient, 1, 10);
                if (ready == -1)
                {
                    std::cerr << "Client poll error" << std::endl;
                    break;
                }

                while (ready-- && psClient.revents == POLLIN)
                {
                    handleClient(clientSocket);
                }
            }
            close(clientSocket);
        }
    }

    close(serverSocket);
    return true;
}

void Server::handleClient(int clientSocket)
{
    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
    {
        std::cerr << "Failed to read from client." << std::endl;
        return;
    }

    buffer[bytesRead] = '\0';
    std::string request(buffer);
    std::string host = getHost(request);

    char methodBuff[16], pathBuff[256], protocolBuff[16];
    if (sscanf(buffer, "%15s %255s %15s", methodBuff, pathBuff, protocolBuff) != 3)
    {
        std::cerr << "Invalid request format." << std::endl;
        std::string response = createErrorResponse(400, "Bad Request");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string method(methodBuff), relativePath(pathBuff), protocol(protocolBuff);

    if (method != "GET")
    {
        std::string response = createErrorResponse(501, "Not Implemented");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    if (startsWith(relativePath, ".."))
    {
        std::cout << relativePath << " " << directory << std::endl;
        std::string response = createErrorResponse(403, "Forbidden");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    if (relativePath == "/")
    {
        std::string response = createMovedPermanently("http://" + host + ":" + std::to_string(port) + "/index.html");
        std::cout << response << std::endl;
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string path = directory + "/" + host + relativePath;
    std::ifstream file(path);
    if (!file)
    {
        std::string response = createErrorResponse(404, "Not Found");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string contentType = getContentType(path);
    std::string response = createSuccessResponse(content, contentType);
    send(clientSocket, response.c_str(), response.length(), 0);
}

std::string Server::getContentType(const std::string &path)
{
    if (endsWith(path, ".txt"))
        return "text/plain; charset=utf-8";
    else if (endsWith(path, ".html"))
        return "text/html; charset=utf-8";
    else if (endsWith(path, ".css"))
        return "text/css";
    else if (endsWith(path, ".jpg"))
        return "image/jpeg";
    else if (endsWith(path, ".jpeg"))
        return "image/jpeg";
    else if (endsWith(path, ".png"))
        return "image/png";
    else if (endsWith(path, ".pdf"))
        return "application/pdf";
    return "application/octet-stream";
}

std::string Server::getHost(const std::string &request)
{
    std::istringstream stream(request);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.substr(0, 5) == "Host:")
        {
            std::string host = line.substr(6);
            size_t colonPos = host.find(':');
            return host.substr(0, colonPos);
        }
    }
    return "";
}
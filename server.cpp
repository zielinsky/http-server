#include "server.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

Server::Server(int port, const std::string &directory) : port(port), directory(directory) {}

bool Server::start()
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

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1)
        {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        }

        handleClient(clientSocket);
        close(clientSocket);
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

    std::istringstream requestStream(request);
    std::string requestLine;
    std::getline(requestStream, requestLine);

    std::string method, path, protocol;
    std::istringstream requestLineStream(requestLine);
    requestLineStream >> method >> path >> protocol;

    if (method != "GET")
    {
        std::string response = createErrorResponse(501, "Not Implemented");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string fullPath = directory + "/" + host + (path == "/" ? "/index.html" : path);
    std::ifstream file(fullPath);
    if (!file)
    {
        std::string response = createErrorResponse(404, "Not Found");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string mimeType = getContentType(fullPath);

    std::string response = createSuccessResponse(fileContent, mimeType);
    send(clientSocket, response.c_str(), response.length(), 0);
}

std::string Server::getContentType(const std::string &path)
{
    if (endsWith(path, ".html"))
        return "text/html; charset=utf-8";
    else if (endsWith(path, ".txt"))
        return "text/plain; charset=utf-8";
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
            if (colonPos != std::string::npos)
            {
                host = host.substr(0, colonPos);
            }
            return host;
        }
    }
    return "";
}
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

void Server::handle(int clientSocket)
{
    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, 4095, 0);
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

    std::ifstream file(directory + "/" + host + relativePath);
    if (!file)
    {
        std::string response = createErrorResponse(404, "Not Found");
        send(clientSocket, response.c_str(), response.length(), 0);
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string contentType = getContentType(relativePath);
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
    else if (endsWith(path, ".jpg") || endsWith(path, ".jpeg"))
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
            return host.substr(0, host.find(':'));
        }
    }
    return "";
}
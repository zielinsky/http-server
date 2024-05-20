#include "utils.h"
#include <sstream>

std::string createErrorResponse(int statusCode, const std::string &statusMessage)
{
    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    response << "Content-Type: text/html; charset=utf-8\r\n";
    response << "Content-Length: " << statusMessage.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << statusMessage;
    return response.str();
}

std::string createSuccessResponse(const std::string &content, const std::string &contentType)
{
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;
    return response.str();
}

std::string createMovedPermanently(const std::string &locationUrl)
{
    std::ostringstream response;
    response << "HTTP/1.1 301 Moved Permanently\r\n";
    response << "Location: " << locationUrl << "\r\n";
    return response.str();
}

bool endsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool startsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
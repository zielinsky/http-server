#include "http_utils.h"
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

std::string createSuccessResponse(const std::string &content, const std::string &mimeType)
{
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << mimeType << "\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;
    return response.str();
}
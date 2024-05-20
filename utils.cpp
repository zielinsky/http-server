// 330261 - Patryk Zieliński

#include "utils.h"
#include <time.h>
#include <sstream>

std::string createErrorResponse(int statusCode, const std::string &statusMessage)
{
    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\n";
    response << "Content-Type: text/html; charset=utf-8\n";
    response << "Content-Length: " << statusMessage.length() << "\n";
    response << "Connection: close\n";
    response << "\n";
    response << statusMessage;
    return response.str();
}

std::string createSuccessResponse(const std::string &content, const std::string &contentType)
{
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\n";
    response << "Content-Type: " << contentType << "\n";
    response << "Content-Length: " << content.length() << "\n";
    response << "Connection: close\n";
    response << "\n";
    response << content;
    return response.str();
}

std::string createMovedPermanently(const std::string &locationUrl)
{
    std::ostringstream response;
    response << "HTTP/1.1 301 Moved Permanently\n";
    response << "Location: " << locationUrl << "\n";
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

long int getMilliseconds()
{
    return time(nullptr);
}
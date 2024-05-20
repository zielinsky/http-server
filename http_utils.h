#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <string>

std::string createErrorResponse(int statusCode, const std::string& statusMessage);
std::string createSuccessResponse(const std::string& content, const std::string& mimeType);

#endif
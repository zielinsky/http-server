#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string createErrorResponse(int statusCode, const std::string &statusMessage);
std::string createSuccessResponse(const std::string &content, const std::string &mimeType);
bool endsWith(const std::string &str, const std::string &suffix);

#endif
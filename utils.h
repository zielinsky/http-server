// 330261 - Patryk Zieliński

#include <string>

std::string createMovedPermanently(const std::string &locationUrl);
std::string createErrorResponse(int statusCode, const std::string &statusMessage);
std::string createSuccessResponse(const std::string &content, const std::string &contentType, bool closeConnection);
bool endsWith(const std::string &str, const std::string &suffix);
bool startsWith(const std::string &str, const std::string &prefix);
long int getMilliseconds();
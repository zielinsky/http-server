#include <string>

std::string createErrorResponse(int statusCode, const std::string &statusMessage);
std::string createSuccessResponse(const std::string &content, const std::string &mimeType);
std::string createMovedPermanently(const std::string &locationUrl);
bool endsWith(const std::string &str, const std::string &suffix);
bool startsWith(const std::string &str, const std::string &prefix);
long int getMilliseconds();
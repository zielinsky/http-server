// 330261 - Patryk Zieliński

#include <string>

class Server
{
public:
    int port;
    std::string directory;
    Server(int port, const std::string &directory);

    bool handle(int clientSocket);
    std::string readFile(const std::string &path);
    std::string getContentType(const std::string &path);
};
#include <string>

class Server
{
public:
    int port;
    std::string directory;
    Server(int port, const std::string &directory);

    void handle(int clientSocket);
    std::string readFile(const std::string &path);
    std::string getHost(const std::string &request);
    std::string getContentType(const std::string &path);
};
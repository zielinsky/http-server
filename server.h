#include <string>

class Server
{
public:
    Server(int port, const std::string &directory);
    bool start();

private:
    int port;
    std::string directory;

    void handleClient(int clientSocket);
    std::string readFile(const std::string &path);
    std::string getHost(const std::string &request);
    std::string getContentType(const std::string &path);
};
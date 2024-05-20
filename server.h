#ifndef SERVER_H
#define SERVER_H

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
    std::string getContentType(const std::string &path);
    std::string readFile(const std::string &path);
    std::string getHost(const std::string& request);
};

#endif
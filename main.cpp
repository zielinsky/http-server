// 330261 - Patryk Zieliński
#include <iostream>
#include <string>
#include "server.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <directory>" << std::endl;
        return EXIT_SUCCESS;
    }

    int port = std::stoi(argv[1]);
    std::string directory = argv[2];

    Server server(port, directory);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Create server socket error" << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind server socket error" << std::endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 10) == -1)
    {
        std::cerr << "Listen server socket error" << std::endl;
        close(serverSocket);
        return EXIT_FAILURE;
    }

    pollfd ps;
    ps.fd = serverSocket;
    ps.events = POLLIN;
    ps.revents = 0;

    pollfd psClient;
    psClient.events = POLLIN;
    psClient.revents = 0;

    int ready;

    while (true)
    {
        ready = poll(&ps, 1, 1000);
        if (ready && ps.revents == POLLIN)
        {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == -1)
            {
                std::cerr << "Client accept error" << std::endl;
                continue;
            }

            psClient.fd = clientSocket;
            server.handle(clientSocket);

            long int start_time = getMilliseconds();
            while ((getMilliseconds() - start_time) <= 2)
            {
                ready = poll(&psClient, 1, 10);
                if (ready == -1)
                {
                    std::cerr << "Client poll error" << std::endl;
                    break;
                }

                if (ready == 1 && psClient.revents == POLLIN)
                {
                    server.handle(clientSocket);
                }
                std::cout << (getMilliseconds() - start_time) << " " << ready << std::endl;
            }
            
            std::string response = createSuccessResponse("", "", "true");
            send(clientSocket, response.c_str(), response.length(), 0);
            close(clientSocket);
            psClient.revents = 0;
        }
    }

    close(serverSocket);
    return EXIT_SUCCESS;
}
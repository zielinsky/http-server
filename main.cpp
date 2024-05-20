#include <iostream>
#include <string>
#include "server.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <directory>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::string directory = argv[2];

    Server server(port, directory);
    if (!server.start()) {
        std::cerr << "Failed to start server." << std::endl;
        return 1;
    }

    return 0;
}
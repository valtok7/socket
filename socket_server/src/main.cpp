#include "SocketServer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number. Port must be between 1 and 65535." << std::endl;
        return 1;
    }

    SocketServer server(port);
    server.Start();

    return 0;
}

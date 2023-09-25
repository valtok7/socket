#include "SocketClient.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <file_to_send>" << std::endl;
        return 1;
    }

    const std::string serverIP = argv[1];
    int serverPort = std::atoi(argv[2]);
    const std::string filePath = argv[3];

    SocketClient client(serverIP, serverPort);

    if (client.Connect() && client.SendFile(filePath)) {
        std::cout << "File sent successfully." << std::endl;

        if (client.ReceiveFile("received_file.bin")) {
            std::cout << "File received and saved as 'received_file.bin'." << std::endl;
        }
    }

    return 0;
}

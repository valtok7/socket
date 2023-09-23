#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

class SocketClient {
public:
    SocketClient(const std::string& serverIP, int serverPort)
        : serverIP_(serverIP), serverPort_(serverPort), clientSocket_(-1) {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock" << std::endl;
            exit(1);
        }
#endif
    }

    ~SocketClient() {
        CloseClientSocket();
#ifdef _WIN32
        WSACleanup();
#endif
    }

    bool Connect() {
        clientSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket_ == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort_);
        inet_pton(AF_INET, serverIP_.c_str(), &serverAddress.sin_addr);

        if (connect(clientSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to connect to the server" << std::endl;
            CloseClientSocket();
            return false;
        }

        std::cout << "Connected to the server" << std::endl;
        return true;
    }

    bool SendFile(const std::string& filePath) {
        std::ifstream inputFile(filePath, std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open the input file for reading" << std::endl;
            return false;
        }
        std::filesystem::path path(filePath); // ファイルのパスを指定してください
        std::uintmax_t fileSize = std::filesystem::file_size(filePath);
        std::cout << "File size: " << fileSize << " bytes" << std::endl;

        // ファイルサイズを送信する
        std::uint32_t fileSize32 = static_cast<std::uint32_t>(fileSize);
        send(clientSocket_, reinterpret_cast<char*>(&fileSize32), sizeof(fileSize32), 0);

        char buffer[1024];
        int bytesRead;
        while (!inputFile.eof()) {
            inputFile.read(buffer, sizeof(buffer));
            bytesRead = inputFile.gcount();
            if (bytesRead > 0) {
                send(clientSocket_, buffer, bytesRead, 0);
            }
        }

        inputFile.close();
        return true;
    }

    bool ReceiveFile(const std::string& savePath) {
        std::ofstream outputFile(savePath, std::ios::binary);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open the output file for writing" << std::endl;
            return false;
        }

        // これから受信するデータサイズを受け取る
        int bytesRead;
        char buffer[1024];
        bytesRead = recv(clientSocket_, buffer, sizeof(buffer), 0);
        std::uint32_t fileSize = *reinterpret_cast<std::uint32_t*>(buffer);
        if(bytesRead<= 0){
            std::cerr << "An unexpected disconnection has occurred.(1)" << std::endl;
            outputFile.close();
            return false;
        }
        std::cerr << "The expected receiving size is " << fileSize << " bytes." << std::endl;

        // 受信したデータをファイルに保存する
        std::uint32_t totalBytesRead = 0;
        while ((bytesRead = recv(clientSocket_, buffer, sizeof(buffer), 0)) > 0) {
            totalBytesRead += bytesRead;
            outputFile.write(buffer, bytesRead);
            if(totalBytesRead == fileSize){
                std::cerr << "The received size is " << fileSize << " bytes." << std::endl;
                break;
            } else if(totalBytesRead > fileSize){
                std::cerr << "The received size exceeds " << fileSize << " bytes." << std::endl;
                outputFile.close();
                return false;
            } else if(bytesRead <= 0){
                std::cerr << "An unexpected disconnection has occurred.(2)" << std::endl;
                outputFile.close();
                return false;
            }
        }

        outputFile.close();
        return true;
    }

    void CloseClientSocket() {
        if (clientSocket_ != -1) {
#ifdef _WIN32
            closesocket(clientSocket_);
#else
            close(clientSocket_);
#endif
        }
    }

private:
    std::string serverIP_;
    int serverPort_;
    int clientSocket_;
};

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

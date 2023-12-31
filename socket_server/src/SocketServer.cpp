#include "SocketServer.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

#ifdef _WIN32
using SOCKLEN = int;
#else
using SOCKLEN = socklen_t;
#endif

// コンストラクタ
SocketServer::SocketServer(int port) : port_(port), serverSocket_(-1) {
#ifdef _WIN32
    // Windows用の初期化処理
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        exit(1);
    }
#endif
}

// デストラクタ
SocketServer::~SocketServer() {
    // サーバーソケットを閉じる
    CloseServerSocket();
#ifdef _WIN32
    // Windows用のクリーンアップ処理
    WSACleanup();
#endif
}

// サーバーの起動とクライアントの受け入れを開始するメソッド
void SocketServer::Start() {
    if (CreateServerSocket() && BindServerSocket() && ListenForClients()) {
        std::cout << "Server is listening on port " << port_ << "..." << std::endl;
        AcceptClients();
    }
}

// サーバーソケットを作成するメソッド
bool SocketServer::CreateServerSocket() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    return true;
}

// サーバーソケットを指定のポートにバインドするメソッド
bool SocketServer::BindServerSocket() {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    return true;
}

// クライアントの接続を待ち受けるメソッド
bool SocketServer::ListenForClients() {
    if (listen(serverSocket_, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }
    return true;
}

// クライアントの接続を受け入れるメソッド
void SocketServer::AcceptClients() {
    while (true) {
        sockaddr_in clientAddress;
        SOCKLEN clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::cout << "Client connected" << std::endl;

        // これから受信するデータサイズを取得する
        int bytesRead;
        char buffer[1024];
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::uint32_t dataSize = *reinterpret_cast<std::uint32_t*>(buffer);
        if(bytesRead <= 0)
        {
            std::cerr << "An unexpected disconnection has occurred.(1)" << std::endl;
#ifdef _WIN32
            closesocket(clientSocket);
#else
            close(clientSocket);
#endif
            return;
        }
        std::cerr << "The expected receiving size is " << dataSize << " bytes." << std::endl;

        // データを受信してバッファに蓄積
        std::vector<char> dataBuffer;
        std::uint32_t totalBytesRead = 0;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            totalBytesRead += bytesRead;
            for (int i = 0; i < bytesRead; ++i) {
                dataBuffer.push_back(buffer[i]);
            }
            if(totalBytesRead == dataSize){
                std::cerr << "The received size is " << dataSize << " bytes." << std::endl;
                break;
            } else if(totalBytesRead > dataSize){
                std::cerr << "The received size exceeds " << dataSize << " bytes." << std::endl;
#ifdef _WIN32
                closesocket(clientSocket);
#else
                close(clientSocket);
#endif
                return;
            } else if(bytesRead <= 0){
                std::cerr << "An unexpected disconnection has occurred.(2)" << std::endl;
#ifdef _WIN32
                closesocket(clientSocket);
#else
                close(clientSocket);
#endif
                return;
            }
        }

        // データサイズを送信する
        std::uint32_t dataSize32 = static_cast<std::uint32_t>(dataBuffer.size());
        send(clientSocket, reinterpret_cast<char*>(&dataSize32), sizeof(dataSize32), 0);

        // 受信したデータをそのままクライアントに返す
        int bytesSent = send(clientSocket, dataBuffer.data(), dataBuffer.size(), 0);
        if (bytesSent == -1) {
            std::cerr << "Failed to send data to client" << std::endl;
        } else {
            std::cout << "Sent " << bytesSent << " bytes of data to client" << std::endl;
        }

        // クライアントソケットを閉じる
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif

        std::cout << "Received and sent " << dataBuffer.size() << " bytes of data" << std::endl;
    }
}

// サーバーソケットを閉じるメソッド
void SocketServer::CloseServerSocket() {
    if (serverSocket_ != -1) {
#ifdef _WIN32
        closesocket(serverSocket_);
#else
        close(serverSocket_);
#endif
    }
}

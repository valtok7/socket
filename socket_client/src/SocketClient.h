#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

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
    // コンストラクタ
    SocketClient(const std::string& serverIP, int serverPort);
    
    // デストラクタ
    ~SocketClient();

    // サーバーへの接続を確立するメソッド
    bool Connect();

    // ファイルをサーバーに送信するメソッド
    bool SendFile(const std::string& filePath);

    // ファイルをサーバーから受信するメソッド
    bool ReceiveFile(const std::string& savePath);

    // クライアントソケットを閉じるメソッド
    void CloseClientSocket();

private:
    // サーバーのIPアドレス
    std::string serverIP_;

    // サーバーポート番号
    int serverPort_;

    // クライアントソケットのファイルディスクリプタ
    int clientSocket_;
};

#endif // SOCKETCLIENT_H

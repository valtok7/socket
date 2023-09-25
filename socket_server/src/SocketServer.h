#ifndef SOCK_SERVER_H
#define SOCK_SERVER_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

class SocketServer {
public:
    // コンストラクタ
    SocketServer(int port);

    // デストラクタ
    ~SocketServer();

    // サーバーの起動とクライアントの受け入れを開始するメソッド
    void Start();

private:
    // ポート番号
    int port_;

    // サーバーソケット
    int serverSocket_;

    // サーバーソケットを作成するメソッド
    bool CreateServerSocket();

    // サーバーソケットを指定のポートにバインドするメソッド
    bool BindServerSocket();

    // クライアントの接続を待ち受けるメソッド
    bool ListenForClients();

    // クライアントの接続を受け入れるメソッド
    void AcceptClients();

    // サーバーソケットを閉じるメソッド
    void CloseServerSocket();
};

#endif // SOCK_SERVER_H

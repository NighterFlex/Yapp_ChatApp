#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class Server {
protected:
    int port;
    SOCKET listenSocket;
    vector<SOCKET> clients;

public:
    Server(int port) : port(port), listenSocket(INVALID_SOCKET) {
        if (!Initialize()) {
            cerr << "Winsock initialization failed" << endl;
            exit(1);
        }
    }

    ~Server() {
        Cleanup();
    }

    void Start() {
        CreateSocket();
        BindSocket();
        ListenOnSocket();
        AcceptClients();
    }

    bool Initialize() {
        WSADATA data;
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }

    void Cleanup() {
        if (listenSocket != INVALID_SOCKET) {
            closesocket(listenSocket);
        }
        WSACleanup();
    }

    void CreateSocket() {
        listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket == INVALID_SOCKET) {
            cerr << "Socket creation failed" << endl;
            Cleanup();
            exit(1);
        }
    }

    void BindSocket() {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        if (InetPton(AF_INET, _T("0.0.0.0"), &serverAddr.sin_addr) != 1) {
            cerr << "Setting address structure failed" << endl;
            Cleanup();
            exit(1);
        }

        if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Bind failed" << endl;
            Cleanup();
            exit(1);
        }
    }

    void ListenOnSocket() {
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            cerr << "Listen failed" << endl;
            Cleanup();
            exit(1);
        }
        cout << "Server has started listening on port: " << port << endl;
    }

    void AcceptClients() {
        while (true) {
            SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Invalid client socket" << endl;
                continue;
            }
            clients.push_back(clientSocket);
            thread t(&Server::InteractWithClient, this, clientSocket);
            t.detach();
        }
    }

    void InteractWithClient(SOCKET clientSocket) {
        cout << "Client connected" << endl;
        char buffer[4096];

        while (true) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                cout << "Client disconnected" << endl;
                break;
            }

            string message(buffer, bytesReceived);
            cout << "Message from client: " << message << endl;

            for (auto client : clients) {
                if (client != clientSocket) {
                    send(client, message.c_str(), message.length(), 0);
                }
            }
        }

        RemoveClient(clientSocket);
    }

    void RemoveClient(SOCKET clientSocket) {
        auto it = find(clients.begin(), clients.end(), clientSocket);
        if (it != clients.end()) {
            clients.erase(it);
        }
        closesocket(clientSocket);
    }
};

int main() {
    Server server(55555);
    server.Start();
    return 0;
}

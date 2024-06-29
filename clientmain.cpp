#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class Client {
protected:
    SOCKET clientSocket;
    string serverAddress;
    int port;

public:
    Client(const string& address, int port) : serverAddress(address), port(port), clientSocket(INVALID_SOCKET) {
        if (!Initialize()) {
            cerr << "Winsock initialization failed" << endl;
            exit(1);
        }
    }

    ~Client() {
        Cleanup();
    }

    bool Initialize() {
        WSADATA data;
        return WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }

    void Cleanup() {
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
        }
        WSACleanup();
    }

    void Connect() {
        CreateSocket();
        ConnectToServer();
    }

    void StartCommunication() {
        thread senderThread(&Client::SendMsg, this);
        thread receiverThread(&Client::ReceiveMsg, this);
        senderThread.join();
        receiverThread.join();
    }

    void CreateSocket() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Invalid Socket created" << endl;
            Cleanup();
            exit(1);
        }
    }

    void ConnectToServer() {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, serverAddress.c_str(), &(serverAddr.sin_addr));

        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Not able to connect to server" << endl;
            cerr << " : " << WSAGetLastError() << endl;
            Cleanup();
            exit(1);
        }
        cout << "Successfully connected to the server" << endl;
    }

    void SendMsg() {
        cout << "Enter Username: ";
        string name;
        getline(cin, name);

        cout << "Enter Password: ";
        string password;
        getline(cin, password);

        string message;
        while (true) {
            getline(cin, message);
            string msg = name + " : " + message;
            int byteSent = send(clientSocket, msg.c_str(), msg.length(), 0);
            if (byteSent == SOCKET_ERROR) {
                cerr << "Error sending message" << endl;
                break;
            }

            if (message == "quit") {
                cout << "Stopping the application" << endl;
                break;
            }
        }
        closesocket(clientSocket);
        WSACleanup();
    }

    void ReceiveMsg() {
        char buffer[4096];
        int recvLength;
        string msg;
        while (true) {
            recvLength = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (recvLength <= 0) {
                cerr << "Disconnected from the server" << endl;
                break;
            }
            else {
                msg = string(buffer, recvLength);
                cout << msg << endl;
            }
        }
        closesocket(clientSocket);
        WSACleanup();
    }
};

int main() {
    Client client("127.0.0.1", 55555);
    client.Connect();
    client.StartCommunication();
    return 0;
}

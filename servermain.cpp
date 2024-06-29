#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
/*	
	
	//Initialize winsock library +finalize

	//create socket

	//get ip and port

	//bind ip and port with socket

	//listen on socket

	//accept (blocking call)

	//recv and send

	//close socket

	//cleanup winsock library

*/

bool Initialize() {

	WSADATA data;
	return WSAStartup(MAKEWORD(2,2), &data) == 0;
}


void InteractWithClient(SOCKET clientsocket, vector<SOCKET>& clients) {

	//send and recv
	
	cout << "Client connected" << endl;
	char buffer[4096];

	while (true) {
		int bytesrecvd = recv(clientsocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0) {
			cout << "\nClient disconnected" << endl;
			break;
		}

		string message(buffer, bytesrecvd);
		cout << "\nMessage from client : " << message;

		for (auto client : clients) {
			if (client != clientsocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}

	}
	// To remove the disconnected clients from the vector so it doesnt have it stored
	auto it = find(clients.begin(), clients.end(), clientsocket);
	if (it != clients.end()) {
		clients.erase(it);
	}
	closesocket(clientsocket);
}



int main() {

	if (!Initialize()) {
		cout << "Winsock initialization failed " << endl;
		return 1;
	}

	cout << "Server Program " << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {

		cout << "Socket creation failed " << endl;
		return 1;
	}

	//create address structure

	int port = 55555;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port); //api called host to networks

	//convert ip add (0.0.0.0) put it inside the sin_family in binary format
	
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "Setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {

		cout << "Bind failed " << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {

		cout << "Listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server has started listening on port : " << port << endl;
	vector<SOCKET> clients;
	
		while (true) {
			//accept

			SOCKET clientsocket = accept(listenSocket, nullptr, nullptr); 
			if (clientsocket == INVALID_SOCKET) { 
				cout << "Invalid client socket" << endl;
		}
			clients.push_back(clientsocket);
			thread t1(InteractWithClient, clientsocket, std::ref(clients));
			t1.detach();
	}

	closesocket(listenSocket);

	WSACleanup();
	return 0;
}
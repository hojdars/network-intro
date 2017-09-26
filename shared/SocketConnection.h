#pragma once
#include <windows.h>
#include <iostream>
#include <string>

#pragma comment(lib,"ws2_32.lib")

class SocketConnection
{
public:
	/// Return SOCKET object
	// Required for FD macros
	SOCKET& getSocket() {
		return mSocket;
	}

	/// Close the socket and reset the member to 0
	void close() {
		closesocket(mSocket);
		mSocket = 0;
	}

	/// Return IP adress of this socket
	std::string getIpAddr() const {
		return inet_ntoa(mSockAdr.sin_addr);
	}

	/// Initialize an empty socket
	SocketConnection() : mSocket(0) {};

	/// Initialize a fully functioning socket
	SocketConnection(int af, int type, int protocol, std::string ipAdress, int port) {
		mSocket = socket(af, type, protocol);
		if (mSocket == INVALID_SOCKET) {
			throw std::exception("Socket initialization failed.");
		}

		// Set socket settings - IP adress and port
		mSockAdr.sin_family = AF_INET;
		mSockAdr.sin_addr.s_addr = inet_addr(ipAdress.c_str());
		mSockAdr.sin_port = htons(port);
	}

	~SocketConnection() {
		closesocket(mSocket);
	}

	/// Connect to the adress given during initialization
	int socketConnect() {
		int ss = connect(mSocket, (struct sockaddr *)&mSockAdr, sizeof(mSockAdr));
		if (ss != 0) {
			return -1;
		}
		printf("Connection successful.\n");
		return 0;
	}

	/// Send buffer over the socket
	int socketSend(char* buffer, int len) {
		int slen = send(mSocket, buffer, len, 0);
		if (slen < 0) {
			printf("[ERROR] Cannot send data!\n");
			return -1;
		}
		return slen;
	}

	/// Receive information into buffer
	int socketReceive(char* buffer, int len) {
		int slen = recv(mSocket, buffer, len, 0);
		if (slen < 0) {
			printf("[ERROR] Cannot send data!\n");
			return -1;
		}
		return slen;
	}

	/// Bind the socket
	int socketBind() {
		int err = bind(mSocket, (LPSOCKADDR)&mSockAdr, sizeof(mSockAdr));
		if (err != 0) {
			printf("[ERROR] Socket binding failed.\n");
			return -1;
		}
		return err;
	}

	/// Listen on this socket
	int socketListen() {
		int err = listen(mSocket, 2);
		if (err == SOCKET_ERROR) {
			printf("[ERROR] Socket listening failed.\n");
			return -1;
		}
		return err;
	}

	/// Accept an incoming connection and transform it into 'targetSocket'
	int socketAccept(SocketConnection& targetSocket) {
		int so2len = sizeof(targetSocket.mSockAdr);
		targetSocket.mSocket = accept(mSocket, (sockaddr *)&targetSocket.mSockAdr, &so2len);

		if (targetSocket.mSocket == INVALID_SOCKET) {
			printf("[ERROR] Could not accept, invalid socket.\n");
			return -1;
		}
		return 0;
	}

private:
	SOCKET mSocket; /// The socket object itself
	SOCKADDR_IN mSockAdr; /// Information about the connection (where it leads, its type, etc)
};
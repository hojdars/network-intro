/// http://www.dreamincode.net/forums/topic/137293-networking-tutorial/
/// Packet sending implemented in later post

#include <windows.h>
#include <stdio.h>
#include "../shared/SocketConnection.h"

#pragma comment(lib,"ws2_32.lib")

#define MAX_CLIENTS 4

int StartServer(int port) {
	printf("Server starting up.\n");
	WSADATA Data;
	WSAStartup(MAKEWORD(2, 2), &Data);

	int clients = 0;
	SocketConnection clientSockets[MAX_CLIENTS];
	SocketConnection masterSocket(AF_INET, SOCK_STREAM, 0, "127.0.0.1", port);

	masterSocket.socketBind();
	masterSocket.socketListen();

	printf("Active and waiting for clients.\n");
	bool terminate = false;
	fd_set waitingSockets;
	while (!terminate) {
		// Reset the waiting list, fill it with connected sockets
		FD_ZERO(&waitingSockets);
		FD_SET(masterSocket.getSocket(), &waitingSockets);
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (clientSockets[i].getSocket() > 0)
				FD_SET(clientSockets[i].getSocket(), &waitingSockets);
		}

		// Check which sockets need our attention
		int numAccepts = select(0, &waitingSockets, nullptr, nullptr, NULL);

		// If the master socket needs attention, we have a new connection
		if (FD_ISSET(masterSocket.getSocket(), &waitingSockets) && clients < MAX_CLIENTS) {
			if (masterSocket.socketAccept(clientSockets[clients]) >= 0) {
				printf("Client number %i has joined the server (IP: %s).\n", clients, clientSockets[clients].getIpAddr().c_str());
				clients++;
			}
		}
		// Otherwise there is some traffic coming from the already connected clients
		else {
			for (int client = 0; client < MAX_CLIENTS; ++client) {
				if (FD_ISSET(clientSockets[client].getSocket(), &waitingSockets)) {
					char message[1025];
					int msgLen = clientSockets[client].socketReceive(message, 1024);

					if (msgLen <= 0) { // If the message is 0 zero lenght, it is a DC.
						printf("Client %i disconnected.\n", client);
						clientSockets[client].close();
						clients--;
					}
					else {
						message[msgLen] = '\0';
						printf("Client %i: %s\n", client, message);
					}
				}
			}
		}
	}

	WSACleanup();
	return 0;
}

int main() {
	StartServer(13337);
	return 0;
}
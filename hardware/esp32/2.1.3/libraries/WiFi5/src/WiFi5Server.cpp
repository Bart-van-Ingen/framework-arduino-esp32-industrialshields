#include "WiFi5Server.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi5Server::begin(uint16_t port) {
	this->port = port;
	begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void WiFi5Server::begin() {
	if (socket >= 0) {
		// Already connected
		return;
	}

	int availableSocket = ISM43340.findAvailableSocket();
	if (availableSocket < 0) {
		// Free socket not found
		return;
	}

	if (!ISM43340.startServer(availableSocket, port)) {
		// Impossible to start server
		return;
	}

	socket = availableSocket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
WiFi5Client WiFi5Server::available() {
	if (socket < 0) {
		return WiFi5Client();
	}

	return WiFi5Client(socket);
}

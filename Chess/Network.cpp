#include "stdafx.h"
#include "Network.h"

sf::Packet & operator <<(sf::Packet & packet, const userInputs & data) {
	packet << data.newClick << data.clickIndex;
	return packet;
}

sf::Packet & operator >>(sf::Packet & packet, userInputs & data) {
	packet >> data.newClick >> data.clickIndex;
	return packet;
}

Server::Server() {
	listener.listen(port);
	selector.add(listener);
}

void Server::startServer() {
	if (selector.wait(sf::milliseconds(10))) {
		//A ready listener means there is a new client attempting to connect
		if (selector.isReady(listener)) {
			//Add the new client if it successfully connects
			if (listener.accept(client) == sf::Socket::Done) {
				selector.add(client);
			}
		}
		// The listener socket is not ready (no new connections), so check the other sockets (the clients)
		else {
			if (selector.isReady(client)) {
				//The selector sees data available from the client
				sf::Packet packet;
				if (client.receive(packet) == sf::Socket::Done) {
					//Packet received, process it
					std::string start;
					if (packet >> start) {
						//If the packet received is the start game message, set the waiting bool to false
						if (start == "start") {
							waitingForStart = false;
						}
					}
				}
			}
		}
	}
}

void Server::collectPackets() {
	if (selector.wait(sf::microseconds(10))) {
		if (selector.isReady(listener)) {
			//This section could be used to reconnect a player
		}
		//No player to reconnect, check the clients
		else {
			if (selector.isReady(client)) {
				//Receive the packet
				sf::Packet packet;
				if (client.receive(packet) == sf::Socket::Done) {
					//Packet received, set client's input data
					userInputs data;
					if (packet >> data) {
						playerInputs[1] = data;
					}
				}
			}
		}
	}
}

void Server::clearInputs() {
	playerInputs[0].newClick = false;
	playerInputs[1].newClick = false;
}

void Server::distributePackets() {
	for (int playerID = 0; playerID < 2; playerID++) {
		sf::Packet packet;
		packet << playerID << playerInputs[playerID];
		if (selector.isReady(client)) {
			if (client.send(packet) != sf::Socket::Done) {
				//Client not ready for packets. Did the client disconnect?
			}
		}
	}
}

void Client::connectToHost() {
	if (socket.connect(serverIP, port) != sf::Socket::Done) {
		//Attempting to connect
	}
	else {
		//Successfully connected
		selector.add(socket);
		sf::Packet packet;
		waitingForStart = false;
		packet << "start";
		if (socket.send(packet) != sf::Socket::Done) {
			//Packet failed to send
		}
	}
}

void Client::clearInputs() {
	clientInputs.newClick = false;
	playerInputs[0].newClick = false;
	playerInputs[1].newClick = false;
}

void Client::sendPlayerPacket() {
	sf::Packet packet;
	packet << clientInputs;
	if (socket.send(packet) != sf::Socket::Done) {
		//Packet failed to send
	}
}

void Client::receivePackets() {
	sf::Packet packet;
	int playerID = 0;
	userInputs data;
	for (int i = 0; i < 2; i++) {
		if (selector.wait(sf::microseconds(10))) {
			if (socket.receive(packet) != sf::Socket::Done) {
				//Either missed packet or player does not exist
			}
			else {
				//Packet received, process it
				if (packet >> playerID >> data) {
					//Confirm that the packet belongs to the correct player in the inputs array
					if (playerID == i) {
						playerInputs[i] = data;
					}
				}
			}
		}
	}
}
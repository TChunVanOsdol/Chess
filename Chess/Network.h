#pragma once
#include <SFML/Network.hpp>

//Establish the port you want to use here. Any value is good, as long as it doesn't conflict with another program!
unsigned const short port = 60000;

//Keep user inputs together using a struct
struct userInputs {
	bool newClick;
	int clickIndex;
};

//The packet needs its insertion and extraction operators overloaded to make using them easier.
sf::Packet & operator <<(sf::Packet & packet, const userInputs & data);
sf::Packet & operator >>(sf::Packet & packet, userInputs & data);

class Server {
public:
	sf::TcpListener listener;
	sf::TcpSocket client;
	sf::SocketSelector selector;

	bool waitingForStart = true;

	userInputs playerInputs[2];

	Server();
	void startServer();
	void collectPackets();
	void clearInputs();
	void distributePackets();
};

class Client {
public:
	sf::IpAddress serverIP;
	sf::SocketSelector selector;
	sf::TcpSocket socket;

	bool waitingForStart = true;

	userInputs clientInputs;
	userInputs playerInputs[2];

	Client() {}
	void connectToHost();
	void clearInputs();
	void sendPlayerPacket();
	void receivePackets();
};
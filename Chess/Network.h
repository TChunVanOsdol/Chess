#pragma once
#include <SFML/Network.hpp>

unsigned const short port = 60000;

struct userInputs {
	bool newClick;
	int clickIndex;
};

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
	void serverInputs(userInputs data);
	void distributePackets();
};

class Client {
public:
	sf::IpAddress serverIP;
	sf::SocketSelector selector;
	sf::TcpSocket socket;

	bool waitingForStart = true;

	userInputs playerInputs[2];

	Client() {}
	void connectToHost();
	void clientInput(userInputs data);
	void sendPlayerPacket();
	void receivePackets();
};
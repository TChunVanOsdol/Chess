// Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Unit.h"
#include "Game.h"
#include "Network.h"
#include <iostream>

const int windowW = 800;
const int windowH = 800;
const int tilesPerRow = 8;
const int tilesPerCol = 8;

int main()
{
	//Establish server and client
	char serverStatus;
	bool gameSetup = true;
	Server hostServer;
	Client client;
	//Establish who is the server and who is the client
	while (gameSetup) {
		std::cout << "Are you the server (s) or the client (c)?: ";
		std::cin >> serverStatus;
		if (serverStatus == 's') {
			std::cout << "Starting server on port: " << port << std::endl;
			std::cout << "Server IP address: \"" << sf::IpAddress::getLocalAddress() << "\"" << std::endl;
			std::cout << "Waiting for an opponent..." << std::endl;
			gameSetup = false;
		}
		else if (serverStatus == 'c') {
			std::cout << "Enter the server's IP address: ";
			std::cin >> client.serverIP;
			gameSetup = false;
		}
		else {
			std::cout << "Try again" << std::endl;
		}
	}
	//Attempt to connect. While loop breaks when the server/client connects
	while (hostServer.waitingForStart && client.waitingForStart) {
		if (serverStatus == 's') {
			hostServer.startServer();
		}
		else {
			client.connectToHost();
		}
	}
	
	//Start original chess game
	sf::RenderWindow window(sf::VideoMode(windowW, windowH), "Chess");

	//Objects for gameplay
	Board GameBoard(window, windowW, windowH, tilesPerRow, tilesPerCol);
	Game GameHandler;
	bool declareWinner = false;
	Pawn nullPawn;
	Pawn* promotingPawn = &nullPawn;
	int promotedCount = 0;

	//Vectors of units
	std::vector<Unit*> unitsPtr;
	unitsPtr.reserve(GameBoard.tileCount);
	std::vector<Pawn*> pawnsPtr;
	std::vector<Pawn> pawns;
	std::vector<Rook> rooks;
	std::vector<Knight> knights;
	std::vector<Bishop> bishops;
	std::vector<Queen> queens;
	std::vector<King> kings;
	std::vector<King*> kingsPtr; 
	for (int i = 0; i < 8; i++) {
		pawns.push_back(Pawn(GameHandler, GameBoard, i + 8, black));
		pawns.push_back(Pawn(GameHandler, GameBoard, GameBoard.tileCount - 9 - i, white));
	}
	for (int i = 0; i < pawns.size(); i++) {
		pawnsPtr.push_back(&pawns[i]);
		unitsPtr.push_back(&pawns[i]);
	}
	for (int i = 0; i < 2; i++) {
		rooks.push_back(Rook(GameHandler, GameBoard, i * 7, black));
		rooks.push_back(Rook(GameHandler, GameBoard, GameBoard.tileCount - 1 - i * 7, white));
		knights.push_back(Knight(GameHandler, GameBoard, i * 5 + 1, black));
		knights.push_back(Knight(GameHandler, GameBoard, GameBoard.tileCount - 2 - i * 5, white));
		bishops.push_back(Bishop(GameHandler, GameBoard, i * 3 + 2, black));
		bishops.push_back(Bishop(GameHandler, GameBoard, GameBoard.tileCount - 3 - i * 3, white));
	}
	for (int i = 0; i < rooks.size(); i++) unitsPtr.push_back(&rooks[i]);
	for (int i = 0; i < knights.size(); i++) unitsPtr.push_back(&knights[i]);
	for (int i = 0; i < bishops.size(); i++) unitsPtr.push_back(&bishops[i]);
	queens.push_back(Queen(GameHandler, GameBoard, 3, black));
	queens.push_back(Queen(GameHandler, GameBoard, GameBoard.tileCount - 5, white));
	for (int i = 0; i < queens.size(); i++) unitsPtr.push_back(&queens[i]);
	kings.push_back(King(GameHandler, GameBoard, 4, black));
	kings.push_back(King(GameHandler, GameBoard, GameBoard.tileCount - 4, white));
	for (int i = 0; i < kings.size(); i++) {
		kingsPtr.push_back(&kings[i]);
		unitsPtr.push_back(&kings[i]);
	}

	while (window.isOpen())
	{
		//Reset game loop values
		window.clear();
		GameBoard.newClick = false;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.key.code == sf::Mouse::Left) {
					//Update corresponding userInputs array for server/client
					if (serverStatus = 's') {
						hostServer.playerInputs[0].newClick = true;
						hostServer.playerInputs[0].clickIndex = GameBoard.getTileN(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
					}
					else {
						hostServer.playerInputs[1].newClick = true;
						hostServer.playerInputs[1].clickIndex = GameBoard.getTileN(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
					}
				}
			}
		}
		//Collect packets from clients, then broadcast new information 
		if (serverStatus = 's') {
			hostServer.collectPackets();
			hostServer.distributePackets();
			//Set input clicks used by game based on the whose turn it is
			GameBoard.newClick = hostServer.playerInputs[GameHandler.playerTurn].newClick;
			GameBoard.clickPos = hostServer.playerInputs[GameHandler.playerTurn].clickIndex;
		}
		//Send packet to server, then receive new information
		else {
			client.sendPlayerPacket();
			client.receivePackets();
			//Set input clicks used by game based on the whose turn it is
			GameBoard.newClick = client.playerInputs[GameHandler.playerTurn].newClick;
			GameBoard.clickPos = client.playerInputs[GameHandler.playerTurn].clickIndex;
		}

		//Original gameplay
		GameBoard.drawBoard(window);
		GameHandler.drawTurn(window);
		GameHandler.turnChanged = false;

		for (Unit* u : unitsPtr) {
			u->update(window, unitsPtr);
		}

		if (GameHandler.showPromotion == true) {
			GameHandler.drawPromoteWindow(window);
			GameHandler.promoteChoice(GameBoard, window);
		}

		//Update effects on a new turn. A default game has extra rules for pawns and kings
		if (GameHandler.turnChanged == true) {
			GameHandler.newTurn = true;
			for (Pawn* p : pawnsPtr) {
				//Decrement en passant check
				if (p->twoStepped > 0)
					p->twoStepped--;
				//Check for promoted pieces
				if (p->promoteCheck() == true) {
					GameHandler.promoteColor = p->team;
					GameHandler.showPromotion = true;
					promotingPawn = p;
				}
			}
			for (King* k : kingsPtr) {
				//Invalidate castle move if king was moved (including castle move initiated by a rook)
				if (k->startPos != k->position)
					k->hasMoved = 1;
				if (!k->alive)
					declareWinner = GameHandler.setVictory(k->team);
			}
		}
		else {
			GameHandler.newTurn = false;
			if (promotingPawn != &nullPawn) {
				switch (GameHandler.promoteChosen) {
				case 0:
					//Promote to rook
					unitsPtr.push_back(new Rook(GameHandler, GameBoard, promotingPawn->position, promotingPawn->team));
					promotedCount++;
					promotingPawn->alive = false;
					promotingPawn->position = GameBoard.tileCount;
					GameHandler.showPromotion = false;
					promotingPawn = &nullPawn;
					break;
				case 1:
					//Promote to knight
					unitsPtr.push_back(new Knight(GameHandler, GameBoard, promotingPawn->position, promotingPawn->team));
					promotedCount++;
					promotingPawn->alive = false;
					promotingPawn->position = GameBoard.tileCount;
					GameHandler.showPromotion = false;
					promotingPawn = &nullPawn;
					break;
				case 2:
					//Promote to bishop
					unitsPtr.push_back(new Bishop(GameHandler, GameBoard, promotingPawn->position, promotingPawn->team));
					promotedCount++;
					promotingPawn->alive = false;
					promotingPawn->position = GameBoard.tileCount;
					GameHandler.showPromotion = false;
					promotingPawn = &nullPawn;
					break;
				case 3:
					//Promote to queen
					unitsPtr.push_back(new Queen(GameHandler, GameBoard, promotingPawn->position, promotingPawn->team));
					promotedCount++;
					promotingPawn->alive = false;
					promotingPawn->position = GameBoard.tileCount;
					GameHandler.showPromotion = false;
					promotingPawn = &nullPawn;
					break;
				}
			}
		}

		

		if (declareWinner == true) {
			GameHandler.drawVictory(window);
		}

		window.display();
	}
	//Delete any used dynamic memory
	for (int i = 0; i < promotedCount; i++) {
		delete unitsPtr.back();
		unitsPtr.pop_back();
	}
    return 0;
}


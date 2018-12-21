// Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Unit.h"
#include "Game.h"

const int windowW = 800;
const int windowH = 800;
const int tilesPerRow = 8;
const int tilesPerCol = 8;

int main()
{
	sf::RenderWindow window(sf::VideoMode(windowW, windowH), "Chess");

	//Objects for gameplay
	Board GameBoard(window, windowW, windowH, tilesPerRow, tilesPerCol);
	Game GameHandler;

	Pawn testPawn1(GameHandler, GameBoard, 10, black);
	Pawn testPawn2(GameHandler, GameBoard, 25, white);
	Rook testRook1(GameHandler, GameBoard, 19, white);
	Knight testKnight1(GameHandler, GameBoard, 30, black);
	Bishop testBishop1(GameHandler, GameBoard, 38, white);
	Queen testQueen1(GameHandler, GameBoard, 40, black);
	King testKing1(GameHandler, GameBoard, 50, black);

	bool declareWinner = false;

	//Pointers for all pieces to manipulate in class functions
	std::vector<Unit*> unitsPtr;
	unitsPtr.push_back(&testPawn1);
	unitsPtr.push_back(&testPawn2);
	unitsPtr.push_back(&testRook1);
	unitsPtr.push_back(&testKnight1);
	unitsPtr.push_back(&testBishop1);
	unitsPtr.push_back(&testQueen1);
	unitsPtr.push_back(&testKing1);
	std::vector<Pawn*> pawnsPtr;
	pawnsPtr.push_back(&testPawn1);
	pawnsPtr.push_back(&testPawn2);

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
					//Register a new mouse click for piece selection functions
					GameBoard.newClick = true;
					//Find the tile that was clicked in
					GameBoard.clickPos = GameBoard.getTileN(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				}
			}
		}
		GameBoard.drawBoard(window);
		GameHandler.drawTurn(window);
		GameHandler.turnChanged = false;

		for (Unit* u : unitsPtr) {
			u->update(window, unitsPtr);
		}

		//Update effects on a new turn. A default game only considers en passant rule and defeated kings
		if (GameHandler.turnChanged == true) {
			GameHandler.newTurn = true;
			for (Pawn* p : pawnsPtr) {
				if (p->twoStepped > 0)
					p->twoStepped--;
			}
		}
		else {
			GameHandler.newTurn = false;
		}

		if (declareWinner == true) {
			GameHandler.drawVictory(window);
		}

		window.display();
	}
    return 0;
}


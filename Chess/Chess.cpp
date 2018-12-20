// Chess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Unit.h"

const int windowW = 800;
const int windowH = 800;
const int tilesPerRow = 8;
const int tilesPerCol = 8;

int main()
{
	sf::RenderWindow window(sf::VideoMode(windowW, windowH), "Chess");

	//Objects for gameplay
	Board GameBoard(window, windowW, windowH, tilesPerRow, tilesPerCol);

	Pawn testPawn1(GameBoard, 10, black);
	Pawn testPawn2(GameBoard, 26, white);
	Rook testRook1(GameBoard, 19, white);

	//int piecesPerTeam = 2;
	//int startingPlayer = 0;
	//Game GameHandler(startingPlayer, piecesPerTeam);
	bool declareWinner = false;

	//Pointers for all pieces to manipulate in class functions
	std::vector<Unit*> unitsPtr;
	unitsPtr.push_back(&testPawn1);
	unitsPtr.push_back(&testPawn2);
	unitsPtr.push_back(&testRook1);

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
		for (Unit* u : unitsPtr) {
			u->update(window, unitsPtr);
		}

		window.display();
	}
    return 0;
}


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
	bool declareWinner = false;

	//Vectors of units
	std::vector<Unit*> unitsPtr;
	std::vector<Pawn*> pawnsPtr;
	std::vector<King*> kingsPtr;
	for (int i = 0; i < 8; i++) {
		pawnsPtr.push_back(new Pawn(GameHandler, GameBoard, i + 8, black));
		unitsPtr.push_back(pawnsPtr[2 * i]);
		pawnsPtr.push_back(new Pawn(GameHandler, GameBoard, GameBoard.tileCount - 9 - i, white));
		unitsPtr.push_back(pawnsPtr[2 * i + 1]);
	}
	for (int i = 0; i < 2; i++) {
		unitsPtr.push_back(new Rook(GameHandler, GameBoard, i * 7, black));
		unitsPtr.push_back(new Rook(GameHandler, GameBoard, GameBoard.tileCount - 1 - i * 7, white));
		unitsPtr.push_back(new Knight(GameHandler, GameBoard, i * 5 + 1, black));
		unitsPtr.push_back(new Knight(GameHandler, GameBoard, GameBoard.tileCount - 2 - i * 5, white));
		unitsPtr.push_back(new Bishop(GameHandler, GameBoard, i * 3 + 2, black));
		unitsPtr.push_back(new Bishop(GameHandler, GameBoard, GameBoard.tileCount - 3 - i * 3, white));
	}
	unitsPtr.push_back(new Queen(GameHandler, GameBoard, 3, black));
	unitsPtr.push_back(new Queen(GameHandler, GameBoard, GameBoard.tileCount - 5, white));
	kingsPtr.push_back(new King(GameHandler, GameBoard, 4, black));
	unitsPtr.push_back(kingsPtr[0]);
	kingsPtr.push_back(new King(GameHandler, GameBoard, GameBoard.tileCount - 4, white));
	unitsPtr.push_back(kingsPtr[1]);

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
				//Decrement en passant check
				if (p->twoStepped > 0)
					p->twoStepped--;
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
		}

		if (declareWinner == true) {
			GameHandler.drawVictory(window);
		}

		window.display();
	}
    return 0;
}


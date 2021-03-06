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


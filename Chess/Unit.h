#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Game.h"

class Unit {
public:
	Game* gameref;
	Board* boardref;
	int position;
	colortype team;
	bool alive = true;

	sf::Sprite unitSprite;
	sf::Texture unitTexture;

	bool selected = false;
	sf::RectangleShape selectorBox;

	Unit(Game& gref, Board& bref, int pos, colortype col);
	void update(sf::RenderWindow& window, std::vector<Unit*> units);
	virtual std::pair<int, int> specialRule() { return std::make_pair(0, 0); }

protected:
	void deselectUnit();

private:
	void drawUnit(sf::RenderWindow& window);
	void selectUnit();
	virtual void moveUnit(std::vector<Unit*> units) = 0;
};

class Pawn : public Unit {
public:
	bool hasMoved = false;
	int twoStepped = 0;
	Pawn(Game& gref, Board& bref, int pos, colortype col);

private:
	void moveUnit(std::vector<Unit*> units);
	//En passant rule
	std::pair<int, int> specialRule();
};

class Rook : public Unit {
public:
	bool hasMoved = false;
	Rook(Game& gref, Board& bref, int pos, colortype col);
	
private:
	void moveUnit(std::vector<Unit*> units);
	//Castle move with king
	//std::pair<int, int> specialRule();
};

class Knight : public Unit {
public:
	//Knight(Board& bref, int pos, colortype col);
private:
	//void moveUnit(std::vector<Unit*> units);
};
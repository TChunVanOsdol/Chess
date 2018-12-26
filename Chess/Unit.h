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
	int unitType = 0;

	sf::Sprite unitSprite;
	sf::Texture unitTexture;

	bool selected = false;
	sf::RectangleShape selectorBox;

	Unit() {}
	Unit(Game& gref, Board& bref, int pos, colortype col);
	void update(sf::RenderWindow& window, std::vector<Unit*> units);
	virtual std::pair<int, int> specialRule() { return std::make_pair(0, unitType); }

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
	Pawn() {}
	Pawn(Game& gref, Board& bref, int pos, colortype col);
	bool promoteCheck();

private:
	void moveUnit(std::vector<Unit*> units);
	//En passant rule
	std::pair<int, int> specialRule();
};

void addRook(Pawn p, std::vector<Unit*> units);

class Rook : public Unit {
public:
	int hasMoved = 0;
	Rook(Game& gref, Board& bref, int pos, colortype col);
	
private:
	void moveUnit(std::vector<Unit*> units);
	//Castle move with king
	std::pair<int, int> specialRule();
};

class Knight : public Unit {
public:
	Knight(Game& gref, Board& bref, int pos, colortype col);
private:
	void moveUnit(std::vector<Unit*> units);
};

class Bishop : public Unit {
public:
	Bishop(Game& gref, Board& bref, int pos, colortype col);
private:
	void moveUnit(std::vector<Unit*> units);
};

class Queen : public Unit {
public:
	Queen(Game& gref, Board& bref, int pos, colortype col);
private:
	void moveUnit(std::vector<Unit*> units);
};

class King : public Unit {
public:
	int hasMoved = 0;
	int startPos;
	King(Game& gref, Board& bref, int pos, colortype col);
private:
	void moveUnit(std::vector<Unit*> units);
	//Castle move with rook
	std::pair<int, int> specialRule();
};
#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"

class Unit {
public:
	Board* boardref;
	int position;
	colortype team;
	bool alive = true;

	sf::Sprite unitSprite;
	sf::Texture unitTexture;

	bool selected = false;
	sf::RectangleShape selectorBox;

	Unit(Board& bref, int pos, colortype col);
	void update(sf::RenderWindow& window, std::vector<Unit*> units);

protected:
	void deselectUnit();

private:
	void drawUnit(sf::RenderWindow& window);
	void selectUnit();
	virtual void moveUnit(std::vector<Unit*> units) =0;
};

class Pawn : public Unit {
public:
	bool hasMoved = false;
	Pawn(Board& bref, int pos, colortype col);

private:
	void moveUnit(std::vector<Unit*> units);
};

class Rook : public Unit {
public:
	bool hasMoved = false;
	Rook(Board& bref, int pos, colortype col);
	
private:
	void moveUnit(std::vector<Unit*> units);
};
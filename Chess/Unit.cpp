#include "stdafx.h"
#include "Unit.h"
#include <iostream>

Unit::Unit(Board& bref, int pos, colortype col) {
	boardref = &bref;
	position = pos;
	team = col;
	sf::Vector2f boxSize(bref.tileW, bref.tileH);
	selectorBox.setSize(boxSize);
	selectorBox.setFillColor(sf::Color::Transparent);
	selectorBox.setOutlineColor(sf::Color::Green);
	selectorBox.setOutlineThickness(0.f);
	selectorBox.setPosition(bref.getTilePos(pos));
}

void Unit::update(sf::RenderWindow& window, std::vector<Unit*> units) {
	drawUnit(window);
	selectUnit();
	moveUnit(units);
}

void Unit::drawUnit(sf::RenderWindow& window) {
	unitSprite.setPosition(boardref->getTilePos(position));
	selectorBox.setPosition(boardref->getTilePos(position));
	window.draw(selectorBox);
	window.draw(unitSprite);
}

void Unit::selectUnit() {
	//Select unit
	if (boardref->clickPos == position && selected == false && boardref->newClick == true) {
		selected = true;
		selectorBox.setOutlineThickness(5.f);
	}
	//Deselect unit
	else if (boardref->clickPos == position && selected == true && boardref->newClick == true) {
		//Clicked on piece again
		deselectUnit();
	}
}

void Unit::deselectUnit() {
	selectorBox.setOutlineThickness(0.f);
	selected = false;
}

Pawn::Pawn(Board& bref, int pos, colortype col) : Unit(bref, pos, col) {
	if (col == black)
		unitTexture.loadFromFile("Textures/PawnB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/PawnW.png");
	unitSprite.setTexture(unitTexture);
}

void Pawn::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Check all units to see if one of them is on the new click position
		bool pieceBlocked = false;
		Unit* target = this;
		for (Unit* u : units) {
			if (boardref->clickPos == u->position && u->alive) {
				//Location is blocked by another unit
				pieceBlocked = true;
				target = u;
				break;
			}
		}
		//Set pawn direction
		int dir;
		if (team == black)
			dir = 1;
		else if (team == white)
			dir = -1;
		//Pawn moves straight
		if (pieceBlocked == false && boardref->clickPos == position + dir * boardref->tilesPerRow) {
			position = boardref->clickPos;
			deselectUnit();
			hasMoved = true;
		}
		//Pawn moves 2 on first move
		else if (pieceBlocked == false && boardref->clickPos == position + 2 * dir * boardref->tilesPerRow && hasMoved == false) {
			//Prevent movement if a unit is in the way
			for (Unit* u : units) {
				if (u->position == position + dir * boardref->tilesPerRow) {
					pieceBlocked = true;
				}
			}
			if (pieceBlocked == false) {
				position = boardref->clickPos;
				hasMoved = true;
			}
			deselectUnit();
		}
		//Pawn attacks diagonally
		else if (pieceBlocked == true && (boardref->clickPos == position + dir * boardref->tilesPerRow - 1 || boardref->clickPos == position + dir * boardref->tilesPerRow + 1) && target->team != team) {
			position = boardref->clickPos;
			deselectUnit();
			hasMoved = true;
			//Eliminate enemy piece
			target->alive = false;
			target->position = boardref->tileCount;
		}
		//Invalid move, deselect
		else {
			deselectUnit();
		}
	}
}

Rook::Rook(Board& bref, int pos, colortype col) : Unit(bref, pos, col) {
	if (col == black)
		unitTexture.loadFromFile("Textures/RookB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/RookW.png");
	unitSprite.setTexture(unitTexture);
}

void Rook::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Set rook direction
		int dir;
		//Check for positive or negative direction
		if (boardref->clickPos > position) {
			dir = 1;
		}
		else if (boardref->clickPos < position) {
			dir = -1;
		}
		//Check if target and rook are in the same column
		if (boardref->clickPos % boardref->tilesPerRow == position % boardref->tilesPerRow) {
			dir *= boardref->tilesPerRow;
		}
		//Check if target is invalid
		else if (boardref->clickPos / boardref->tilesPerRow != position / boardref->tilesPerRow) {
			deselectUnit();
			return;
		}
		//Check all units to see if any are blocking this unit, or if one is on the target space
		bool pieceBlocked = false;
		Unit* target = this;
		//First, iterate each step from unit to target
		for (int i = 1; i <= (boardref->clickPos - position) / dir; i ++) {
			//Then search all units for a possible block
			for (Unit* u : units) {
				if (position + i * dir == u->position && u->alive) {
					//If this is the clicked position, set target instead
					if (boardref->clickPos == u->position) {
						target = u;
					}
					//Otherwise, the target is blocked by another unit
					else {
						pieceBlocked = true;
					}
					break;
				}
			}
		}
		//Rook moves straight
		if (pieceBlocked == false && (target->team != team || target == this)) {
			position = boardref->clickPos;
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
			hasMoved = true;
		}
		//Blocked move, deselect
		else {
			deselectUnit();
		}
	}
}
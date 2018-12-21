#include "stdafx.h"
#include "Unit.h"
#include <iostream>

Unit::Unit(Game& gref, Board& bref, int pos, colortype col) {
	gameref = &gref;
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
	if (team == gameref->turnColor && boardref->clickPos == position && selected == false && boardref->newClick == true) {
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

Pawn::Pawn(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 1;
	if (col == black)
		unitTexture.loadFromFile("Textures/PawnB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/PawnW.png");
	unitSprite.setTexture(unitTexture);
}

void Pawn::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Set pawn direction
		int dir;
		if (team == black)
			dir = 1;
		else if (team == white)
			dir = -1;
		//Check all units to see if one of them is on the new click position
		bool pieceBlocked = false;
		bool enPassant = false;
		Unit* target = this;
		for (Unit* u : units) {
			if (u->specialRule() == std::make_pair(1, 1) && boardref->clickPos == u->position + dir * boardref->tilesPerRow) {
				//En passant rule check
				enPassant = true;
				target = u;
				break;
			}
			if (boardref->clickPos == u->position && u->alive) {
				//Location is blocked by another unit
				pieceBlocked = true;
				target = u;
				break;
			}
		}
		//Pawn moves straight
		if (pieceBlocked == false && boardref->clickPos == position + dir * boardref->tilesPerRow) {
			position = boardref->clickPos;
			deselectUnit();
			gameref->changeTurn();
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
				gameref->changeTurn();
				hasMoved = true;
				twoStepped = 2;
			}
			deselectUnit();
		}
		//En passant rule
		else if (pieceBlocked == false && (boardref->clickPos == position + dir * boardref->tilesPerRow - 1 || boardref->clickPos == position + dir * boardref->tilesPerRow + 1) && enPassant == true) {
			position = boardref->clickPos;
			deselectUnit();
			gameref->changeTurn();
			hasMoved = true;
			//Eliminate enemy piece
			target->alive = false;
			target->position = boardref->tileCount;
		}
		//Pawn attacks diagonally
		else if (pieceBlocked == true && (boardref->clickPos == position + dir * boardref->tilesPerRow - 1 || boardref->clickPos == position + dir * boardref->tilesPerRow + 1) && target->team != team) {
			position = boardref->clickPos;
			deselectUnit();
			gameref->changeTurn();
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

std::pair<int, int> Pawn::specialRule() {
	return std::make_pair(twoStepped, unitType);
}

Rook::Rook(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 2;
	if (col == black)
		unitTexture.loadFromFile("Textures/RookB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/RookW.png");
	unitSprite.setTexture(unitTexture);
}

void Rook::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Set rook direction
		int dir = 0;
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
		//If target was not in the same column, then also check if it is not in the same row
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
			gameref->changeTurn();
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
			hasMoved = 1;
		}
		//Castle move with king
		else if (pieceBlocked == false && target->team == team && target->specialRule() == std::make_pair(0,6) && hasMoved == 0) {
			//Move king two spaces towards rook
			target->position -= 2 * dir;
			//Move rook behind king
			position = target->position + dir;
			gameref->changeTurn();
			deselectUnit();
			hasMoved = 1;
		}
		//Blocked move, deselect
		else {
			deselectUnit();
		}
	}
}

std::pair<int, int> Rook::specialRule() {
	return std::make_pair(hasMoved, unitType);
}

Knight::Knight(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 3;
	if (col == black)
		unitTexture.loadFromFile("Textures/KnightB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/KnightW.png");
	unitSprite.setTexture(unitTexture);
}

void Knight::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Check all units to see if any are on the target space
		bool pieceBlocked = false;
		Unit* target = this;
		for (Unit* u : units) {
			if (boardref->clickPos == u->position && u->alive) {
				//Blocking piece is an enemy
				if (u->team != team) {
					target = u;
				}
				//Blocking piece is an ally
				else {
					pieceBlocked = true;
				}
				break;
			}
		}
		//Find a valid move using a diagonal of the knight's position plus a step outwards in two directions
		int corner = 0;
		int dir = 0;
		//Target above knight
		if (boardref->clickPos / boardref->tilesPerRow < position / boardref->tilesPerRow) {
			corner = -boardref->tilesPerRow;
		}
		//Target below knight
		else if (boardref->clickPos / boardref->tilesPerRow > position / boardref->tilesPerRow) {
			corner = boardref->tilesPerRow;
		}
		//Target right of knight
		if (boardref->clickPos % boardref->tilesPerRow > position % boardref->tilesPerRow) {
			corner++;
			dir = 1;
		}
		//Target left of knight
		else if (boardref->clickPos % boardref->tilesPerRow < position % boardref->tilesPerRow) {
			corner--;
			dir = -1;
		}
		//Move is invalid
		if (abs(corner) != boardref->tilesPerRow + 1 && abs(corner) != boardref->tilesPerRow - 1) {
			deselectUnit();
		}
		//Knight moves
		else if (pieceBlocked == false && (target->team != team || target == this) && (boardref->clickPos == position + corner + dir || boardref->clickPos == position + 2 * corner - dir)) {
			position = boardref->clickPos;
			gameref->changeTurn();
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
		}
		//Invalid move, deselect
		else {
			deselectUnit();
		}
	}
}

Bishop::Bishop(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 4;
	if (col == black)
		unitTexture.loadFromFile("Textures/BishopB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/BishopW.png");
	unitSprite.setTexture(unitTexture);
}

void Bishop::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Find distance from position
		int x = 0;
		int y = 0;
		//Set x distance
		x = boardref->clickPos % boardref->tilesPerRow - position % boardref->tilesPerRow;
		//Set y distance
		y = boardref->clickPos / boardref->tilesPerRow - position / boardref->tilesPerRow;
		//Not a diagonal move; invalid
		if (abs(x) != abs(y)) {
			deselectUnit();
			return;
		}
		//Find direction
		int dir = 0;
		if (y > 0) {
			dir = boardref->tilesPerRow;
		}
		else if (y < 0) {
			dir = -boardref->tilesPerRow;
		}
		if (x > 0) {
			dir++;
		}
		else if (x < 0) {
			dir--;
		}
		//Check all units to see if any are blocking this unit, or if one is on the target space
		bool pieceBlocked = false;
		Unit* target = this;
		//First, iterate each step from unit to target
		for (int i = 1; i <= (boardref->clickPos - position) / dir; i++) {
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
		//Bishop moves
		if (pieceBlocked == false && (target->team != team || target == this)) {
			position = boardref->clickPos;
			gameref->changeTurn();
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
		}
		//Invalid move, deselect
		else {
			deselectUnit();
		}
	}
}

Queen::Queen(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 5;
	if (col == black)
		unitTexture.loadFromFile("Textures/QueenB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/QueenW.png");
	unitSprite.setTexture(unitTexture);
}

void Queen::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Find distance from position
		int x = 0;
		int y = 0;
		//Set x distance
		x = boardref->clickPos % boardref->tilesPerRow - position % boardref->tilesPerRow;
		//Set y distance
		y = boardref->clickPos / boardref->tilesPerRow - position / boardref->tilesPerRow;
		//Not a straight or diagonal move; invalid
		if (x * y != 0 && abs(x) != abs(y)) {
			deselectUnit();
			return;
		}
		//Find direction
		int dir = 0;
		if (y > 0) {
			dir = boardref->tilesPerRow;
		}
		else if (y < 0) {
			dir = -boardref->tilesPerRow;
		}
		if (x > 0) {
			dir++;
		}
		else if (x < 0) {
			dir--;
		}
		//Check all units to see if any are blocking this unit, or if one is on the target space
		bool pieceBlocked = false;
		Unit* target = this;
		//First, iterate each step from unit to target
		for (int i = 1; i <= (boardref->clickPos - position) / dir; i++) {
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
		//Queen moves
		if (pieceBlocked == false && (target->team != team || target == this)) {
			position = boardref->clickPos;
			gameref->changeTurn();
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
		}
		//Invalid move, deselect
		else {
			deselectUnit();
		}
	}
}

King::King(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	startPos = pos;
	unitType = 6;
	if (col == black)
		unitTexture.loadFromFile("Textures/KingB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/KingW.png");
	unitSprite.setTexture(unitTexture);
}

void King::moveUnit(std::vector<Unit*> units) {
	if (selected == true && boardref->clickPos != position) {
		//Check all units to see if any are on the target space
		bool pieceBlocked = false;
		Unit* target = this;
		for (Unit* u : units) {
			if (boardref->clickPos == u->position && u->alive) {
				//Blocking piece is an enemy or castling rook
				if (u->team != team || u->specialRule() == std::make_pair(0, 2)) {
					target = u;
				}
				//Blocking piece is an ally
				else {
					pieceBlocked = true;
				}
				break;
			}
		}
		//King moves
		if (pieceBlocked == false && (target->team != team || target == this) &&
			((boardref->clickPos % boardref->tilesPerRow <= position % boardref->tilesPerRow + 1 && boardref->clickPos % boardref->tilesPerRow >= position % boardref->tilesPerRow - 1) &&
			(boardref->clickPos / boardref->tilesPerRow <= position / boardref->tilesPerRow + 1 && boardref->clickPos / boardref->tilesPerRow >= position / boardref->tilesPerRow - 1))) {
			position = boardref->clickPos;
			gameref->changeTurn();
			//Eliminate enemy piece if a target was set as an enemy
			if (target != this) {
				target->alive = false;
				target->position = boardref->tileCount;
			}
			deselectUnit();
			hasMoved = 1;
		}
		//Castle move with king
		else if (pieceBlocked == false && target->team == team && target->specialRule() == std::make_pair(0, 2) && hasMoved == 0) {
			int dir = 0;
			if (boardref->clickPos > position)
				dir = 1;
			else if (boardref->clickPos < position)
				dir = -1;
			//Move king two spaces towards rook
			position += 2 * dir;
			//Move rook behind king
			target->position = position - dir;
			gameref->changeTurn();
			deselectUnit();
			hasMoved = 1;
		}
		//Invalid move, deselect
		else {
			deselectUnit();
		}
	}
}

std::pair<int, int> King::specialRule() {
	return std::make_pair(hasMoved, unitType);
}
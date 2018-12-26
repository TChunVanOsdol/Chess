#include "stdafx.h"
#include "Ninja.h"

Ninja::Ninja(Game& gref, Board& bref, int pos, colortype col) : Unit(gref, bref, pos, col) {
	unitType = 7;
	if (col == black)
		unitTexture.loadFromFile("Textures/NinjaB.png");
	else if (col == white)
		unitTexture.loadFromFile("Textures/NinjaW.png");
	unitSprite.setTexture(unitTexture);
}

void Ninja::moveUnit(std::vector<Unit*> units) {
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
		//Set ninja direction
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
		//Ninja moves in a straight line, up to 3 tiles. Cannot be blocked by units.
		if (pieceBlocked == false && (target->team != team || target == this) && (boardref->clickPos - position) / dir <= 3) {
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
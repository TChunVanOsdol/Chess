#pragma once
#include "Unit.h"

class Ninja : public Unit {
public:
	Ninja(Game& gref, Board& bref, int pos, colortype col);
private:
	void moveUnit(std::vector<Unit*> units);
};
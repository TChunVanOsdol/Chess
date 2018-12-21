#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Board.h"

const int playerCount = 2;

class Game {
public:
	int playerTurn = 1;
	sf::RectangleShape turnIndicator;
	colortype turnColor;

	bool newTurn = true;
	bool turnChanged = false;

	std::string victoryMsg[playerCount];
	sf::Text victoryText;
	sf::Font victoryFont;

	Game();
	void changeTurn();
	void drawTurn(sf::RenderWindow &window);
	void drawVictory(sf::RenderWindow &window);
	bool setVictory(colortype player);
};
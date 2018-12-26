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

	bool showPromotion = false;
	colortype promoteColor;
	sf::RectangleShape promoteWindow;
	sf::RectangleShape promoteOption;
	sf::Sprite promoteSprite[4];
	sf::Texture promoteTexture[8];
	int promoteChosen = -1;

	std::string victoryMsg[playerCount];
	sf::Text victoryText;
	sf::Font victoryFont;

	Game();
	void changeTurn();
	void drawTurn(sf::RenderWindow &window);
	void drawVictory(sf::RenderWindow &window);
	bool setVictory(colortype player);
	void promoteChoice(Board &boardref, sf::RenderWindow &window);
	void drawPromoteWindow(sf::RenderWindow &window);
};
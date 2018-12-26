#pragma once
#include "stdafx.h"
#include "Game.h"

Game::Game() {
	victoryFont.loadFromFile("Textures/arial.ttf");
	victoryText.setFont(victoryFont);
	victoryText.setPosition(170.f, 320.f);
	victoryText.setFillColor(sf::Color::Green);
	victoryText.setCharacterSize(60);
	victoryMsg[0] = "Black is defeated!";
	victoryMsg[1] = "White is defeated!";

	turnIndicator.setFillColor(sf::Color::White);
	turnIndicator.setSize({ 23, 23 });
	turnIndicator.setPosition({ 2,2 });
	turnIndicator.setOutlineColor(sf::Color::Blue);
	turnIndicator.setOutlineThickness(2.f);
	turnColor = white;

	promoteWindow.setSize({ 300.f, 300.f });
	promoteWindow.setPosition(250.f, 250.f);
	promoteWindow.setFillColor(sf::Color::White);
	promoteWindow.setOutlineColor(sf::Color::Black);
	promoteWindow.setOutlineThickness(5.f);
	promoteOption.setSize({ 100.f, 100.f });
	promoteOption.setOutlineColor(sf::Color::Green);
	promoteOption.setOutlineThickness(5.f);
	promoteTexture[0].loadFromFile("Textures/RookB.png");
	promoteTexture[1].loadFromFile("Textures/RookW.png");
	promoteTexture[2].loadFromFile("Textures/KnightB.png");
	promoteTexture[3].loadFromFile("Textures/KnightW.png");
	promoteTexture[4].loadFromFile("Textures/BishopB.png");
	promoteTexture[5].loadFromFile("Textures/BishopW.png");
	promoteTexture[6].loadFromFile("Textures/QueenB.png");
	promoteTexture[7].loadFromFile("Textures/QueenW.png");
}

void Game::changeTurn() {
	//Increment the turn and loop around when it passes the player count
	playerTurn++;
	if (playerTurn >= playerCount) {
		playerTurn = 0;
	}
	switch (playerTurn) {
	case 0:
		turnColor = black;
		turnIndicator.setFillColor(sf::Color::Black);
		break;
	case 1:
		turnColor = white;
		turnIndicator.setFillColor(sf::Color::White);
		break;
	}
	//Track that a new turn is coming up
	turnChanged = true;
}

void Game::drawTurn(sf::RenderWindow &window) {
	window.draw(turnIndicator);
}

void Game::drawVictory(sf::RenderWindow &window) {
	window.draw(victoryText);
}

bool Game::setVictory(colortype player) {
	victoryText.setString(victoryMsg[player]);
	return true;
}

void Game::promoteChoice(Board &boardref, sf::RenderWindow &window) {
	promoteChosen = -1;
	for (int i = 0; i < 4; i++) {
		if (boardref.newClick == true && promoteSprite[i].getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
			promoteChosen = i;
		}
	}
}

void Game::drawPromoteWindow(sf::RenderWindow &window) {
	window.draw(promoteWindow);
	sf::Vector2f objPos;
	int textureColor = 0;
	int spriteNumber = 0;
	promoteOption.setFillColor(sf::Color::Black);
	if (promoteColor == white) {
		textureColor = 1;
		promoteOption.setFillColor(sf::Color::White);
	}
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			objPos.x = 150 * x + 275;
			objPos.y = 150 * y + 275;
			promoteOption.setPosition(objPos);
			promoteSprite[spriteNumber].setTexture(promoteTexture[textureColor]);
			promoteSprite[spriteNumber].setPosition(objPos);
			window.draw(promoteOption);
			window.draw(promoteSprite[spriteNumber]);
			textureColor += 2;
			spriteNumber++;
		}
	}
}
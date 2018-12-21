#pragma once
#include "stdafx.h"
#include "Game.h"

Game::Game() {
	victoryFont.loadFromFile("Assets/arial.ttf");
	victoryText.setFont(victoryFont);
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
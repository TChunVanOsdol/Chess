#pragma once
#include "stdafx.h"
#include "Board.h"

Board::Board(sf::RenderWindow &window, int height, int width, int tilesInRow, int tilesInCol) {
	boardH = height;
	boardW = width;
	tilesPerRow = tilesInRow;
	tilesPerCol = tilesInCol;
	tileCount = tilesPerCol * tilesPerRow;
	tileH = boardH / tilesPerCol;
	tileW = boardW / tilesPerRow;
	tileShape = sf::RectangleShape(sf::Vector2f(tileW, tileH));
	//Generate vector for tiles
	tileTypes = std::vector<colortype>(tileCount);
	for (int i = 0; i < tileCount; i++) {
		//Try running through the first few iterations, what happens when 8 <= i < 16?
		if ((i % 2 - (i / tilesPerRow) % 2) == 0) {
			tileTypes[i] = white;
		}
		else {
			tileTypes[i] = black;
		}
	}
}

void Board::drawBoard(sf::RenderWindow &window) {
	//Move a single rectangle across the board to draw each tile
	sf::Vector2f tilePos;
	for (int i = 0; i < tileCount; i++) {
		tilePos = getTilePos(i);
		tileShape.setPosition(tilePos);
		if (tileTypes[i] == white) {
			tileShape.setFillColor(sf::Color::White);
		}
		else {
			tileShape.setFillColor(sf::Color::Black);
		}
		window.draw(tileShape);
	}
}

int Board::getTileN(float x, float y) {
	//Find the index number starting at 0,0 going right then down
	int tileX, tileY;
	tileX = int(x / tileW);
	tileY = int(y / tileH);
	return tileX + (tileY * tilesPerCol);
}

sf::Vector2f Board::getTilePos(int N) {
	//Return position of tile's origin in pixels
	float posX, posY;
	posX = (N % tilesPerRow) * tileW;
	posY = (N / tilesPerCol) * tileH;
	return sf::Vector2f(posX, posY);
}
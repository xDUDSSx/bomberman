#include "Map.h"

#include "Tile.h"
#include "Constants.h"
#include "Game.h"
#include "MapData.h"
#include "EntityManager.h"

#include <iostream>
#include <sstream>

#include "Utils.h"

Map::Map() = default;

Map::Map(const Map &map) {
	*this = map;
}

Map& Map::operator=(const Map& map) {
	if (this != &map) {
		this->mapWidth = map.mapWidth;
		this->mapHeight = map.mapHeight;
		this->tileSize = map.tileSize;
		this->mapTileWidth = map.mapTileWidth;
		this->mapTileHeight = map.mapTileHeight;

		fillWithEmptyPointers();

		for (int y = 0; y < mapTileHeight; y++) {
			for (int x = 0; x < mapTileWidth; x++) {
				tiles[x][y] = std::make_shared<Tile>(Tile(*map.tiles[x][y]));
			}
		}
	}
	return *this;
}

void Map::generate(int pMapWidth, int pMapHeight, Game* game) {
	mapWidth = pMapWidth;
	mapHeight = pMapHeight;
	tileSize = Constants::TILE_SIZE;
	mapTileWidth = mapWidth / tileSize;
	mapTileHeight = mapHeight / tileSize;
	
	fillWithEmptyPointers();

	//Generating a default map
	for (int y = 0; y < mapTileHeight; y++) {
		for (int x = 0; x < mapTileWidth; x++) {
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			if (!((x < 3 && y < 3) || (x > mapTileWidth-4 && y > mapTileHeight-4))) {
				if (Utils::getRandomIntNumberInRange(0, 100) <= 80) {
					newTile->setWall(true);
				}
			}
			if (x % 2 == 0 && y % 2 == 0) {
				newTile->setWall(true);
				newTile->setIndestructible(true);
			}
			if (x == 0 || y == 0 || x == mapTileWidth - 1 || y == mapTileHeight - 1) {
				newTile->setWall(true);
				newTile->setIndestructible(true);
				newTile->setEdgeWall(true);
			}
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
		}
	}

	//Default players
	game->registerPlayer('A', getTileAtCoordinates(Constants::TILE_SIZE, Constants::TILE_SIZE));
	game->registerPlayer('B', getTileAtCoordinates(mapWidth - Constants::TILE_SIZE * 2, mapHeight - Constants::TILE_SIZE * 2));
}

bool Map::generateFromMapData(MapData* mapData, Game* game) {
	try {
		mapTileWidth = mapData->tileWidth;
		mapTileHeight = mapData->tileHeight;
		mapWidth = mapData->pixelWidth;
		mapHeight = mapData->pixelHeight;
		tileSize = mapData->pixelWidth / mapData->tileWidth;
		
		fillWithEmptyPointers();
		
		//Parsing map data symbols
		for (int y = 0; y < mapTileHeight; y++) {
			//Making sure that the lines have the same width
			std::string line = mapData->lines[y];
			if (static_cast<int>(line.length()) != mapTileWidth) {
				throw std::runtime_error("Map data map width is not consistent!");
			}

			//Parsing characters in a line
			for (int x = 0; x < mapTileWidth; x++) {
				const char c = line[x];
				parseCharacterAndLoadTile(x, y, c, game);
			}
		}
	} catch (std::runtime_error& error) {
		std::cerr << "Failed to generate map from map data! " << error.what() << std::endl;
		return false;
	}
	return true;
}

void Map::render(SDL_Renderer * renderer) {
	for (int y = 0; y < mapTileHeight; y++) {
		for (int x = 0; x < mapTileWidth; x++) {
			tiles[x][y]->render(renderer);
		}
	}
}

Tile* Map::getTileAtCoordinates(const int x, const int y) const {
	if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
		return nullptr;
	}
	Tile* tilePtr = tiles[x / tileSize][y / tileSize].get();
	return tilePtr;
}

Tile* Map::getTileAtIndexes(int x, int y) const {
	if (x < 0 || y < 0 || x >= mapTileWidth || y >= mapTileHeight) {
		return nullptr;
	}
	Tile* tilePtr = tiles[x][y].get();
	return tilePtr;
}

Tile* Map::getTileAbove(Tile * tile) const {
	return getTileAtCoordinates(tile->getCenterX(), tile->getCenterY() - tileSize);
}

Tile* Map::getTileLeft(Tile * tile) const {
	return getTileAtCoordinates(tile->getCenterX() - tileSize, tile->getCenterY());
}

Tile* Map::getTileRight(Tile * tile) const {
	return getTileAtCoordinates(tile->getCenterX() + tileSize, tile->getCenterY());
}

Tile* Map::getTileBelow(Tile * tile) const {
	return getTileAtCoordinates(tile->getCenterX(), tile->getCenterY() + tileSize);
}

Tile* Map::getTileTopLeftCorner(Tile* tile) const {
	return getTileAtCoordinates(tile->getCenterX() - tileSize, tile->getCenterY() - tileSize);
}

Tile* Map::getTileBottomLeftCorner(Tile* tile) const {
	return getTileAtCoordinates(tile->getCenterX() - tileSize, tile->getCenterY() + tileSize);
}

Tile* Map::getTileBottomRightCorner(Tile* tile) const {
	return getTileAtCoordinates(tile->getCenterX() + tileSize, tile->getCenterY() + tileSize);
}

Tile* Map::getTileTopRightCorner(Tile* tile) const {
	return getTileAtCoordinates(tile->getCenterX() + tileSize, tile->getCenterY() - tileSize);
}

std::vector<Tile*> Map::getNeighbourTiles(Tile* tile) const {
	std::vector<Tile*> neighbours = std::vector<Tile*>(8, nullptr);
	neighbours[0] = getTileAbove(tile);
	neighbours[1] = getTileTopLeftCorner(tile);
	neighbours[2] = getTileLeft(tile);
	neighbours[3] = getTileBottomLeftCorner(tile);
	neighbours[4] = getTileBelow(tile);
	neighbours[5] = getTileBottomRightCorner(tile);
	neighbours[6] = getTileRight(tile);
	neighbours[7] = getTileTopRightCorner(tile);
	return neighbours;
}

std::vector<Tile*> Map::getExistingNeighbourTiles(Tile* tile, bool ignoreCorners = false) const {
	std::vector<Tile*> neighbours;
	if (Tile* t = getTileAbove(tile)) neighbours.push_back(t);
	if (Tile* t = getTileTopLeftCorner(tile)) {
		if (!ignoreCorners) neighbours.push_back(t);
	}
	if (Tile* t = getTileLeft(tile)) neighbours.push_back(t);
	if (Tile* t = getTileBottomLeftCorner(tile)) {
		if (!ignoreCorners) neighbours.push_back(t);
	}
	if (Tile* t = getTileBelow(tile)) neighbours.push_back(t);
	if (Tile* t = getTileBottomRightCorner(tile)) {
		if (!ignoreCorners) neighbours.push_back(t);
	}
	if (Tile* t = getTileRight(tile)) neighbours.push_back(t);
	if (Tile* t = getTileTopRightCorner(tile)) {
		if (!ignoreCorners) neighbours.push_back(t);
	}
	return neighbours;
}

std::vector<Tile*> Map::getWalkableNeighbourTiles(Tile* tile, bool ignoreCorners = false) const {
	std::vector<Tile*> neighbours;
	for (int i = 0; i < 8; i++) {
		Tile* t = nullptr;
		switch (i) {
			case 0: t = getTileAbove(tile);  break;
			case 1: if (!ignoreCorners) t = getTileTopLeftCorner(tile);  break;
			case 2: t = getTileLeft(tile);  break;
			case 3: if (!ignoreCorners) t = getTileBottomLeftCorner(tile);  break;
			case 4: t = getTileBelow(tile);  break;
			case 5: if (!ignoreCorners) t = getTileBottomRightCorner(tile);  break;
			case 6: t = getTileRight(tile);  break;
			case 7: if (!ignoreCorners) t = getTileTopRightCorner(tile);  break;
		}
		if (t && t->isOverrideWalkable()) {
			neighbours.push_back(t);
		} else
		if (t && !t->isWall() && !t->isDangerous() && !t->isBombermanPresent()) {
			neighbours.push_back(t);
		}
	}
	return neighbours;
}

const std::vector<std::vector<std::shared_ptr<Tile>>>& Map::getTiles() const {
	return tiles;
}

void Map::parseCharacterAndLoadTile(const int x, const int y, const char c, Game* game) {
	switch (c) {
		case 'X':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			newTile->setWall(true);
			newTile->setIndestructible(true);
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			break;
		}
		case 'O':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			newTile->setWall(true);
			newTile->setIndestructible(true);
			newTile->setEdgeWall(true);
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			break;
		}
		case 'o':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			newTile->setWall(true);
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			break;
		}
		case '-':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			break;
		}
		case 'c':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			game->registerComputer(newTile.get());
			break;
		}
		case 'A':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			game->registerPlayer('A', newTile.get());
			break;
		}
		case 'B':
		{
			auto newTile = std::make_shared<Tile>(Tile(x * tileSize, y * tileSize));;
			tiles[x][y] = newTile;
			game->entityManager->addEntity(newTile);
			game->registerPlayer('B', newTile.get());
			break;
		}
		default:
			std::ostringstream oss;
			oss << "Invalid map character ('" << c << "') at row:column " << x << ":" << y << std::endl;
			throw std::runtime_error(oss.str());
	}
}

void Map::fillWithEmptyPointers() {
	for (int i = 0; i < mapTileWidth; i++) {
		tiles.push_back(std::vector<std::shared_ptr<Tile>>(mapTileHeight, std::shared_ptr<Tile>()));
	}
}
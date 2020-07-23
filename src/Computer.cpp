#include "Computer.h"

#include "Game.h"
#include "TextureManager.h"
#include "Utils.h"
#include "EntityManager.h"
#include "Map.h"
#include "Player.h"

#include <set>
#include <unordered_set>
#include <algorithm>
#include <list>

#include "PowerUp.h"

Computer::Computer(int id, int initialX, int initialY)
: Bomberman(initialX, initialY), id(id), randomNumberGenerator((std::random_device())()) {
	
}

struct TileSafe {
	bool operator() (Tile* tile, Map* map) const {
		if (tile->getTicksToExplosion() == 0 && !tile->isDangerous())
			return true;
		return false;
	}
};

struct TileHasAWallNextToIt {
	bool operator() (Tile* tile, Map* map) const {
		for (auto& neighbour : map->getExistingNeighbourTiles(tile, true)) {
			if (neighbour->isWall() && !neighbour->isIndestructible()) {
				return true;
			}
		}
		return false;
	}
};

void Computer::update(Game* game) {
	Tile* currentTile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
	std::vector<Tile*> path;
	
	updateLogicTick++;
	bombCooldownTick--;

	//AI logical update
	//the AI decides on new target locations periodically with a slight randomness added.
	if (updateLogicTick > updateLogicActiveDuration) {
		updateLogicTick = 0;
		updateLogicActiveDuration = updateLogicDuration + Utils::getRandomIntNumberInRange(0, updateLogicRandomness);

		if (bombCooldownTick < 0) {
			bombCooldownTick = 0;
		}

		//AI information gathering ------------------------------------------------------------------

		//Create copy of the map for the AI to manipulate
		Map localMapCopy = *game->map;
		localMapCopy.getTileAtCoordinates(currentTile->getX(), currentTile->getY())->setBombermanPresent(false);
		
		std::multimap<float, Entity*> closestEntities;

		for (auto& entity : game->entityManager->getEntities()) {
			Entity* entityPtr = entity.get();

			float dist = Utils::distance(currentTile->getCenterX(), currentTile->getCenterY(), entityPtr->getCenterX(), entityPtr->getCenterY());
			closestEntities.insert(std::pair<float, Entity*>(dist, entityPtr));
		}

		std::vector<Tile*> closestWalls;
		std::vector<Tile*> closestTilesNotInDanger;
		std::vector<Tile*> closestPlayerTiles;
		std::vector<Tile*> closestPowerUpTiles;

		for (auto itr = closestEntities.begin(); itr != closestEntities.end(); itr++) {
			Entity* entityPtr = itr->second;
			if (Tile* tile = dynamic_cast<Tile*>(entityPtr)) {
				if (tile->isWall() && !tile->isIndestructible()) {
					closestWalls.push_back(tile);
				}
				if (!tile->isWall() && tile->getTicksToExplosion() <= 0 && !tile->isDangerous()) {
					closestTilesNotInDanger.push_back(tile);
				}
			}
			if (Player* player = dynamic_cast<Player*>(entityPtr)) {
				if (itr->first < visionRadius * Constants::TILE_SIZE) {
					closestPlayerTiles.push_back(game->map->getTileAtCoordinates(player->getCenterX(), player->getCenterY()));
				}
			}
			if (PowerUp* powerup = dynamic_cast<PowerUp*>(entityPtr)) {
				if (itr->first < visionRadius * Constants::TILE_SIZE) {
					if (!powerup->isNegative()) {
						closestPowerUpTiles.push_back(game->map->getTileAtCoordinates(powerup->getCenterX(), powerup->getCenterY()));
					}
				}
			}
		}

		//AI decision making ------------------------------------------------------------------------		

		bool playerSpotted = false;
		bool nearPlayer = false;
		
		//A target tile to place a bomb to damage a player
		Tile* playerBombTarget = nullptr;
		
		//If a player is close, check if he is reachable
		if (!closestPlayerTiles.empty()) {
			auto playerPath = findAccessibleTile(currentTile, closestPlayerTiles, *game->map);			
			if (!playerPath.empty()) {
				playerSpotted = true;
				
				//Player is reachable, pick a random tile that is on the current player path and also near enough to the player
				//that a bomb with the AI current bomb power level would damage the player.
				
				std::vector<Tile*> potentialBombTiles;
				auto tilesAroundPlayer = getTilesAroundPlayer(playerPath[0], game->map);

				int tmp = this->bombPowerLevel;
				if (static_cast<int>(playerPath.size()) < bombPowerLevel) {
					tmp = static_cast<int>(playerPath.size());
				}

				for (int i = 0; i < tmp; i++) {
					for (auto &tileAroundPlayer : tilesAroundPlayer) {
						if (playerPath[i]->equals(tileAroundPlayer)) {
							potentialBombTiles.push_back(playerPath[i]);
						}
						if (currentTile->equals(tileAroundPlayer)) {
							if (Utils::getRandomIntNumberInRange(0, 100) >= 50) {
								nearPlayer = true;
							}
						}
					}
				}

				if (!potentialBombTiles.empty()) {
					playerBombTarget = potentialBombTiles[Utils::getRandomIntNumberInRange(0, static_cast<int>(potentialBombTiles.size()) - 1)];
				}				
			}
		}

		//if ((currentTile->isDangerous() || currentTile->getTicksToExplosion() > 0) && Utils::getRandomIntNumberInRange(0, 100) >= ignoreDangerChance) {

		if ((currentTile->isDangerous() || currentTile->getTicksToExplosion() > 0)) {
			//If the current tile is dangerous, try moving out of the way, there is a chance that the AI will ignore danger
			if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Moving to a safe spot" << std::endl;
			Tile* newSafeTile = tileBreadthFirstSearch<TileSafe>(currentTile, game->map, TileSafe());
			bombExitTile = newSafeTile;
			targetTile = newSafeTile;
		} else
		if (bombCooldownTick == 0) {
			//Check for powerups nearby
			bool powerUpFound = false;
			if (!closestPowerUpTiles.empty() && !playerBombTarget) {
				auto powerupPath = findAccessibleTile(currentTile, closestPowerUpTiles, *game->map);
				if (!powerupPath.empty()) {
					if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Getting a power up" << std::endl;
					targetTile = powerupPath[0];
					powerUpFound = true;
				}
			}

			if (!powerUpFound) {
				//If no bombs are placed, find a spot to place a bomb, either next to a wall or a player.
				std::vector<Tile*> newTargetPath;
				Tile* newTargetTile = nullptr;

				//Selecting target tile
				if (playerBombTarget) {
					//There is a reachable player nearby
					//If a tile that could damage a player was found, consider placing a bomb there
					
					if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Targeting player" << std::endl;
					newTargetPath = getPathAstar(currentTile, playerBombTarget, *game->map);
					newTargetTile = playerBombTarget;
				} else {
					//Search for an accessible wall
					//No players nearby, find a wall and destroy it safely
					
					if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Searching for a wall" << std::endl;
					newTargetTile = tileBreadthFirstSearch<TileHasAWallNextToIt>(currentTile, &localMapCopy, TileHasAWallNextToIt());
					if (newTargetTile) {
						newTargetPath = getPathAstar(currentTile, newTargetTile, *game->map);
					}
				}

				//Check, that if the AI was to place a bomb on the target tile, that the AI could escape its blast
				if (newTargetTile && (!newTargetPath.empty() || currentTile->equals(newTargetTile))) {
					Tile* potentialBombTile = newTargetTile;

					simulateBombExplosionOnMap(potentialBombTile, this->bombPowerLevel, Constants::DEFAULT_BOMB_PENETRATION, &localMapCopy);

					Tile* potentialExitTile = tileBreadthFirstSearch<TileSafe>(potentialBombTile, &localMapCopy, TileSafe());

					if (potentialExitTile) {
						//The bomb is safe to place, either place it or move towards the target tile
						if (currentTile->equals(potentialBombTile) || (playerSpotted && nearPlayer)) {
							//Wall reached, place down bomb
							if (playerSpotted) {
								if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Placed bomb next to a player" << std::endl;
							} else {
								if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Placed bomb next to a wall" << std::endl;
							}

							this->placeDownBombOnUpdate = true;
							this->bombCooldownTick = bombCooldown;

							//Potential exit and bomb tiles are owned by the local map copy, it is required to find the equivalent game map tiles
							this->bombExitTile = game->map->getTileAtCoordinates(potentialExitTile->getX(), potentialExitTile->getY());
							this->bombPlacedTile = game->map->getTileAtCoordinates(potentialBombTile->getX(), potentialBombTile->getY());
						} else {
							//Move to the wall
							targetTile = newTargetPath[0];
						}
					}
				}
			}
		} else {
			//A bomb is placed, move towards the calculated bomb blast exit tile
			if (bombExitTile && bombPlacedTile) {
				if (currentTile->equals(bombExitTile)) {
					//Currently standing on the planned bomb exit tile
					if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Waiting for bomb explosion" << std::endl;
					//This tile is now dangerous, find a new one
					if (currentTile->isDangerous() || currentTile->getTicksToExplosion() > 0) {
						if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Searching for a new safe spot" << std::endl;
						Tile* newSafeTile = tileBreadthFirstSearch<TileSafe>(currentTile, game->map, TileSafe());
						bombExitTile = newSafeTile;
						targetTile = newSafeTile;
					}
				} else {
					//Retreat from a bomb
					if (Game::showAI) std::cout << game->getTickString() << "AI " << id << " - Retreating from own bomb" << std::endl;
					targetTile = bombExitTile;
				}
			}
		}
	}

	//AI movement -----------------------------------------------------------------------------------
	
	//If a target tile was decided on by the logic update, move towards it
		//otherwise just stand still
	if (currentTile && targetTile) {
		path = getPathAstar(currentTile, targetTile, *game->map);
		if (!path.empty()) {
			Tile* nextTile = path.back();
			moveTowardsTile(nextTile);
		} else {
			moveTowardsTile(currentTile);
		}
	} else {
		if (currentTile)
			moveTowardsTile(currentTile);
		else
			moveStop();
	}
	
	//AI path visualisation, hella ineffective but hey, its a debug thing
	if (Game::showAI) {
		for (auto& tile : path) {
			game->entityManager->addEntity(new Particle(TextureManager::defaultTexture, 2, tile->getX(), tile->getY(), tile->getWidth(), tile->getHeight()));
		}
	}
	
	Bomberman::update(game);
}

int Computer::getID() const {
	return id;
}

void Computer::simulateBombExplosionOnMap(Tile* explosionTile, int bombPowerLevel, int bombPenetrationLevel, Map* map) {
	//Create bomb beams in all 4 major directions from the base tile.
	//The beams stop at the first indestructible wall or when they destroy as many walls as their penetration value.
	Tile* nextTile = map->getTileAtCoordinates(explosionTile->getX(), explosionTile->getY());
	nextTile->setBombPlaced(true);
	nextTile->setTicksToExplosion(Constants::BOMB_EXPLOSION_TIME + Constants::EXPLOSION_DURATION);
	for (int dir = 0; dir < 4; dir++) {
		int wallsDestroyed = 0;
		for (int i = 0; i < bombPowerLevel; i++) {
			switch (dir) {
				case 0:
					nextTile = map->getTileAbove(nextTile); break;
				case 1:
					nextTile = map->getTileLeft(nextTile); break;
				case 2:
					nextTile = map->getTileBelow(nextTile); break;
				case 3:
					nextTile = map->getTileRight(nextTile); break;
			}
			if (nextTile && !nextTile->isIndestructible() && wallsDestroyed < bombPenetrationLevel) {
				//If a wall was truly destroyed, increment the wallsDestroyed counter
				if (!nextTile->isIndestructible() && nextTile->isWall()) {
					wallsDestroyed++;
				}

				//Alert tile of bomb
				if (nextTile->getTicksToExplosion() < 1) {
					nextTile->setTicksToExplosion(Constants::BOMB_EXPLOSION_TIME + Constants::EXPLOSION_DURATION);
				}
			} else {
				break;
			}
		}
		nextTile = explosionTile;
	}
}

std::vector<Tile*> Computer::getTilesAroundPlayer(Tile* playerTile, Map* map) {
	std::vector<Tile*> explosionTiles;
	//Create bomb beams in all 4 major directions from the base tile.
	//The beams stop at the first indestructible wall or when they destroy as many walls as their penetration value.
	Tile* nextTile = map->getTileAtCoordinates(playerTile->getX(), playerTile->getY());
	
	for (int dir = 0; dir < 4; dir++) {
		int wallsDestroyed = 0;
		for (int i = 0; i < bombPowerLevel; i++) {
			switch (dir) {
				case 0:
					nextTile = map->getTileAbove(nextTile); break;
				case 1:
					nextTile = map->getTileLeft(nextTile); break;
				case 2:
					nextTile = map->getTileBelow(nextTile); break;
				case 3:
					nextTile = map->getTileRight(nextTile); break;
			}
			if (nextTile && !nextTile->isIndestructible() && wallsDestroyed < Constants::DEFAULT_BOMB_PENETRATION) {
				//If a wall was truly destroyed, increment the wallsDestroyed counter
				if (!nextTile->isIndestructible() && nextTile->isWall()) {
					wallsDestroyed++;
				}

				explosionTiles.push_back(nextTile);
			} else {
				break;
			}
		}
		nextTile = playerTile;
	}
	return explosionTiles;
}

std::vector<Tile*> Computer::findAccessibleTile(Tile* startingTile, std::vector<Tile*>& listOfTiles, Map& map) {
	return findAccessibleTile(startingTile, listOfTiles, map, false, 50, 1);
}

std::vector<Tile*> Computer::findAccessibleTile(Tile* startingTile, std::vector<Tile*>& listOfTiles, Map& map, bool introduceRandomness, int skipChance, int minRandomnessCount) {
	auto foundPath = std::vector<Tile*>();
	for (auto &tile : listOfTiles) {
		if (introduceRandomness && static_cast<int>(listOfTiles.size()) > minRandomnessCount) {
			if (Utils::getRandomIntNumberInRange(0, 100) > skipChance) {
				continue;
			}
		}

		tile->setOverrideWalkable(true);
		auto checkPath = getPathAstar(startingTile, tile, map);
		if (!checkPath.empty()) {
			foundPath = checkPath;
			tile->setOverrideWalkable(false);
			break;
		}
		tile->setOverrideWalkable(false);
	}
	return foundPath;
}

struct TileHeuristicCompare {
	bool operator()(const Tile* t1, const Tile* t2) const {
		return t1->pathfindingData.heuristic < t2->pathfindingData.heuristic;
	}
};

struct TileHash {
	std::size_t operator()(const Tile* tile) const {
		return (53 + tile->getX()) * 53 + tile->getY();
	}
};

struct TileEquals {
	bool operator () (Tile const* t1, Tile const* t2) const {
		return ((t1->getX() == t2->getX()) && (t1->getY() == t2->getY()));
	}
};

std::vector<Tile*> Computer::getPathAstar(Tile* start, Tile* finish, Map& map) {
	//Multiset functions as a priority queue ordered by tile pathfinding heuristic
	std::multiset<Tile*, TileHeuristicCompare> prioritySet;

	//Using an unordered set to check whether a tile has been already visited due to fast find() performance.
	std::unordered_set<Tile*, TileHash, TileEquals> alreadyVisited;

	//The returned path, the first path found is returned (the algorithm does not keep searching)
	std::vector<Tile*> path;
	bool pathFound = false;

	//Resets the tile distances and heuristics, distances and heuristics set to infinity (std::numeric_limits<float>::max() in this case)
	for (int y = 0; y < map.mapTileHeight; y++) {
		for (int x = 0; x < map.mapTileWidth; x++) {
			auto tile = map.getTiles()[x][y];
			//The start tile distance is 0 and the heuristic is the distance between start and end tile coordinates
			if (tile->equals(start)) {
				tile->pathfindingData.distance = 0;
				//Using a float as the heuristic value in case I wanted to switch to euclidean distance for example
				//Currently using manhattan distance because it is recommended for 4 directional grids
				tile->pathfindingData.heuristic = static_cast<float>(Utils::manhattanDistance(start->getX(), start->getY(), finish->getX(), finish->getY()));
			} else {
				tile->pathfindingData.distance = std::numeric_limits<float>::max();
				tile->pathfindingData.heuristic = std::numeric_limits<float>::max();
			}
		}
	}

	//Initial priority queue element
	prioritySet.insert(start);

	int iterations = 0; //For debug

	//Keep searching until you've run out of tiles or a path has been found
	while (!prioritySet.empty() || !pathFound) {
		//Getting the top set element (lowest heuristic)
		if (prioritySet.empty()) {
			break;
		}
		auto itr = prioritySet.begin();
		Tile* smallest = *itr;
		prioritySet.erase(itr);

		iterations++;

		//Mark the dequeued as already visited
		alreadyVisited.insert(smallest);

		//If the top tile in the set is the target tile, start tracing back the path, break the outer loop and end the search.
		if (smallest->equals(finish)) {
			pathFound = true;
			while (smallest != start) {
				path.push_back(smallest);
				smallest = smallest->pathfindingData.previous;
			}
			break;
		}

		if (smallest->pathfindingData.distance == std::numeric_limits<float>::max()) {
			break;
		}

		//Get viable neighbours in 4 major directions
		auto neighbours = map.getWalkableNeighbourTiles(smallest, true);
		for (auto& neighbour : neighbours) {
			//Check if this tile was not visited already (if so, skip it) this should prevent looping (and thus undefined behaviour)
			if (alreadyVisited.find(neighbour) != alreadyVisited.end()) {
				continue;
			}

			//A* heuristic, heurDist = current chain step count + weight 1 (because the grid is uniform) + distance from the target to the current tile
			const float heurDist = smallest->pathfindingData.distance + 1 + Utils::manhattanDistance(neighbour->getX(), neighbour->getY(), finish->getX(), finish->getY());
			const float dist = smallest->pathfindingData.distance + 1;

			if (dist < neighbour->pathfindingData.distance) {
				auto neighbourItr = prioritySet.find(neighbour);

				neighbour->pathfindingData.distance = dist;
				neighbour->pathfindingData.heuristic = heurDist;
				neighbour->pathfindingData.previous = smallest;

				if (neighbourItr != prioritySet.end()) {
					//Update the tile heuristic
					prioritySet.erase(neighbourItr);
					prioritySet.insert(neighbour);
				} else {
					prioritySet.insert(neighbour);
				}
			}
		}
	}

	//std::cout << "A* iterations: " << iterations << std::endl;
	return path;
}

template <typename ConditionFunction>
Tile* Computer::tileBreadthFirstSearch(Tile* startingTile, Map* map, ConditionFunction targetTileCondition) {
	std::list<Tile*> queue;
	//Resets the visited state of tiles
	for (int y = 0; y < map->mapTileHeight; y++) {
		for (int x = 0; x < map->mapTileWidth; x++) {
			auto tile = map->getTiles()[x][y];
			tile->pathfindingData.visited = false;
		}
	}
	startingTile = map->getTileAtCoordinates(startingTile->getX(), startingTile->getY());
	startingTile->pathfindingData.visited = true;
	queue.push_back(startingTile);

	while (!queue.empty()) {
		Tile* currentTile = queue.front();
		queue.pop_front();
		if (targetTileCondition(currentTile, map)) {
			return currentTile;
		}
		auto neighbours = map->getWalkableNeighbourTiles(currentTile, true);
		std::shuffle(neighbours.begin(), neighbours.end(), randomNumberGenerator); //Shuffling neighbours to eliminate direction bias
		for (auto &neighbour : neighbours) {
			if (!neighbour->pathfindingData.visited) {
				neighbour->pathfindingData.visited = true;
				queue.push_back(neighbour);
			}
		}
	}
	return nullptr;
}

void Computer::moveTowardsTile(Tile* tile) {
	int movementX = 0;
	int movementY = 0;
	
	if (abs(this->getCenterX() - tile->getCenterX()) > 2) {
		if (this->getCenterX() > tile->getCenterX()) {
			movementX = -1;
		} else {
			movementX = 1;
		}
	}

	if (abs(this->getCenterY() - tile->getCenterY()) > 2) {
		if (this->getCenterY() > tile->getCenterY()) {
			movementY = -1;
		} else {
			movementY = 1;
		}
	}
	
	this->movementDirection.x = movementX;
	this->movementDirection.y = movementY;
}


void Computer::moveStop() {
	this->movementDirection.x = 0;
	this->movementDirection.y = 0;
}


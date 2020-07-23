#pragma once

#include <random>

#include "Bomberman.h"

class Map;
class Tile;

/*!
 * A bomberman controlled by the computer.
 */
class Computer : public Bomberman {
private:
	int id;
	
	/*! Detection radius for players. */
	int visionRadius = 8;

	bool getTheClosestPowerup = false;
	
	/*! Time the AI waits for upon placing a bomb. */
	int bombCooldown = Constants::BOMB_EXPLOSION_TIME;
	int bombCooldownTick = 0;

	/*! Interval between AI logic updates. */
	int updateLogicDuration = 20;
	/*! Max random value added to the updateLogicDuration. */
	int updateLogicRandomness = 10;
	int updateLogicActiveDuration = updateLogicDuration;
	int updateLogicTick = 0;

	/*! Tile the AI moves towards. */
	Tile* targetTile = nullptr;

	Tile* bombPlacedTile = nullptr;
	Tile* bombExitTile = nullptr;
	
	std::mt19937 randomNumberGenerator;
public:
	/*!
	 * Creates a computer controlled bomberman with specified coordinates.
	 * \param index Number used to distinguish between other computers
	 */
	Computer(int id, int initialX, int initialY);

	void update(Game* game) override;

	int getID() const;
private:
	/*!
	 * Simulates effects of a bomb explosion on tiles of a map.
	 * 
	 * \param explosionTile Explosion centre tile.
	 * \param bombPowerLevel Power level of the bomb.
	 * \param bombPenetrationLevel Penetration level of the bomb.
	 * \param map The map.
	 */
	void simulateBombExplosionOnMap(Tile* explosionTile, int bombPowerLevel, int bombPenetrationLevel, Map* map);

	/*!
	 * Returns a list of tiles around the player that are within "beams" projected
	 * from the player with the current Bomberman::bombPowerLevel length.
	 * 
	 * \param playerTile The tile the player is standing on.
	 * \param map The map.
	 */
	std::vector<Tile*> getTilesAroundPlayer(Tile* playerTile, Map* map);

	/*!
	 * Searches for the first tile to which a path can be found in the given list.
	 * An empty path is returned when no path can be found to any of the tiles in the list.
	 * This operation is very performance demanding if there are potential targets.
	 * 
	 * \param startingTile Tile of the path start.
	 * \param listOfTiles List of potential path targets.
	 * \param map The map.
	 * \return The path as returned from Astar pathfinding.
	 * \see getPathAstar()
	 */
	std::vector<Tile*> findAccessibleTile(Tile* startingTile, std::vector<Tile*>& listOfTiles, Map& map);

	/*!
	 * Searches for the first tile to which a path can be found in the given list.
	 * An empty path is returned when no path can be found to any of the tiles in the list.
	 * This operation is very performance demanding if there are potential targets.
	 * Some targets may get skipped based on a random chance.
	 *
	 * \param startingTile Tile of the path start.
	 * \param listOfTiles List of potential path targets.
	 * \param map The map.
	 * \param introduceRandomness
	 * \param skipChance
	 * \param minRandomnessCount
	 * \return The path as returned from Astar pathfinding.
	 * \see getPathAstar()
	 */
	std::vector<Tile*> findAccessibleTile(Tile* startingTile, std::vector<Tile*>& listOfTiles, Map& map, bool introduceRandomness, int skipChance, int minRandomnessCount);

	/*!
	 * Returns the shortest path between the two tiles using the A* pathfinding algorithm.
	 *
	 * \param start Start tile
	 * \param finish Target tile
	 * \return A list containing a sequence of tiles forming the path. The last element of the list is the first tile of the path.
	 * \see Map::getWalkableNeighbourTiles()
	 */
	std::vector<Tile*> getPathAstar(Tile* start, Tile* finish, Map& map);

	
	/*!
	 * Returns the first tile that fulfills a condition found by a breadth first search algorithm
	 * starting from the starting tile. The algorithm searches walkable tiles.
	 *
	 * The condition can be specified like so:
	 * \code{.cpp}
	 	bool operator() (Tile* tile, Map* map) const {
			if (tile->isWall()) {
				return true;
			}
			return false;
		}
	 * \endcode 
	 * \param startingTile The start tile of the search.
	 * \param map The map.
	 * \param targetTileCondition A condition that the tile needs to fulfill.
	 * \return The found tile or a nullptr if nothing was found.
	 * \see Map::getWalkableNeighbourTiles()
	 */
	template<typename ConditionFunction>
	Tile* tileBreadthFirstSearch(Tile* startingTile, Map* map, ConditionFunction targetTileCondition);

	/*!
	 * Moves the AI towards the center of a tile.
	 */
	void moveTowardsTile(Tile* tile);

	/*!
	 * Sets the AI movement direction to 0
	 */
	void moveStop();
};
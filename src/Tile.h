#pragma once

#include "TilePathfindingData.h"

#include "Entity.h"

class EntityManager;
class Map;

/*!
 * A tile of the map. A tile can be empty or function as a wall.
 */
class Tile : public Entity {
private:
	/*! Whether this tile acts as a solid wall rather than empty space. */
	bool wall = false;

	/*! Whether the wall of this tile is invulnerable to bomb explosions. */
	bool indestructible = false;
	
	/*! Determines if this wall acts as an map edge wall. Currently just changes its texture. */
	bool edgeWall = false;

	/*! Utility flag for enabling pathfinding algorithms to ignore walls of this tile. */
	bool overrideWalkable = false;
	
	/*! Whether a bomb is placed on this tile. */
	bool bombPlaced = false;

	/*! Whether a bomberman is on this tile. */
	bool bombermanPresent = false;

	/*!
	 * Game ticks remaining until a bomb explodes.
	 * Tick of 0 would mean that no bombs are currently about to explode near it.
	 */
	int ticksToExplosion = 0;
public:
	Tile(int initialX, int initialY);

	/*!
	 * \see TilePathfindingData
	 */
	TilePathfindingData pathfindingData;

	void update(Game* game) override;
	void render(SDL_Renderer* renderer) override;
	
	/*!
	 * Destroys the wall of the tile if a wall is present.
	 * \return true if a wall was truly destroyed, false if not.
	 */
	bool destroyWall(Game* game);

	/*!
	 * Whether this tile is safe to stand on.
	 * Used by the bomberman AI.
	 */
	bool isDangerous() const;

	/*!
	 * Compares two tiles based on their coordinates.
	 *
	 * \return true if their coordinates are the same.
	 */
	bool equals(Tile* tile) const;
private:
	/*!
	 * Creates a random powerup at the tile location.
	 */
	void createPowerup(Game* game) const;
public:
	bool isWall() const;
	void setWall(bool wall);

	bool isIndestructible() const;
	void setIndestructible(bool indestructible);

	bool isEdgeWall() const;
	void setEdgeWall(bool edgewall);

	bool isOverrideWalkable() const;
	void setOverrideWalkable(bool overrideWalkable);

	bool isBombPlaced() const;
	void setBombPlaced(bool bombPlaced);

	int getTicksToExplosion() const;
	void setTicksToExplosion(int ticksToExplosion);

	bool isBombermanPresent() const;
	void setBombermanPresent(bool bombermanPresent);
};

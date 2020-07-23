#pragma once

#include "Tile.h"

#include <vector>
#include <memory>

struct MapData;

class EntityManager;

/*!
 * Map holds and retains information about the world area in the form of a tile grid.
 * Handles the map rendering but registers tiles in the entity manager for updates.
 */
class Map {
private:
	std::vector<std::vector<std::shared_ptr<Tile>>> tiles;
public:
	/* Map width in game units (pixels). */
	int mapWidth{};
	/* Map height in game units (pixels). */
	int mapHeight{};
	/* Tile width/height in game units (pixels). */
	int tileSize{};
	/* A number of tiles on the map X axis. */
	int mapTileWidth{};
	/* A number of tiles on the map Y axis. */
	int mapTileHeight{};

	/*!
	 * Creates a map object.
	 * The map needs to be generated before use.
	 *
	 * \see generate()
	 * \see generateFromMapData()
	 */
	Map();

	/*!
	 * Creates a map object as a copy of an existing one.
	 */
	Map(const Map &map);

	Map& operator= (const Map& map);
	
	/*!
	 * Generates a default map.
	 */
	void generate(int pMapWidth, int pMapHeight, Game* game);

	/*!
	 * Generates a map from file.
	 * 
	 * \return Returns true on success, false on failure.
	 */
	bool generateFromMapData(MapData* mapData, Game* game);

	/*!
	 * Renders the map tiles
	 */
	void render(SDL_Renderer* renderer);

	/*!
	 * Get tile that is located at game coordinates.
	 *
	 * \return A pointer to the tile.
	 */
	Tile* getTileAtCoordinates(int x, int y) const;

	/*!
	 * Get tile by its index in the tile array.
	 * Tiles are indexed from 0.
	 *
	 * \return A pointer to the tile.
	 */
	Tile* getTileAtIndexes(int x, int y) const;
	
	Tile* getTileAbove(Tile* tile) const;
	Tile* getTileLeft(Tile* tile) const;
	Tile* getTileRight(Tile* tile) const;
	Tile* getTileBelow(Tile* tile) const;
	Tile* getTileTopLeftCorner(Tile* tile) const;
	Tile* getTileBottomLeftCorner(Tile* tile) const;
	Tile* getTileBottomRightCorner(Tile* tile) const;
	Tile* getTileTopRightCorner(Tile* tile) const;

	/*!
	 * Returns a vector containing 8 neighbour tiles starting from the tile above, continuing counter-clockwise.
	 * Tile that do not exists are represented with a nullptr.
	 */
	std::vector<Tile*> getNeighbourTiles(Tile* tile) const;

	/*!
	 * Returns a vector containing 0 to 8 neighbour (or 0 to 4 neighbours if ignoreCorners is true) tiles starting from the tile above, continuing counter-clockwise.
	 * Tile that do not exist are ommitted.
	 */
	std::vector<Tile*> getExistingNeighbourTiles(Tile* tile, bool ignoreCorners) const;

	/*!
	 * Returns a vector containing 0 to 8 neighbour (or 0 to 4 neighbours if ignoreCorners is true) tiles starting from the tile above, continuing counter-clockwise.
	 * Only existing tiles that are not a wall or dangerous are included.
	 *
	 * \see Tile::isDangerous()
	 */
	std::vector<Tile*> getWalkableNeighbourTiles(Tile* tile, bool ignoreCorners) const;

	
	/*!
	 * Returns a reference to the internal 2D tile vector.
	 */
	const std::vector<std::vector<std::shared_ptr<Tile>>>& getTiles() const;
private:
	/*!
	 * Creates the appropriate tile specified by the character at the x,y coordinates.
	 * \param x Map x coordinate
	 * \param y Map y coordinate
	 * \param c Tile character
	 * \param game The game
	 * \throws std::runtime_error
	 */
	void parseCharacterAndLoadTile(const int x, const int y, const char c, Game* game);

	/*!
	 * Initialises tiles with empty pointers
	 */
	void fillWithEmptyPointers();
};
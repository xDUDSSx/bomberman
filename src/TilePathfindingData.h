#pragma once

#include <limits>

class Tile;

/*!
 * A container struct holding tile pathfinding metadata used by the astar and BFS pathfinding algorithms.
 */
struct TilePathfindingData {
public:
	float distance = std::numeric_limits<float>::max();
	float heuristic = std::numeric_limits<float>::max();
	Tile* previous = nullptr;
	bool visited = false;
};

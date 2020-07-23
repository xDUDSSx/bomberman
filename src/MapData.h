#pragma once

#include <string>
#include <vector>

/*!
 * A container struct holding data about a loaded map file.
 */
struct MapData {
	std::vector<std::string> lines{};
	int tileWidth{};
	int tileHeight{};
	int pixelWidth{};
	int pixelHeight{};
};

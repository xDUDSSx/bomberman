#include "Game.h"

#include <iostream>

/*!
 * Project main method
 */
int main(int argc, char* argv[]) {
	//Loading config argument
	char* configPath = nullptr;
	if (argc >= 2) {
		if (argc == 2) {
			configPath = argv[1];
		} else {
			std::cerr << "Invalid arguments! Usage: <executable name> <config file path>" << std::endl;
			return 1;
		}
	}

	//Starting the game
	std::unique_ptr<Game> game = std::make_unique<Game>();
	if (!game->run(configPath)) {
		return 1;
	}
	
	return 0;
}
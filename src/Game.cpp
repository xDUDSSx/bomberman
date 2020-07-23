#include "Game.h"

#include "TextureManager.h"
#include "FileUtils.h"
#include "Constants.h"
#include "MapData.h"
#include "Map.h"
#include "Computer.h"
#include "Tile.h"
#include "Player.h"
#include "EntityManager.h"
#include "GameInfoBar.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>

#include <SDL_image.h>

#include "Bomb.h"

//TODOs:
//
//DONE: Bomb explosion stops at the first regular wall
//DONE: Bomberman health management
//DONE: Explosion damage bombermans
//DONE: Bomberman damage concussion effect
//DONE: Fix powerup chances
//DONE: Added sprite scaling effects
//DONE: Wall destruction animation
//DONE: Bomb ticking effect
//DONE: Bomb explosion animations
//DONE: Bombs trigger other bombs
//DONE: Fix overlapping bomb explosion lag
//DONE: Add render layer functionality to entity manager
//DONE: Make explosions merge together (fixed via render layers)
//DONE: Make sprite effects easier to use
//DONE: Make powerups expire
//DONE: Add the blinking effect to sprite class
//DONE: Add an option to specify sprite effect duration
//DONE: Add health pickup
//DONE: Optimise bomb explosion damage via damage colliders
//DONE: Optimise bombs triggering other bombs via damage colliders
//DONE: Movement speed powerup
//DONE: Computer AI <------------
//DONE: Highscore system <-----------
//TODO: Add fps monitor and graph
//DONE: Move rendering into an offscreen buffer
//DONE: Some basic UI elements
//TODO: Bomberman death animation?
//TODO: Possibly bomb batting
//TODO: Add sound effects
//DONE: Winning conditions <-----------
//DONE: Player/computer management and indicator <----------
//DONE: Config loading <---------
//DONE?: Improve AI
//DONE: Finish docs
//DONE: Review makefile
//DONE: Make AI pickup powerups
//TODO: Investigate AI behaviour where no walls are remaining, only players

bool Game::debug = false;
bool Game::drawWireframes = false;
bool Game::slowMo = false;
bool Game::showAI = false;

Game::Game() = default;

Game::~Game() {
	dispose();
}

bool Game::run(const char* configFile) {
	if (configFile) {
		try {
			loadConfigFile(configFile);
		} catch (const std::runtime_error& error) {
			std::cerr << "Failed to load configuration file! ";
			std::cerr << error.what() << std::endl;
			return false;
		}
	} else {
		std::cout << "Using default config" << std::endl;
	}
	if (!init()) {
		std::cerr << "Failed to initialise the game!" << std::endl;
		return false;
	}
	
	gameloop();
	return true;
}

void Game::registerPlayer(char playerId, Tile* tile) {
	if (players.size() < Constants::PLAYER_MAX_COUNT) {
		if (computers.size() + players.size() < Constants::PLAYER_OR_COMPUTER_MAX_COUNT) {
			std::shared_ptr<Player> newPlayer;
			switch (playerId) {
				case 'A':
					newPlayer = std::make_shared<Player>(
						Player(playerId, tile->getX(), tile->getY(),
							   SDL_SCANCODE_SPACE,
							   SDL_SCANCODE_W, SDL_SCANCODE_A,
							   SDL_SCANCODE_S, SDL_SCANCODE_D
						)
					);
					break;
				case 'B':
					newPlayer = std::make_shared<Player>(
						Player(playerId, tile->getX(), tile->getY(),
							   SDL_SCANCODE_RETURN,
							   SDL_SCANCODE_UP, SDL_SCANCODE_LEFT,
							   SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT
						)
					);
					break;
				default:
					std::ostringstream oss;
					oss << "Invalid playerID (" << playerId << ") registered!" << std::endl;
					throw std::runtime_error(oss.str());
			}
			std::string playerName;
			if (playerNames.find(playerId) != playerNames.end()) {
				playerName = playerNames[playerId];
				newPlayer->setName(playerName);
			}
			entityManager->addEntity(newPlayer);
			players.push_back(std::shared_ptr<Player>(newPlayer));

			if (playerName.empty()) {
				std::cout << "Player " << playerId << " registered at " << tile->getX() << ":" << tile->getY() << std::endl;
			} else {
				std::cout << "Player " << playerName << "(" << playerId << ") registered at " << tile->getX() << ":" << tile->getY() << std::endl;
			}
		} else {
			std::ostringstream oss;
			oss << "Too many players/computers in the map! There can only be a maximum of " << Constants::PLAYER_OR_COMPUTER_MAX_COUNT << " players/computers in the map." << std::endl;
			throw std::runtime_error(oss.str());
		}
	} else {
		std::ostringstream oss;
		oss << "Too many players in the map! There can only be a maximum of " << Constants::PLAYER_MAX_COUNT << " players in the map." << std::endl;
		throw std::runtime_error(oss.str());
	}
}

void Game::registerComputer(Tile* tile) {
	if (computers.size() + players.size() < Constants::PLAYER_OR_COMPUTER_MAX_COUNT) {
		auto newComputer = std::make_shared<Computer>(Computer(static_cast<int>(computers.size()) + 1, tile->getX(), tile->getY()));
		entityManager->addEntity(newComputer);
		computers.push_back(std::shared_ptr<Computer>(newComputer));
		std::cout << "Computer " << newComputer->getID() << " registered at " << tile->getX() << ":" << tile->getY() << std::endl;
	} else {
		std::ostringstream oss;
		oss << "Too many computers in the map! There can only be a maximum of " << Constants::PLAYER_OR_COMPUTER_MAX_COUNT << " players or computers in the map." << std::endl;
		throw std::runtime_error(oss.str());
	}
}

bool Game::init() {
	std::cout << "Initialising game" << std::endl;

	//Initialising srand with the current time
	srand(static_cast<unsigned int>(time(nullptr)));
	
	int windowWidth;
	int windowHeight;

	//Pre-load map to get window dimensions
	std::unique_ptr<MapData> mapData;
	if (useCustomMap) {
		try {
			mapData = loadMapFile(customMapFile.c_str());
		} catch (const std::runtime_error& error) {
			std::cerr << "Failed to load map file! ";
			std::cerr << error.what() << std::endl;
			return false;
		}
		windowTitle = std::string("Bomberman - ") + customMapFile;
		gameDisplayRect.w = mapData->pixelWidth;
		gameDisplayRect.h = mapData->pixelHeight;
	} else {
		gameDisplayRect.w = 512 + 32 + 64;
		gameDisplayRect.h = gameDisplayRect.w;
	}

	//Resolve game display and infobar positions
	gameDisplayRect.x = 0;
	gameDisplayRect.y = infoBarHeight;
	windowWidth = gameDisplayRect.w;
	windowHeight = gameDisplayRect.h + infoBarHeight;

	//Initialise SDL
	if (!initialiseSDL(windowTitle.c_str(), windowHorizontalPos, windowVerticalPos, windowWidth, windowHeight, windowFullscreen)) {
		return false;
	}

	//Load font
	if (!loadFonts()) {
		return false;
	}
	
	//Load textures
	TextureManager::loadAllTextures(renderer);

	//Create display buffer
	gameDisplay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gameDisplayRect.w, gameDisplayRect.h);

	//Create the game info bar
	infoBar = new GameInfoBar(0, 0, gameDisplayRect.w, infoBarHeight);
	
	entityManager = new EntityManager();
	map = new Map();

	//Generate map
	if (useCustomMap) {
		//Custom map
		if (!map->generateFromMapData(mapData.get(), this)) {
			return false;
		}
	} else {
		//Default map
		map->generate(gameDisplayRect.w, gameDisplayRect.h, this);
	}

	//Test code that just places bombs everywhere
	/*for (int y = 0; y < map->mapTileHeight; y++) {
		for (int x = 0; x < map->mapTileWidth; x++) {
			if (Tile* t = map->getTileAtIndexes(x, y)) {
				entityManager->addEntity(new Bomb(3, t->getX(), t->getY()));
			}
		}
	}*/
	
	return true;
}

bool Game::loadFonts() {
	font = TTF_OpenFont("slkscr.ttf", fontSize);
	if (!font) {
		std::cerr << "Failed to load slkscr font!" << std::endl;
		std::cerr << TTF_GetError() << std::endl;
		return false;
	}
	smallerFont = TTF_OpenFont("slkscr.ttf", smallerFontSize);
	if (!smallerFont) {
		std::cerr << "Failed to load slkscr smaller font!" << std::endl;
		std::cerr << TTF_GetError() << std::endl;
		return false;
	}
	return true;
}

void Game::update() {
	if (!endOfGame) {
		tick++;

		entityManager->updateEntities(this);

		if (debug) {
			debugInfoTick++;
			if (debugInfoTick >= debugInfoTickThreshold) {
				std::cout << getTickString() << "INFO: Number of entities: " << entityManager->getEntities().size() << std::endl;
				debugInfoTick = 0;
			}
		}
	}
	
	//Check winning conditions
	bool gameEnded = endOfGame;
	int playersAlive = 0;
	int computersAlive = 0;
	for (auto& player : players) {
		if (player->getHealth() > 0) {
			playersAlive++;
		}
	}
	for (auto& computer : computers) {
		if (computer->getHealth() > 0) {
			computersAlive++;
		}
	}
	
	if (players.size() > 0) {
		if (players.size() == 1) {
			if (computers.size() > 0) {
				if (playersAlive < 1) {
					//End of game, player loses
					handleEndOfGame("Game Over");
				} else {
					if (computersAlive < 1) {
						//End of game, player wins
						handleEndOfGame("You win");
					}
				}
			}
			//No computers to play against, sandbox
		} else {
			//More than 1 player
			if (playersAlive == 1 && computersAlive == 0) {
				//End of game, player alive wins
				std::string name;
				for (auto& player : players) {
					if (player->getHealth() > 0) {
						name = player->getNameOrId();
					}
				}
				if (!name.empty()) {
					std::ostringstream oss;
					oss << "Player " << name << " wins!";
					handleEndOfGame(oss.str());
				} else {
					handleEndOfGame("Player wins");
				}
			} else {
				if (playersAlive < 1) {
					//End of game, all players are dead, computer/s win
					handleEndOfGame("Game over!");
				}
			}
		}
	} else {
		if (computersAlive == 1) {
			//End of game, The computer left alive wins
			handleEndOfGame("Computer wins");
		}
	}

	if (gameEnded != endOfGame) {
		try {
			loadHighscores(highscoreFile.c_str());
		} catch (std::runtime_error& error) {
			std::cout << "Cannot load old highscore file (" << error.what() << ")" << std::endl;
		}
		std::cout << "Generating highscore file" << std::endl;
		generateHighscoreFile(highscoreFile.c_str());
		std::cout << "Game ended! Press ESC to quit." << std::endl;
	}
}

void Game::render() const {
	SDL_SetRenderTarget(renderer, gameDisplay);
	SDL_RenderClear(renderer);

	map->render(renderer);
	entityManager->renderEntities(renderer);
	
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, gameDisplay, NULL, &gameDisplayRect);

	infoBar->render(renderer, this);

	if (endOfGame) renderEndOfGame();
	
	SDL_RenderPresent(renderer);
}

void Game::handleEvents() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				handleKeyDown(&event.key, keys);
				break;
			case SDL_KEYUP:
				handleKeyUp(&event.key, keys);
				break;
			default:
				break;
		}
	}
	handleKeyHeldDown(keys);
}

void Game::handleKeyDown(SDL_KeyboardEvent* event, const Uint8 * keys) {
	if (event->repeat == 0) {
		if (event->keysym.scancode == SDL_SCANCODE_ESCAPE) {
			isRunning = false;
		}
		if (event->keysym.scancode == SDL_SCANCODE_F1) {
			drawWireframes = !drawWireframes;
		}
		if (event->keysym.scancode == SDL_SCANCODE_F4) {
			slowMo = !slowMo;
		}
		if (event->keysym.scancode == SDL_SCANCODE_F2) {
			showAI = !showAI;
		}
		if (event->keysym.scancode == SDL_SCANCODE_F3) {
			debug = !debug;
		}
	}

	for (auto& player : players) {
		player->handleKeyDown(event);
	}
}

void Game::handleKeyUp(SDL_KeyboardEvent* event, const Uint8 * keys) {
	for (auto& player : players) {
		player->handleKeyUp(event);
	}
}

void Game::handleKeyHeldDown(const Uint8* keys) {
	for (auto& player : players) {
		player->handleKeyHeldDown(keys);
	}
}

void Game::dispose() const {
	std::cout << "Calling dispose" << std::endl;

	//Freeing textures
	TextureManager::dispose();

	//Freeing game components
	delete infoBar;
	delete entityManager;
	delete map;

	//Freeing SDL_ttf
	if (TTF_WasInit() == 1) {
		TTF_CloseFont(font);
		TTF_CloseFont(smallerFont);
		TTF_Quit();
	}

	//Freeing SDL_image
	IMG_Quit();
	
	//Freeing SDL
	if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0) {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
	}
}

void Game::gameloop() {
	std::cout << "Starting gameloop" << std::endl;

	int frameDelay;
	Uint32 frameStart;
	int frameTime;

	while (isRunning) {
		frameStart = SDL_GetTicks();
		frameDelay = slowMo ? 1000 / Constants::SLOWMO_FPS : 1000 / Constants::TARGET_FPS;

		handleEvents();
		update();
		render();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void Game::handleEndOfGame(std::string message) {
	endOfGame = true;
	endOfGameMessage = message;
}

void Game::renderEndOfGame() const {
	SDL_Rect bannerRect{ gameDisplayRect.x, gameDisplayRect.y + gameDisplayRect.h / 2 - 20, gameDisplayRect.w, 40 };
	SDL_RenderFillRect(renderer, &bannerRect);

	SDL_Surface* endText = TTF_RenderText_Solid(font, endOfGameMessage.c_str(), fontColor);
	SDL_Texture* endTextTexture = SDL_CreateTextureFromSurface(renderer, endText);
	int textWidth = 0;
	int textHeight = 0;
	TTF_SizeText(font, endOfGameMessage.c_str(), &textWidth, &textHeight);
	SDL_Rect endTextTextureRect{ bannerRect.x + bannerRect.w /2 - textWidth/2, bannerRect.y + bannerRect.h/2 - textHeight/2,textWidth, textHeight };
	SDL_RenderCopy(renderer, endTextTexture, NULL, &endTextTextureRect);
	SDL_FreeSurface(endText);
	SDL_DestroyTexture(endTextTexture);
}

void Game::loadConfigFile(const char* configFile) {	
	std::cout << "Loading configuration file" << std::endl;
	std::string configContent = FileUtils::loadStringFromFile(configFile);
	std::istringstream contentStream(configContent);
	std::string line;
	int lineNumber = 0;
	while (std::getline(contentStream, line)) {
		std::string key;
		std::istringstream lineStream(line);
		if (std::getline(lineStream, key, '=')) {
			std::string value;
			if (std::getline(lineStream, value)) {
				loadConfigEntry(key, value);
			} else {
				std::cerr << "Failed to parse value at line " << lineNumber << std::endl;
			}
		} else {
			std::cerr << "Failed to parse key at line " << lineNumber << std::endl;
		}
		lineNumber++;
	}
}

void Game::loadConfigEntry(const std::string& key, const std::string& value) {
	if (key == "map") {
		std::cout << "Using custom map " << value << std::endl;
		this->useCustomMap = true;
		this->customMapFile = value;
	} else
	if (key == "A") {
		std::cout << "Player A = " << value << std::endl;
		playerNames.insert(std::pair<char, std::string>('A', value));
	} else
	if (key == "B") {
		std::cout << "Player B = " << value << std::endl;
		playerNames.insert(std::pair<char, std::string>('B', value));
	}
}

void Game::loadHighscores(const char* highscoreFile) {
	std::cout << "Loading highscores file" << std::endl;
	std::string highscoreContent = FileUtils::loadStringFromFile(highscoreFile);
	std::istringstream highscoreStream(highscoreContent);
	std::string line;
	int lineNumber = 0;
	while (std::getline(highscoreStream, line)) {
		std::string key;
		std::istringstream lineStream(line);
		if (std::getline(lineStream, key, '=')) {
			std::string value;
			if (std::getline(lineStream, value)) {
				loadHighscoreEntry(key, value);
			} else {
				std::cerr << "Failed to parse value at line " << lineNumber << std::endl;
			}
		} else {
			std::cerr << "Failed to parse key at line " << lineNumber << std::endl;
		}
		lineNumber++;
	}
}

void Game::loadHighscoreEntry(const std::string& keyName, const std::string& value) {
	try {
		int highscoreValue = std::stoi(value);
		highscores.push_back(std::pair<int, std::string>(highscoreValue, keyName));
	} catch (const std::invalid_argument& e) {
		std::cerr << e.what() << std::endl;
	} catch (const std::out_of_range& e) {
		std::cerr << e.what() << std::endl;
	}
}

void Game::generateHighscoreFile(const char* highscoreFile) {
	//Go through the highscores and update ones that exist
	for (auto& player : players) {
		bool found = false;
		if (!player->getName().empty()) {
			for (auto& pair : highscores) {
				if (pair.second == player->getName()) {
					if (pair.first < player->getScore()) {
						pair.first = player->getScore();
					}
					found = true;
				}
			}
			if (!found) {
				highscores.push_back(std::pair<int, std::string>(player->getScore(), player->getName()));
			}
		} else {
			for (auto& pair : highscores) {
				if (pair.second == "NONAME") {
					if (pair.first < player->getScore()) {
						pair.first = player->getScore();
					}
					found = true;
				}
			}
			if (!found) {
				highscores.push_back(std::pair<int, std::string>(player->getScore(), "NONAME"));
			}
		}
		
	}

	//Sort the highscores by score
	std::sort(highscores.begin(), highscores.end());

	//Write a new highscore file
	std::string output;
	for (int i = static_cast<int>(highscores.size()) - 1; i >= 0; i--) {
		const auto highscorePair = highscores[i];
		output += highscorePair.second;
		output += "=";
		output += std::to_string(highscorePair.first);
		output += "\n";
	}

	FileUtils::writeStringToFile(output, highscoreFile);
}

std::unique_ptr<MapData> Game::loadMapFile(const char* mapFile) const {
	std::unique_ptr<MapData> mapDataPtr = std::make_unique<MapData>(MapData());
	const std::string loadedMapString = FileUtils::loadStringFromFile(mapFile);

	//Splitting map into a vector of individual lines
	std::vector<std::string> lines;
	size_t maxWidth = 0;
	size_t index = 0;
	size_t prevIndex = 0;
	while ((index = loadedMapString.find('\n', prevIndex)) != std::string::npos) {
		std::string lineString = loadedMapString.substr(prevIndex, index - prevIndex);
		if (lineString.length() > maxWidth) {
			maxWidth = lineString.length();
		}
		lines.push_back(lineString);
		prevIndex = index + 1;
	}
	lines.push_back(loadedMapString.substr(prevIndex));

	if (lines.empty()) {
		throw std::runtime_error("Map file contains no lines!");
	}

	mapDataPtr->lines = lines;
	
	//Calculating size
	mapDataPtr->tileWidth = static_cast<int>(lines[0].length());
	mapDataPtr->tileHeight = static_cast<int>(lines.size());
	mapDataPtr->pixelWidth = mapDataPtr->tileWidth * Constants::TILE_SIZE;		
	mapDataPtr->pixelHeight = mapDataPtr->tileHeight * Constants::TILE_SIZE;

	return mapDataPtr;
}

bool Game::initialiseSDL(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, bool fullscreen) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {		
		int windowFlags = SDL_WINDOW_SHOWN;
		if (fullscreen) {
			windowFlags = SDL_WINDOW_FULLSCREEN;
		}

		window = SDL_CreateWindow(title, windowX, windowY, windowWidth, windowHeight, windowFlags);
		if (!window) {
			std::cerr << "Failed to initialise window!" << std::endl;
			std::cerr << SDL_GetError() << std::endl;
			return false;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (!renderer) {
			std::cerr << "Failed to initialise renderer!" << std::endl;
			std::cerr << SDL_GetError() << std::endl;
			return false;
		}

		if (TTF_Init() == -1) {
			std::cerr << "Failed to initialise SDL_ttf!" << std::endl;
			std::cerr << TTF_GetError() << std::endl;
			return false;
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		
		std::cout << "SDL initialised" << std::endl;
		isRunning = true;
		return true;
	} else {
		std::cerr << "SDL initialisation failed!" << std::endl;
		isRunning = false;
		return false;
	}
}

std::string Game::getTickString() const {
	return std::to_string(tick) + "> ";
}

unsigned int Game::getTick() const {
	return tick;
}

TTF_Font* Game::getFont() const {
	return font;
}

TTF_Font* Game::getSmallerFont() const {
	return smallerFont;
}

SDL_Color Game::getFontColor() const {
	return fontColor;
}
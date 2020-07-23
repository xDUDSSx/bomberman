#pragma once

#include "SDL.h"
#include <SDL_ttf.h>

#include <memory>
#include <string>
#include <map>
#include <vector>

class Tile;
class Player;
class Computer;
class GameInfoBar;
class Map;
struct MapData;
class EntityManager;

/*!
 * The main game representation handling the gameloop, game lifecycle and user input.
 */
class Game {
public:
	/*! A debug flag for printing game info to console */
	static bool debug;
	/*! Enables entity wireframe drawing with F1 */
	static bool drawWireframes;
	/*! Switches to a slower framerate with F2 */
	static bool slowMo;
	/*! Displays pathfinding paths and dangerous tiles with F3 */
	static bool showAI;

	/*!
	 * Vector containing all Players in the game
	 * \see Constants::PLAYER_MAX_COUNT
	 */
	std::vector<std::shared_ptr<Player>> players;
	/*!
	 * Vector containing all Computer (computer controlled players) in the game
	 * \see Constants::PLAYER_OR_COMPUTER_MAX_COUNT
	 */
	std::vector<std::shared_ptr<Computer>> computers;

	EntityManager* entityManager = nullptr;
	Map* map = nullptr;
private:
	/*! An unsigned integer that is incremented by one on every gameloop update */
	unsigned int tick = 0;

	/*! Game state controlling the gameloop. */
	bool isRunning = false;

	/*! Whether an end condition was met. */
	bool endOfGame = false;

	/*! Message displayed on game end. */
	std::string endOfGameMessage;
	
	SDL_Window* window = nullptr;
	std::string windowTitle = "Bomberman - Default";
	int windowHorizontalPos = SDL_WINDOWPOS_CENTERED;
	int windowVerticalPos = SDL_WINDOWPOS_CENTERED;
	bool windowFullscreen = false;
	
	SDL_Renderer* renderer = nullptr;
	SDL_Rect gameDisplayRect{};
	SDL_Texture* gameDisplay = nullptr;

	TTF_Font* font = nullptr;
	int fontSize = 16;

	TTF_Font* smallerFont = nullptr;
	int smallerFontSize = 12;
	
	SDL_Color fontColor = { 255, 255, 255 };

	bool useCustomMap = false;
	std::string customMapFile;

	int infoBarHeight = 80;
	GameInfoBar* infoBar = nullptr;

	int debugInfoTick = 0;
	int debugInfoTickThreshold = 100;

	/*!
	 * Map holding player names for player ids.
	 * <char = playerId, string = Player name>
	 */
	std::map<char, std::string> playerNames;

	/*! Path to the highscore file. */
	std::string highscoreFile = "highscores";
	
	/*!
	 * Vector holding highscore data.
	 * <int = Highscore, string = Name>
	 */
	std::vector<std::pair<int, std::string>> highscores;
	
public:
	
	/*!
	 * Empty game constructor. The game needs to be initialised using the Game::init() method.
	 */
	Game();

	/*!
	 * Game destructor
	 */
	~Game();

	/*!
	 * Initialises the game and starts the gameloop.
	 * The game settings are provided by a configuration file.
	 * If no configuration file was specified (nullptr is passed)
	 * the game starts with default settings.
	 *
	 * \return true if game ended without any errors, false if the game failed to start.
	 */
	bool run(const char* configFile);

	/*!
	 * Registers a new player with a playerId at a specified tile.
	 * 
	 * \param playerId The player id.
	 * \param tile The target tile.
	 * \throws std::runtime_error
	 */
	void registerPlayer(char playerId, Tile* tile);

	/*!
	 * Registers a new computer AI at a specified tile.
	 * 
	 * \param tile The target tile.
	 * \throws std::runtime_error
	 */
	void registerComputer(Tile* tile);
private:
	/*!
	 * Initialises the game.
	 * \return true on success, false on failure.
	 */
	bool init();

	/*!
	 * Loads the game font
	 * 
	 * \return false on load failure, true otherwise
	 */
	bool loadFonts();

	/*!
	 * Game update method. Called by the game loop in order to update various game components.
	 */
	void update();

	/*!
	 * Game render method. Called by the game loop to draw game to screen.
	 */
	void render() const;

	/*!
	 * Reads user inputs and informs game components about them.
	 */
	void handleEvents();

	/*!
	 * Handles keyboard key down events.
	 * 
	 * \param event Keyboard event
	 * \param keys Keyboard state
	 */
	void handleKeyDown(SDL_KeyboardEvent * event, const Uint8 * keys);

	/*!
	 * Handles keyboard key up events.
	 *
	 * \param event Keyboard event
	 * \param keys Keyboard state
	 */
	void handleKeyUp(SDL_KeyboardEvent *event, const Uint8 * keys);

	/*!
	 * Handles key held down events.
	 *
	 * \param keys Keyboard state
	 */
	void handleKeyHeldDown(const Uint8* keys);

	/*!
	 * Game dispose method.
	 * Frees game resources.
	 */
	void dispose() const;

	/*!
	 * Starts the gameloop. This method takes up the current thread until its stopped internally.
	 */
	void gameloop();

	/*!
	 * Sets the end of game message to be displayed.
	 */
	void handleEndOfGame(std::string message);

	/*!
	 * Draws the end of game message and banner.
	 */
	void renderEndOfGame() const;

	/*!
	 * Loads settings from a config file.
	 * The config file has to have the following format on each line:
	 * key=value
	 *
	 * Which keys get saved are defined in the loadConfigEntry() method.
	 * \param configFile Path to the configuration file.
	 * \see loadConfigEntry()
	 */
	void loadConfigFile(const char* configFile);

	/*!
	 * Attempts to load individual config key - value pairs.
	 * 
	 * \param key Config key
	 * \param value Config value
	 */
	void loadConfigEntry(const std::string& key, const std::string& value);

	/*!
	 * Loads and parses the highscore file.
	 * 
	 * \param highscoreFile Path to the highscore file.
	 */
	void loadHighscores(const char* highscoreFile);

	/*!
	 * Parses a single highscore file entry
	 * 
	 * \param keyName Name
	 * \param value Highscore value
	 */
	void loadHighscoreEntry(const std::string& keyName, const std::string& value);

	/*!
	 * Adds the current highscores to the highscore data
	 * and writes the new highscore data into a new highscore file.
	 * 
	 * \param highscoreFile The highscore file path.
	 */
	void generateHighscoreFile(const char* highscoreFile);

	/*!
	 * Loads MapData from a map file.
	 * 
	 * \param mapFile path to the map file
	 * \return pointer to the MapData
	 * \throws runtime_error when the map file cannot be loaded
	 */
	std::unique_ptr<MapData> loadMapFile(const char* mapFile) const;

	/*!
	 * Initialises the SDL library components and creates a window
	 * \param title Title of the window
	 * \param windowX
	 * \param windowY
	 * \param windowWidth
	 * \param windowHeight
	 * \param fullscreen
	 * \return true on success, false otherwise
	 */
	bool initialiseSDL(const char* title, int windowX, int windowY, int windowWidth, int windowHeight, bool fullscreen);
public:
	/*!
	 * Returns the current game tick number as a formatted string.
	 */
	std::string getTickString() const;

	/*!
	 * Returns the current game tick number.
	 */
	unsigned int getTick() const;

	/*!
	 * Returns the main game font.
	 */
	TTF_Font* getFont() const;

	TTF_Font* getSmallerFont() const;

	/*!
	 * Returns the game font color.
	 */
	SDL_Color getFontColor() const;
};

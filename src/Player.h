#pragma once

#include "Bomberman.h"

#include "SDL.h"

/*!
 * A bomberman taking input from the keyboard and representing a player in-game.
 */
class Player : public Bomberman {
private:
	/*!
	 * Character that represents the player in a map file.
	 */
	char playerId;

	/*!
	 * Name/nickname of the player.
	 */
	std::string name;

	SDL_Scancode actionKey;
	SDL_Scancode upKey;
	SDL_Scancode downKey;
	SDL_Scancode leftKey;
	SDL_Scancode rightKey;

	bool inputUp = false;
	bool inputDown = false;
	bool inputLeft = false;
	bool inputRight = false;

public:
	/*!
	 * Creates a player identified by a playerId character at the specified coordinates.
	 * 
	 * \param playerId Character identifying the player.
	 * \param initialX X coordinate
	 * \param initialY Y coordinate
	 * \param actionKey The key used to place bombs.
	 * \param upKey Movement up key.
	 * \param leftKey Movement left key.
	 * \param downKey Movement down key.
	 * \param rightKey Movement right key.
	 */
	Player(char playerId, int initialX, int initialY, SDL_Scancode actionKey, SDL_Scancode upKey, SDL_Scancode leftKey, SDL_Scancode downKey, SDL_Scancode rightKey);

	void update(Game* game) override;

	void handleKeyDown(SDL_KeyboardEvent* event);
	void handleKeyUp(SDL_KeyboardEvent* event);
	void handleKeyHeldDown(const Uint8* keys);

	std::string getName() const;
	
	void setName(const std::string& name);

	/*!
	 * Returns name of the player or playerId if name is empty.
	 */
	std::string getNameOrId() const;
};
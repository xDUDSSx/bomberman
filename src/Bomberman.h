#pragma once

#include "Movable.h"

class Tile;

/*!
 * Bomberman character in the game world.
 */
class Bomberman : public Movable {
protected:
	/*! Health of the bomberman. If it drops to 0, the bomberman dies. */
	int health = Constants::DEFAULT_BOMBERMAN_HEALTH;

	/*! Max health of the bomberman. */
	int maxHealth = Constants::BOMBERMAN_MAX_HEALTH;

	/*! Score of the bomberman. Uses mainly for scoring players. */
	int score = 0;

	/*! Whether the bomberman was recently damaged. */
	bool concussion = false;
	int concussionTick = 0;
	int concussionDuration = Constants::BOMBERMAN_CONCUSSION_DURATION;
	
	/*! Flag indicating placing down a bomb on the next update */
	bool placeDownBombOnUpdate = false;

	/*! Maximum amount of bombs the bomberman can have placed or available at any time. */
	int maxBombs = Constants::BOMBERMAN_MAX_BOMB_COUNT;
	/*! The amount of bombs the bomberman can place at the moment. */
	int availableBombs = Constants::DEFAULT_BOMBERMAN_BOMB_COUNT;
	/*! Bombs that the bomberman has placed and haven't exploded yet. */
	int placedBombs = 0;

	/*! Power level of the bombs this bomberman places. */
	int bombPowerLevel = Constants::DEFAULT_BOMB_POWER_LEVEL;

	/*! Tile the bomberman is currently standing on. */
	Tile* currentTile = nullptr;
public:
	/*!
	 * Creates a bomberman with specified coordinates.
	 */
	Bomberman(int initialX, int initialY);

	void render(SDL_Renderer* renderer) override;
	void update(Game* game) override;

	/*!
	 * Returns ther amount of bombs the bomberman can place at the moment.
	 */
	int getAvailableBombs() const;

	/*!
	 * Sets ther amount of bombs the bomberman can place at the moment.
	 */
	void setAvailableBombs(int availableBombs);

	/*!
	 * Returns the amount of bombs that the bomberman has placed and haven't exploded yet.
	 */
	int getPlacedBombs() const;

	/*!
	 * Sets the amount of bombs that the bomberman has placed and haven't exploded yet.
	 */
	void setPlacedBombs(int placedBombs);

	/*!
	 * Adds/subtracts bombs from the bomb count.
	 */
	void changeBombCount(int change);

	/*!
	 * Returns the bombermans bomb power level
	 * \see Bomb
	 */
	int getBombPowerLevel() const;

	/*!
	 * Sets the bombermans bomb power level.
	 * \see Bomb
	 */
	void setBombPowerLevel(int powerLevel);

	/*!
	 * Kills the bomberman. Removing him from the game entity manager.
	 */
	void kill(Game* game);

	/*!
	 * Damages the bomberman and subtracts health by one unless
	 * the bomberman can't take damage at the moment.
	 * 
	 * \return true if the bombermans health decreased.
	 */
	bool damage();

	/*!
	 * Returns the current bomberman health.
	 */
	int getHealth() const;

	/*!
	 * Sets the bombermans health.
	 * Must be lower than the bombermans Bomberman::maxHealth
	 */
	void setHealth(int health);

	/*!
	 * Returns current score of the bomberman. 
	 */
	int getScore() const;

	/*!
	 * Sets the bombermans score.
	 */
	void setScore(int score);

	/*!
	 * Adds a number to the bombermans score.
	 * 
	 * \param score Score to add (can be negative)
	 */
	void addScore(int score);

	void setSpeed(int speed) override;
private:
	/*!
	 * Places down a bomb on the tile where the bomberman is currently located.
	 * Returns true if a bomb was placed, false otherwise.
	 */
	bool placeDownBomb(Game* game);
};
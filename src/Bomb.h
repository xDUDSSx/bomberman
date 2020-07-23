#pragma once

#include "Constants.h"
#include "Entity.h"

class Bomberman;
class Tile;

/*!
 * A bomb that upon creation waits a certain amount of time before exploding.
 * The explosion destroys nearby walls and bombermans.
 */
class Bomb : public Entity {
protected:
	/*! The bomberman entity that placed this bomb, can be null. */
	Bomberman* bomberman = nullptr;

	/*! Number of tiles the bomb explosion spans. */
	int bombPowerLevel;

	/*! Number of walls the bomb explosion beam destroys before stopping. */
	int bombPenetration = Constants::DEFAULT_BOMB_PENETRATION;

	/*!
	 * A utility value used by the AI. Specifies when an explosion is about to happen so the AI can
	 * evaluate how dangerous this tile is.
	 */
	int ticksToExplosion = Constants::BOMB_EXPLOSION_TIME;

	/*!
	 * Flag indicating that the bombs explode method was already triggered.
	 * Prevents issues when bombs trigger other bombs.
	 */
	bool isExploding = false;
public:
	Bomb(int powerLevel, int x, int y);
	Bomb(int powerLevel, int x, int y, Bomberman* bomberman);

	void update(Game* game) override;

	/*!
	 * Triggers the explosion of the bomb.
	 */
	void explode(Game* game);
protected:
	/*!
	 * Creates the explosion beams. Including particle effects and damage colliders.
	 * 
	 * \param tile The center of the explosion.
	 * \param game Game pointer.
	 */
	void createExplosion(Tile* tile, Game* game) const;

	/*!
	 * A utility method for the AI that calculates where the bomb explosion will reach
	 * and sets the ticksToExplosion values for the tiles.
	 * 
	 * \param tile The center of the explosion.
	 * \param game Game pointer.
	 */
	void alertTilesOfBomb(Tile* tile, Game* game) const;
};

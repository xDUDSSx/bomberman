#pragma once

#include "PowerUp.h"

/*!
 * Powerup that increases bombermans speed.
 */
class SpeedPowerUp : public PowerUp {
public:
	/*!
	 * Creates the powerup at the specified coordinates.
	 */
	SpeedPowerUp(int initialX, int initialY);

protected:
	void applyEffects(Game* game) override;
};
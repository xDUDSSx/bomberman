#pragma once

#include "PowerUp.h"

/*!
 * A powerup that adds health to Bombermans
 */
class HealthPickup : public PowerUp {
public:
	/*!
	 * Creates a health pickup at the specified coordinates.
	 */
	HealthPickup(int initialX, int initialY);

protected:
	void applyEffects(Game* game) override;
};

#pragma once

#include "PowerUp.h"

/*!
 * A collectable that changes the power of bombs of a bomberman upon collection.
 */
class BombPowerModifier : public PowerUp {
private:
	int powerLevelIncrease = 1;

public:
	/*!
	 * Creates this modifier at the specified coordinates.
	 * \param powerLevelIncrease Change to the bombermans bomb power level.
	 * Can be negative to act as a negative power-up
	 */
	BombPowerModifier(int powerLevelIncrease, int initialX, int initialY);

protected:
	void applyEffects(Game* game) override;
};

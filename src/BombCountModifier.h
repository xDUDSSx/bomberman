#pragma once

#include "PowerUp.h"

class Game;

/*!
 * A collectable that changes the amount of bombs a bomberman can place upon collection.
 */
class BombCountModifier : public PowerUp {
private:
	int amountOfBombsToAdd = 1;
	
public:
	/*!
	 * Creates this modifier at the specified coordinates.
	 * \param amountOfBombsToAdd Amount added to the bombermans max bomb count.
	 * Can be negative to act as a negative power-up
	 */
	BombCountModifier(int amountOfBombsToAdd, int initialX, int initialY);
	
protected:
	void applyEffects(Game* game) override;

};

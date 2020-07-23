#pragma once

#include "Collectable.h"

class Bomberman;

/*!
 * An abstract power-up class.
 */
class PowerUp : public Collectable {
private:
	/*! If the powerup has positive or negative effects. */
	bool negative = false;
	
public:
	PowerUp(SDL_Texture* texture, int initialX, int initialY);

	void update(Game* game) override;

	bool isNegative() const;
	void setNegative(bool negative);
protected:
	void applyEffects(Game* game) override = 0;

	/*!
	 * Adds/removes points from the bombermans score.
	 * 
	 * \param bomberman The bomberman
	 * \param negative true to remove score, false otherwise
	 */
	void changeScore(Bomberman* bomberman, bool negative = false);
};

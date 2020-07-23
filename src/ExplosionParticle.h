#pragma once

#include "Particle.h"

/*!
 * An effect particle representing an explosion.
 */
class ExplosionParticle : public Particle {
public:
	enum Direction {
		UP, DOWN, LEFT, RIGHT, CENTER,
		UP_END, DOWN_END, LEFT_END, RIGHT_END
	};

	/*!
	 * Creates the explosion particle at specified coordinates
	 * with a texture set based on the direction.
	 * 
	 * \see Direction
	 */
	ExplosionParticle(int x, int y, Direction direction, int powerLevel);
	
	void render(SDL_Renderer* renderer) override;
protected:
	/*! Direction of the explosion particle. Determines its texture. */
	Direction explosionDirection;
};

#pragma once

#include "Entity.h"

#include <memory>
#include <vector>

/*!
 * A game entity that can be collected by other entities and change their
 * properties. The update method checks for entities that can potentially
 * collect this collectable. The type of entity that can collect it depends
 * on the applyEffects() method implementation.
 */
class Collectable : public Entity {
protected:
	/*! Whether the collectable self-destructs. */
	bool vanishEnabled = false;
	/*! Ticks before vanishing if vanish is enabled. */
	int ticksToVanish = 0;

	/*! A list of entities that collide with the collectable. */
	std::vector<std::shared_ptr<Entity>> potentialCollectors;
public:
	/*!
	 * Create the collectable at specified coordinates.
	 */
	Collectable(SDL_Texture* texture, int initialX, int initialY, int width, int height);

	/*!
	 * Create the collectable at specified coordinates.
	 * The collectable will be set to vanish in time specified by the lifetime.
	 * 
	 * \param lifetime Ticks before vanishing.
	 */
	Collectable(SDL_Texture* texture, int lifetime, int initialX, int initialY, int width, int height);


	void update(Game* game) override;
protected:
	/*!
	 * Method called upon collision with another entity. The potentialCollectors vector contains the
	 * list of colliding entities that the collectable can react to.
	 */
	virtual void applyEffects(Game* game) = 0;
};

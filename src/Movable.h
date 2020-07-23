#pragma once

#include "Entity.h"
#include "Constants.h"

#include <memory>
#include <vector>

/*!
 * An entity that can move around in the game world.
 * Handles collision with other entities.
 */
class Movable : public Entity {
protected:	
	/*! A normalized vector indicating the direction of movement.*/
	SDL_Point movementDirection = { 0, 0 };
	
	/*! Mulitplication with movementDirection specifies the movement vector.*/
	int speed = Constants::DEFAULT_MOVABLE_SPEED;

	/*! Enables or disables collision detection for movement.*/
	bool collision = true;
	
	/*! Entities that the entity is currently colliding with.*/
	std::vector<std::shared_ptr<Entity>> collidingEntities;
public:
	Movable(SDL_Texture* texture, int initialX, int initialY, int width, int height);
	
	void update(Game* game) override;

	/*!
	 * Returns the current movable movement direction.
	 */
	SDL_Point getMovementDirection() const;

	/*!
	 * Sets the movable movement direction.
	 */
	void setMovementDirection(const SDL_Point& movementDirection);

	/*!
	 * Returns the current movable movement speed.
	 */
	int getSpeed() const;

	/*!
	 * Sets the current movable movement speed.
	 */
	virtual void setSpeed(int speed);

	/*!
	 * Whether this movable collides with other entities.
	 */
	bool isCollisionEnabled() const;

	/*!
	 * Sets whether this movable collides with other entities.
	 */
	void setCollisionEnabled(bool collision);
};
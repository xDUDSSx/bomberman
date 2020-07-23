#pragma once
#include "Collectable.h"

#include "Bomb.h"

/*!
 * A collectable entity that damages Bombermans that touch it.
 */
class DamageCollider : public Collectable {
protected:
	/*! Owner of the damage entity. Used to keep track of score. */
	Bomberman* owner = nullptr;

public:
	/*!
	 * Creates a damage collider at specified coordinates.
	 * The collider is set to disappear on its own in a certain amount of ticks.
	 * 
	 * \param lifetime Number of ticks the collider exists for.
	 */
	DamageCollider(int lifetime, int x, int y, int width, int height);

	/*!
	 * Returns the owner of the damage entity. Used to keep track of score.
	 */
	Bomberman* getOwner() const;

	/*!
	 * Sets the owner of the damage entity. Used to keep track of score.
	 */
	void setOwner(Bomberman* owner);
protected:
	void applyEffects(Game* game) override;
};
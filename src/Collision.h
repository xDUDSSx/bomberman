#pragma once

#include "SDL.h"

#include "Game.h"
#include "EntityManager.h"

#include <vector>
#include <algorithm>

/*!
 * Class containing static methods for resolving collision between entities.
 */
class Collision {
public:
	/*!
	 * Checks whether the specified rectangle intersects any other entities.
	 *
	 * \param newPosition The collision rectangle
	 * \param entitiesToIgnore A pointer to a list of entities that should be ignored in the collision detection.
	 *	The pointer can be a nullptr. All entities will be tested for a collision.
	 *
	 * \param collidingEntitiesResult A pointer to a list of entitites that will get cleared and filled with a list of entities that are colliding in this calculation.
	 *	The result pointer can be a nullptr. In that case the calculation will stop on the first detected collision.
	 *
	 * \param callingEntity Usually this method is called by entites. The calling entity specifies the entity calling this method
	 * that is to be ignored like the entitiesToIgnore without creating a singleton vector.
	 * Can be a nullptr if called from outside of an entity.
	 *
	 * \param game Game object used to access the entity manager.
	 * \return true if a collision was detected
	 */
	static bool checkCollision(const SDL_Rect& newPosition, std::vector<std::shared_ptr<Entity>>* entitiesToIgnore, std::vector<std::shared_ptr<Entity>>* collidingEntitiesResult, const Entity* callingEntity, Game* game) {
		if (collidingEntitiesResult) collidingEntitiesResult->clear();
		bool collisionDetected = false;
		for (auto& entity : game->entityManager->getEntities()) {
			if (entity.get() != callingEntity && entity->isCollidable()) {
				if (!entitiesToIgnore || (entitiesToIgnore && std::find(entitiesToIgnore->begin(), entitiesToIgnore->end(), entity) == entitiesToIgnore->end())) {
					if (SDL_HasIntersection(&newPosition, entity->getRect())) {
						collisionDetected = true;
						if (collidingEntitiesResult) {
							collidingEntitiesResult->push_back(entity);
						} else {
							break;
						}
					}
				}
			}
		}
		return collisionDetected;
	}
};

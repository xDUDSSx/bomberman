#pragma once

#include "Entity.h"

#include "SDL.h"

#include <vector>
#include <memory>

/*!
 * Manages rendering and updates of game entities.
 */
class EntityManager {
private:
	/*! List of all entities managed by the manager. */
	std::vector<std::shared_ptr<Entity>> entities;
	/*! Helper list used when removing entities from the main one. */
	std::vector<std::shared_ptr<Entity>> entitiesToRemove;
public:
	/*!
	 * Calls update() methods for all entities.
	 * 
	 * \param game The game.
	 */
	void updateEntities(Game* game);

	/*!
	 * Renders all entities of the entity manager that do not have external rendering enabled.
	 * Entity manager itself only supports two render layers so far. Those being foreground and background.
	 * Layer can be set using the Entity::setForegroundRender() method.
	 *
	 * \param renderer The renderer.
	 */
	void renderEntities(SDL_Renderer* renderer) const;

	/*!
	 * Adds the entity to the manager. The entity will be updated and rendered with manager updates and render calls.
	 * 
	 * \param entity Pointer to a dynamically allocated Entity. Its ownership moves to the manager.
	 */
	void addEntity(Entity* entity);

	/*!
	 * Adds the entity to the manager. The entity will be updated and rendered with manager updates and render calls.
	 *
	 * \param entity Entity to add.
	 */
	void addEntity(const std::shared_ptr<Entity>& entity);

	/*!
	 * Removes the entity from the manager on the next update.
	 */
	void removeEntity(const std::shared_ptr<Entity>& entity);

	/*!
	 * Removes the entity from the manager on the next update.
	 */
	void removeEntity(Entity* entity);

	/*!
	 * Returns a list of all entites in the manager.
	 */
	const std::vector<std::shared_ptr<Entity>>& getEntities() const;
private:
	/*!
	 * Draws an entity with a renderer.
	 */
	void renderEntity(const std::shared_ptr<Entity>& entity, SDL_Renderer* renderer) const;
};
#include "EntityManager.h"

#include <memory>

#include "Game.h"

void EntityManager::updateEntities(Game* game) {
	//While iterating through entities, vector modifications can occur
	//and thus an index for loop is used that updates all the entites
	//present at the begining of this method which works properly because
	//entities are always inserted at the end of the vector.
	const size_t size = entities.size();
	for (unsigned int i = 0; i < size; i++) {
		entities[i]->update(game);
	}
	
	//Calculate the number of entities newly created during the update
	const size_t numberOfEntitiesAdded = entities.size() - size;

	/*
	//Removing entities that might have been flagged for deletion during the updates.
	for (auto& entity : entitiesToRemove) {
		auto itr = std::find(entities.begin(), entities.end(), entity);
		if (itr != entities.end()) {
			entities.erase(itr);
		}
	}
	entitiesToRemove.clear();
	*/

	//Removing entities that might have been flagged for deletion during the updates.
	auto itr = entities.begin();
	while (itr != entities.end()) {
		if ((*itr)->isToBeRemoved()) {
			itr = entities.erase(itr);
		} else {
			++itr;
		}
	}

	//If any new entites were added during the update, update them as well now that the old entities were removed.
	if (numberOfEntitiesAdded > 0) {
		const size_t newSize = entities.size();
		for (size_t i = (newSize - numberOfEntitiesAdded); i < newSize; i++) {
			entities[i]->update(game);
		}
	}	
}

void EntityManager::renderEntities(SDL_Renderer * renderer) const {
	//Render background
	for (auto& entity : entities) {
		if (!entity->isRenderingExternal() && !entity->isForegroundRender()) {
			renderEntity(entity, renderer);
		}
	}
	//Render foreground
	for (auto& entity : entities) {
		if (!entity->isRenderingExternal() && entity->isForegroundRender()) {
			renderEntity(entity, renderer);
		}
	}
}

void EntityManager::addEntity(Entity * entity) {
	const auto newEntityPtr = std::shared_ptr<Entity>(entity);
	entities.push_back(newEntityPtr);	//Add the entity to the global entity list
}

void EntityManager::addEntity(const std::shared_ptr<Entity>& entity) {
	entities.push_back(entity); //Add the entity to the global entity list
}

void EntityManager::removeEntity(const std::shared_ptr<Entity>& entity) {
	entity->setToBeRemoved(true);
}

void EntityManager::removeEntity(Entity* entity) {
	entity->setToBeRemoved(true);
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities() const {
	return entities;
}

void EntityManager::renderEntity(const std::shared_ptr<Entity>& entity, SDL_Renderer* renderer) const {
	entity->render(renderer);
	if (Game::drawWireframes) {
		SDL_RenderDrawRect(renderer, entity->getRect());
	}
}

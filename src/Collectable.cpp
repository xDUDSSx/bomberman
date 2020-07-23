#include "Collectable.h"

#include "Collision.h"

#include <iostream>

Collectable::Collectable(SDL_Texture * texture, int initialX, int initialY, int width, int height)
: Entity(texture, initialX, initialY, width, height) {
	this->setCollidable(false);
}

Collectable::Collectable(SDL_Texture* texture, int lifetime, int initialX, int initialY, int width, int height)
: Entity(texture, initialX, initialY, width, height),
vanishEnabled(true),
ticksToVanish(lifetime) {
	this->setCollidable(false);
}

void Collectable::update(Game * game) {
	if (vanishEnabled) {
		ticksToVanish--;
		if (ticksToVanish <= 0) {
			game->entityManager->removeEntity(this);
			if (Game::debug) std::cout << game->getTickString() << "Collectable vanished" << std::endl;
			return;
		}
	}

	Entity::update(game);
	if (Collision::checkCollision(position, nullptr, &potentialCollectors, this, game)) {
		applyEffects(game);
	}
}

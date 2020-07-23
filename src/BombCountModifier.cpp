#include "BombCountModifier.h"

#include "Game.h"
#include "Bomberman.h"
#include "TextureManager.h"
#include "EntityManager.h"

#include <iostream>
#include <string>

BombCountModifier::BombCountModifier(int amountOfBombsToAdd, int initialX, int initialY)
: PowerUp(TextureManager::bombCountUpTexture, initialX, initialY),
amountOfBombsToAdd(amountOfBombsToAdd) {
	if (this->amountOfBombsToAdd < 1) {
		this->setTexture(TextureManager::bombCountDownTexture);
		this->setNegative(true);
	}	
}

void BombCountModifier::applyEffects(Game * game) {
	for (auto& entity: potentialCollectors) {
		Entity* entityPtr = entity.get();
		if (Bomberman* bomberman = dynamic_cast<Bomberman*>(entityPtr)) {
			bomberman->changeBombCount(amountOfBombsToAdd);
			changeScore(bomberman, (this->amountOfBombsToAdd < 1));
			if (Game::debug) std::cout << game->getTickString() << "Bomberman picked up BombCountModifier (" << amountOfBombsToAdd << ")" << std::endl;
			game->entityManager->removeEntity(this);
		}
	}
}

#include "BombPowerModifier.h"

#include "Bomberman.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "PowerUp.h"
#include "Game.h"

#include <iostream>
#include <string>

BombPowerModifier::BombPowerModifier(int powerLevelIncrease, int initialX, int initialY)
: PowerUp(TextureManager::bombPowerUpTexture, initialX, initialY),
  powerLevelIncrease(powerLevelIncrease) {
	if (this->powerLevelIncrease < 1) {
		this->setTexture(TextureManager::bombPowerDownTexture);
		this->setNegative(true);
	}
}

void BombPowerModifier::applyEffects(Game * game) {
	for (auto& entity : potentialCollectors) {
		Entity* entityPtr = entity.get();
		if (Bomberman* bomberman = dynamic_cast<Bomberman*>(entityPtr)) {
			bomberman->setBombPowerLevel(bomberman->getBombPowerLevel() + powerLevelIncrease);
			changeScore(bomberman, (this->powerLevelIncrease < 1));
			if (Game::debug) std::cout << game->getTickString() << "Bomberman picked up BombPowerModifier (" << powerLevelIncrease << ")" << std::endl;
			game->entityManager->removeEntity(this);
		}
	}
}

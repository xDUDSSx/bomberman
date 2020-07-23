#include "SpeedPowerUp.h"

#include "Game.h"
#include "TextureManager.h"
#include "Bomberman.h"
#include "EntityManager.h"

#include <iostream>

SpeedPowerUp::SpeedPowerUp(int initialX, int initialY)
	: PowerUp(TextureManager::speedPowerUp, initialX, initialY) {
}

void SpeedPowerUp::applyEffects(Game * game) {
	for (auto& entity : potentialCollectors) {
		Entity* entityPtr = entity.get();
		if (Bomberman* bomberman = dynamic_cast<Bomberman*>(entityPtr)) {
			bomberman->setSpeed(bomberman->getSpeed() + 1);
			changeScore(bomberman);
			if (Game::debug) std::cout << game->getTickString() << "Bomberman picked up SpeedPowerUp" << std::endl;
			game->entityManager->removeEntity(this);
		}
	}
}

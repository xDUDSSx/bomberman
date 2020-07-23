#include "HealthPickup.h"

#include "Bomberman.h"
#include "EntityManager.h"
#include "Game.h"
#include "TextureManager.h"

HealthPickup::HealthPickup(int initialX, int initialY)
: PowerUp(TextureManager::health, initialX, initialY) {
	
}

void HealthPickup::applyEffects(Game* game) {
	for (auto& entity : potentialCollectors) {
		Entity* entityPtr = entity.get();
		if (Bomberman* bomberman = dynamic_cast<Bomberman*>(entityPtr)) {
			bomberman->setHealth(bomberman->getHealth() + 1);
			changeScore(bomberman);
			game->entityManager->removeEntity(this);
		}
	}
}

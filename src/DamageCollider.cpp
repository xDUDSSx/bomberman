#include "DamageCollider.h"

#include "Bomberman.h"
#include "Game.h"

#include <iostream>

DamageCollider::DamageCollider(int lifetime, int x, int y, int width, int height)
: Collectable(NULL, lifetime, x, y, width, height) {
	
}

Bomberman* DamageCollider::getOwner() const {
	return owner;
}

void DamageCollider::setOwner(Bomberman* owner) {
	this->owner = owner;
}

void DamageCollider::applyEffects(Game* game) {
	for (auto& entity : potentialCollectors) {
		Entity* entityPtr = entity.get();
		if (Bomberman* bomberman = dynamic_cast<Bomberman*>(entityPtr)) {
			if (bomberman->damage()) {
				//Add or remove score of the owner
				if (owner) {
					if (owner == bomberman) {
						owner->addScore(Constants::BOMBERMAN_SELF_DAMAGE_SCORE);
					} else {
						owner->addScore(Constants::BOMBERMAN_DAMAGE_SCORE);
					}	
				}
				if (Game::debug) std::cout << game->getTickString() << "Explosion damaged bomberman" << std::endl;
			}
		}
	}
}

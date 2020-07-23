#include "Particle.h"

#include "Game.h"
#include "EntityManager.h"

#include <iostream>

Particle::Particle(SDL_Texture* texture, int x, int y, int width, int height)
: Particle(texture, 0, x, y, width, height) {
	
}

Particle::Particle(SDL_Texture* texture, int lifetime, int x, int y, int width, int height)
: Entity(texture, x, y, width, height),
ticksToVanish(lifetime) {
	this->setCollidable(false);
	if (this->ticksToVanish > 0) {
		vanishEnabled = true;
	}
}

Particle::Particle(std::vector<SDL_Texture*> frames, int frameDuration, bool repeat, int x, int y, int width, int height)
: Particle(std::move(frames), frameDuration, repeat, 0, x, y, width, height) {
	
}

Particle::Particle(std::vector<SDL_Texture*> frames, int frameDuration, bool repeat, int lifetime, int x, int y, int width, int height)
: Entity(NULL, x, y, width, height),
ticksToVanish(lifetime) {
	this->setCollidable(false);
	if (!frames.empty()) {
		sprite.setTexture(frames[0]);
		if (frames.size() > 1) {
			sprite.setAnimated(true);
			sprite.setAnimationSpeed(frameDuration);
			sprite.setRepeatAnimation(repeat);
			for (auto frame: frames) {
				sprite.addAnimationFrame(frame);
			}
		}
	}
	if (this->ticksToVanish > 0) {
		vanishEnabled = true;
	}
}

void Particle::update(Game * game) {
	Entity::update(game);
	
	if (vanishEnabled) {
		ticksToVanish--;
		if (alphaVanish) {
			if (ticksToVanish < alphaVanishTick) {
				sprite.setAlpha(static_cast<int>(255 * (static_cast<double>(ticksToVanish) / alphaVanishTick)));
			}
		}
		if (ticksToVanish <= 0) {
			game->entityManager->removeEntity(this);
			if (Game::debug) std::cout << game->getTickString() << "Particle vanished" << std::endl;
		}
	} else
	if (sprite.isAnimationFinished()) {
		game->entityManager->removeEntity(this);
		if (Game::debug) std::cout << game->getTickString() << "Particle animation finished" << std::endl;
	}
}

void Particle::enableAlphaVanish(int ticksRemaining) {
	alphaVanish = true;
	alphaVanishTick = ticksRemaining;
}


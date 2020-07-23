#include "ExplosionParticle.h"

#include "Constants.h"
#include "TextureManager.h"

ExplosionParticle::ExplosionParticle(int x, int y, Direction direction, int powerLevel)
: Particle(TextureManager::explosionCenter, Constants::EXPLOSION_DURATION, x, y, Constants::EXPLOSION_PARTICLE_SIZE, Constants::EXPLOSION_PARTICLE_SIZE),
explosionDirection(direction) {
	this->enableAlphaVanish(8);
}

void ExplosionParticle::render(SDL_Renderer* renderer) {
	//Handle texture
	switch (explosionDirection) {
		case UP:
		case DOWN:
		case LEFT:
		case RIGHT:
			this->sprite.setTexture(TextureManager::explosionSide);
			break;
		case UP_END:
		case DOWN_END:
		case LEFT_END:
		case RIGHT_END:
			this->sprite.setTexture(TextureManager::explosionEnd);
			break;
		case CENTER:
			this->sprite.setTexture(TextureManager::explosionCenter);
			break;
	}

	//Handle rotation
	switch (explosionDirection) {
		case UP: break;
		case DOWN: this->sprite.setRotation(180); break;
		case LEFT: this->sprite.setRotation(270); break;
		case RIGHT: this->sprite.setRotation(90); break;
		case UP_END: break;
		case DOWN_END: this->sprite.setRotation(180); break;
		case LEFT_END: this->sprite.setRotation(270); break;
		case RIGHT_END: this->sprite.setRotation(90); break;
		case CENTER: this->setForegroundRender(true); break;
	}
	
	Particle::render(renderer);
}

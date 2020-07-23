#include "PowerUp.h"

#include "Bomberman.h"
#include "Constants.h"
#include "TextureManager.h"

PowerUp::PowerUp(SDL_Texture* texture, int initialX, int initialY)
: Collectable(texture, Constants::POWERUP_LIFETIME, initialX, initialY, Constants::POWERUP_SIZE, Constants::POWERUP_SIZE) {
	
}

void PowerUp::update(Game* game) {
	if (this->ticksToVanish == Constants::POWERUP_BLINK_LIFETIME) {
		this->sprite.setEffect(Sprite::BLINKING, Constants::DEFAULT_BLINKING_EFFECT_DURATION);
	}
	Collectable::update(game);
}

void PowerUp::changeScore(Bomberman* bomberman, bool negative) {
	if (negative) 
		bomberman->addScore(-Constants::POWERUP_PICKUP_SCORE);
	else
		bomberman->addScore(Constants::POWERUP_PICKUP_SCORE);
}

bool PowerUp::isNegative() const {
	return negative;
}

void PowerUp::setNegative(bool negative) {
	this->negative = negative;
}
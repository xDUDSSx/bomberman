#include "Bomberman.h"

#include "Bomb.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include "Utils.h"
#include "Game.h"
#include "Map.h"

Bomberman::Bomberman(int initialX, int initialY)
: Movable(TextureManager::bombermanTexture, initialX, initialY, Constants::BOMBERMAN_SIZE, Constants::BOMBERMAN_SIZE) {
	 
}

void Bomberman::render(SDL_Renderer * renderer) {
	Movable::render(renderer);
}

void Bomberman::update(Game* game) {
	if (health <= 0) {
		kill(game);
		return;
	}
	
	if (concussion) {
		this->sprite.setEffect(Sprite::BLINKING, Constants::DEFAULT_BLINKING_EFFECT_DURATION, false);
		concussionTick++;
		if (concussionTick >= concussionDuration) {
			concussion = false;
			concussionTick = 0;
		}
	} else {
		this->sprite.setEffect(Sprite::NONE);
		this->sprite.setScale(1);
	}

	Movable::update(game);
	
	if (placeDownBombOnUpdate && availableBombs > 0) {
		placeDownBomb(game);
	}

	if (!currentTile) {
		currentTile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
	} else {
		Tile* tempTile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
		if (tempTile && !tempTile->equals(currentTile)) {
			currentTile->setBombermanPresent(false);
			currentTile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
			currentTile->setBombermanPresent(true);
		} else {
			currentTile->setBombermanPresent(true);
		}
	}
}

int Bomberman::getAvailableBombs() const {
	return availableBombs;
}

void Bomberman::setAvailableBombs(int availableBombs) {
	this->availableBombs = availableBombs;
	if (this->availableBombs < 0) {
		this->availableBombs = 0;
	}
}

int Bomberman::getPlacedBombs() const {
	return placedBombs;
}

void Bomberman::setPlacedBombs(int placedBombs) {
	this->placedBombs = placedBombs;
}

void Bomberman::changeBombCount(int change) {
	 if (change < 0) {
		 if (placedBombs + availableBombs + change > 0) {
			 availableBombs--;
		 }
	 } else {
		 if (placedBombs + availableBombs + change <= maxBombs) {
			 availableBombs++;
		 }
	 }
}

int Bomberman::getBombPowerLevel() const {
	return bombPowerLevel;
}

void Bomberman::setBombPowerLevel(int powerLevel) {
	this->bombPowerLevel = powerLevel;
	if (this->bombPowerLevel < 1) {
		this->bombPowerLevel = 1;
	}
}

void Bomberman::kill(Game * game) {
	//TODO: death animation
	if (Game::debug) std::cout << game->getTickString() << "Bomberman killed" << std::endl;
	currentTile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
	if (currentTile) currentTile->setBombermanPresent(false);
	game->entityManager->removeEntity(this);
}

bool Bomberman::damage() {
	if (!concussion) {
		this->health--;
		concussion = true;
		return true;
	}
	return false;
}

int Bomberman::getHealth() const {
	return health;
}

void Bomberman::setHealth(int health) {
	if (health > maxHealth) {
		this->health = maxHealth;
	} else {
		this->health = health;
	}
}

int Bomberman::getScore() const {
	return score;
}

void Bomberman::setScore(int score) {
	this->score = score;
}

void Bomberman::addScore(int score) {
	this->score = this->score + score;
}

void Bomberman::setSpeed(int speed) {
	if (speed > Constants::BOMBERMAN_MAX_SPEED) {
		Movable::setSpeed(Constants::BOMBERMAN_MAX_SPEED);
	} else {
		Movable::setSpeed(speed);
	}
}

bool Bomberman::placeDownBomb(Game * game) {
	bool bombPlaced = false;
	Tile* tile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
	if (tile && this->collidingEntities.empty()) {
		if (Game::debug) std::cout << game->getTickString() << "Bomberman placed down bomb at " << tile->getX() << "," << tile->getY() << std::endl;
		game->entityManager->addEntity(new Bomb(this->bombPowerLevel, tile->getX(), tile->getY(), this));
		bombPlaced = true;
	}
	placeDownBombOnUpdate = false;
	return bombPlaced;
}

#include "Tile.h"

#include "TextureManager.h"
#include "Utils.h"
#include "BombCountModifier.h"
#include "BombPowerModifier.h"
#include "Constants.h"
#include "EntityManager.h"
#include "Game.h"
#include "HealthPickup.h"
#include "SpeedPowerUp.h"
#include "Particle.h"

Tile::Tile(int initialX, int initialY)
: Entity(TextureManager::tileTexture, initialX, initialY, Constants::TILE_SIZE, Constants::TILE_SIZE) {
	this->setCollidable(false);
	this->setRenderingExternal(true);
}

void Tile::update(Game* game) {
	if (ticksToExplosion > 0) {
		ticksToExplosion--;
	} else {
		bombPlaced = false;
	}
}

void Tile::render(SDL_Renderer* renderer) {
	if (wall) {
		if (indestructible) {
			if (edgeWall) {
				this->sprite.setTexture(TextureManager::indestructibleEdgeWallTexture);
			} else {
				this->sprite.setTexture(TextureManager::indestructibleWallTexture);
			}
		} else {
			this->sprite.setTexture(TextureManager::wallTexture);
		}
	} else {
		this->sprite.setTexture(TextureManager::tileTexture);
	}
	Entity::render(renderer);

	if (Game::showAI) {
		if (this->isDangerous() || this->isBombermanPresent()) {
			SDL_RenderDrawRect(renderer, this->getRect());
		}
	}
}

bool Tile::destroyWall(Game* game) {
	if (!indestructible && wall) {
		setWall(false);
		createPowerup(game);
		
		Particle* destructionEffect = TextureManager::createWallDestructionEffect();
		destructionEffect->centerInRect(*this->getRect());
		game->entityManager->addEntity(destructionEffect);
		
		return true;
	}
	return false;
}

bool Tile::isDangerous() const {
	if (bombPlaced) {
		return true;
	}
	if (ticksToExplosion > 0 && ticksToExplosion < 64) {
		return true;
	}
	return false;
}

bool Tile::equals(Tile* tile) const {
	return ((this->getX() == tile->getX()) && (this->getY() == tile->getY()));
}

void Tile::createPowerup(Game* game) const {
	std::shared_ptr<Entity> newPowerUp;
	if (Utils::getRandomIntNumberInRange(0, 100) < Constants::POWERUP_DROP_CHANCE) {
		const int typeChance = Utils::getRandomIntNumberInRange(0, 100);
		if (typeChance < 45) {
			if (Utils::getRandomIntNumberInRange(0, 100) < Constants::NEGATIVE_POWERUP_CHANCE) {
				newPowerUp = std::make_shared<BombCountModifier>(BombCountModifier(-1, this->getX(), this->getY()));
			} else {
				newPowerUp = std::make_shared<BombCountModifier>(BombCountModifier(1, this->getX(), this->getY()));
			}
		} else if (typeChance < 90) {
			if (Utils::getRandomIntNumberInRange(0, 100) < Constants::NEGATIVE_POWERUP_CHANCE) {
				newPowerUp = std::make_shared<BombPowerModifier>(BombPowerModifier(-1, this->getX(), this->getY()));
			} else {
				newPowerUp = std::make_shared<BombPowerModifier>(BombPowerModifier(1, this->getX(), this->getY()));
			}
		} else {
			if (Utils::getRandomIntNumberInRange(0, 100) < 70) {
				newPowerUp = std::make_shared<HealthPickup>(HealthPickup(this->getX(), this->getY()));
			} else {
				newPowerUp = std::make_shared<SpeedPowerUp>(SpeedPowerUp(this->getX(), this->getY()));
			}
		}
		newPowerUp->centerInRect(*this->getRect());
		game->entityManager->addEntity(newPowerUp);
	}	
}

bool Tile::isWall() const {
	return wall;
}

void Tile::setWall(bool wall) {
	this->wall = wall;
	this->setCollidable(wall);
}

bool Tile::isIndestructible() const {
	return indestructible;
}

void Tile::setIndestructible(bool indestructible) {
	this->indestructible = indestructible;
}

bool Tile::isEdgeWall() const {
	return indestructible;
}

void Tile::setEdgeWall(bool edgewall) {
	this->edgeWall = edgewall;
}

bool Tile::isOverrideWalkable() const {
	return overrideWalkable;
}

void Tile::setOverrideWalkable(bool overrideWalkable) {
	this->overrideWalkable = overrideWalkable;
}

bool Tile::isBombPlaced() const {
	return bombPlaced;
}

void Tile::setBombPlaced(bool bombPlaced) {
	this->bombPlaced = bombPlaced;
}

int Tile::getTicksToExplosion() const {
	return ticksToExplosion;
}

void Tile::setTicksToExplosion(int ticksToExplosion) {
	this->ticksToExplosion = ticksToExplosion;
}

bool Tile::isBombermanPresent() const {
	return bombermanPresent;
}

void Tile::setBombermanPresent(bool bombermanPresent) {
	this->bombermanPresent = bombermanPresent;
}
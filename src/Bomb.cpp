#include "Bomb.h"

#include "TextureManager.h"
#include "DamageCollider.h"
#include "ExplosionParticle.h"
#include "Collision.h"
#include "Bomberman.h"
#include "Map.h"

#include <iostream>
#include <string>

Bomb::Bomb(int powerLevel, int x, int y)
: Entity(TextureManager::bombFrame1, x, y, Constants::BOMB_SIZE, Constants::BOMB_SIZE),
bombPowerLevel(powerLevel) {
	sprite.setAnimated(true);
	sprite.setEffect(Sprite::BREATHING, Constants::BOMB_BREATHING_EFFECT_DURATION);
	sprite.setAnimationSpeed(5);
	sprite.addAnimationFrame(TextureManager::bombFrame1);
	sprite.addAnimationFrame(TextureManager::bombFrame2);
	sprite.addAnimationFrame(TextureManager::bombFrame3);
}

Bomb::Bomb(int powerLevel, int x, int y, Bomberman* bomberman) : Bomb(powerLevel, x, y) {
	this->bomberman = bomberman;
	bomberman->setAvailableBombs(bomberman->getAvailableBombs() - 1);
	bomberman->setPlacedBombs(bomberman->getPlacedBombs() + 1);
}

void Bomb::update(Game * game) {
	Entity::update(game);

	if (Tile* t = game->map->getTileAtCoordinates(this->getX(), this->getY())) {
		if (!t->isBombPlaced()) {
			t->setBombPlaced(true);
			alertTilesOfBomb(t, game);
		}
	}
	
	ticksToExplosion--;
	if (ticksToExplosion == Constants::BOMB_EXPLOSION_EFFECT_DURATION) {
		sprite.setEffect(Sprite::EXPLOSION, Constants::BOMB_EXPLOSION_EFFECT_DURATION);
	}
	if (ticksToExplosion <= 0) {
		explode(game);
	} 
}

void Bomb::explode(Game * game) {
	if (!isExploding) {
		isExploding = true;

		//Create explosion
		Tile* tile = game->map->getTileAtCoordinates(this->getCenterX(), this->getCenterY());
		if (tile) {
			if (Game::debug) std::cout << game->getTickString() << "Bomb exploded at " << tile->getX() << "," << tile->getY() << std::endl;
			createExplosion(tile, game);
		} else {
			if (Game::debug) std::cout << game->getTickString() << "Bomb exploded outside the map!" << std::endl;
		}

		//Handle bomb count of the bomberman is one is assigned
		if (bomberman) {
			bomberman->setAvailableBombs(bomberman->getAvailableBombs() + 1);
			bomberman->setPlacedBombs(bomberman->getPlacedBombs() - 1);
		}
		game->entityManager->removeEntity(this);
	}
}

void Bomb::createExplosion(Tile* tile, Game * game) const {
	tile->destroyWall(game);
	game->entityManager->addEntity((new ExplosionParticle(tile->getX(), tile->getY(), ExplosionParticle::CENTER, this->bombPowerLevel))->centerInRect(*tile->getRect()));

	//Create bomb beams in all 4 major directions from the base tile.
	//The beams stop at the first indestructible wall or when they destroy as many walls as their penetration value.

	int effectiveExplosionDistance[4] = { 0 };
	
	Tile* tileToDestroy = tile;
	for (int dir = 0; dir < 4; dir++) {
		int wallsDestroyed = 0;
		for (int i = 0; i < bombPowerLevel; i++) {
			switch (dir) {
				case 0:
					tileToDestroy = game->map->getTileAbove(tileToDestroy); break;
				case 1:
					tileToDestroy = game->map->getTileLeft(tileToDestroy); break;
				case 2:
					tileToDestroy = game->map->getTileBelow(tileToDestroy); break;
				case 3:
					tileToDestroy = game->map->getTileRight(tileToDestroy); break;
			}
			if (tileToDestroy && !tileToDestroy->isIndestructible() && wallsDestroyed < bombPenetration) {
				//If a wall was truly destroyed, increment the wallsDestroyed counter
				if (tileToDestroy->destroyWall(game)) {
					wallsDestroyed++;
					if (this->bomberman) this->bomberman->addScore(Constants::WALL_DESTROY_SCORE);
				}

				Tile* nextTile = nullptr;
				ExplosionParticle::Direction explosionDirection = ExplosionParticle::CENTER;
				ExplosionParticle::Direction explosionDirectionEnd = ExplosionParticle::CENTER;
				switch (dir) {
					case 0:
						nextTile = game->map->getTileAbove(tileToDestroy);
						explosionDirection = ExplosionParticle::UP;
						explosionDirectionEnd = ExplosionParticle::UP_END;
						break;
					case 1:
						nextTile = game->map->getTileLeft(tileToDestroy);
						explosionDirection = ExplosionParticle::LEFT;
						explosionDirectionEnd = ExplosionParticle::LEFT_END;
						break;
					case 2:
						nextTile = game->map->getTileBelow(tileToDestroy);
						explosionDirection = ExplosionParticle::DOWN;
						explosionDirectionEnd = ExplosionParticle::DOWN_END;
						break;
					case 3:
						nextTile = game->map->getTileRight(tileToDestroy);
						explosionDirection = ExplosionParticle::RIGHT;
						explosionDirectionEnd = ExplosionParticle::RIGHT_END;
						break;
				}

				if (nextTile) {
					//Create explosion entity for this tile
					if (nextTile && !nextTile->isIndestructible() && i != bombPowerLevel - 1 && wallsDestroyed < bombPenetration) {
						game->entityManager->addEntity((new ExplosionParticle(tileToDestroy->getX(), tileToDestroy->getY(), explosionDirection, this->bombPowerLevel))->centerInRect(*tileToDestroy->getRect()));
					} else {
						game->entityManager->addEntity((new ExplosionParticle(tileToDestroy->getX(), tileToDestroy->getY(), explosionDirectionEnd, this->bombPowerLevel))->centerInRect(*tileToDestroy->getRect()));
					}
				}
			} else {
				effectiveExplosionDistance[dir] = i;
				break;
			}
			if (i == bombPowerLevel - 1) {
				effectiveExplosionDistance[dir] = i + 1;
			}
		}
		tileToDestroy = tile;
	}

	//Create explosion damage colliders
	//Explosion damage is applied using 2 DamageColliders that apply damage to bombermans that touch them.
	//The two DamageColliders form a cross that represents the explosion beams.
	
	const int horizontalColliderSize = (effectiveExplosionDistance[1] + effectiveExplosionDistance[3] + 1) * tile->getWidth() - (tile->getWidth() - Constants::EXPLOSION_SIZE);
	const int horizontalColliderX = tile->getX() - (effectiveExplosionDistance[1] * tile->getWidth()) + (tile->getWidth() - Constants::EXPLOSION_SIZE) / 2;
	const int horizontalColliderY = tile->getY() + tile->getHeight() / 2 - Constants::EXPLOSION_SIZE / 2;
	
	const int verticalColliderSize = (effectiveExplosionDistance[0] + effectiveExplosionDistance[2] + 1) * tile->getHeight() - (tile->getHeight() - Constants::EXPLOSION_SIZE);
	const int verticalColliderX = tile->getX() + tile->getWidth() / 2 - Constants::EXPLOSION_SIZE / 2;
	const int verticalColliderY = tile->getY() - (effectiveExplosionDistance[0] * tile->getHeight()) + (tile->getHeight() - Constants::EXPLOSION_SIZE) / 2;

	DamageCollider horizontalCollider = DamageCollider(Constants::EXPLOSION_DURATION , horizontalColliderX, horizontalColliderY, horizontalColliderSize, Constants::EXPLOSION_SIZE);
	DamageCollider verticalCollider = DamageCollider(Constants::EXPLOSION_DURATION, verticalColliderX, verticalColliderY, Constants::EXPLOSION_SIZE, verticalColliderSize);

	horizontalCollider.setOwner(bomberman);
	verticalCollider.setOwner(bomberman);

	game->entityManager->addEntity(std::make_shared<DamageCollider>(horizontalCollider));
	game->entityManager->addEntity(std::make_shared<DamageCollider>(verticalCollider));

	//Trigger any bombs that the horizontal collider hits
	std::vector<std::shared_ptr<Entity>> collidingEntities;
	if (Collision::checkCollision(*horizontalCollider.getRect(), nullptr, &collidingEntities, this, game)) {
		for (auto& entity : collidingEntities) {
			Entity* entityPtr = entity.get();
			if (Bomb* bomb = dynamic_cast<Bomb*>(entityPtr)) {
				bomb->explode(game);
			}
		}
	}

	//Trigger any bombs that the vertical collider hits
	if (Collision::checkCollision(*verticalCollider.getRect(), nullptr, &collidingEntities, this, game)) {
		for (auto& entity : collidingEntities) {
			Entity* entityPtr = entity.get();
			if (Bomb* bomb = dynamic_cast<Bomb*>(entityPtr)) {
				bomb->explode(game);
			}
		}
	}
	
}

void Bomb::alertTilesOfBomb(Tile* tile, Game* game) const {
	//Create bomb beams in all 4 major directions from the base tile.
	//The beams stop at the first indestructible wall or when they destroy as many walls as their penetration value.
	Tile* nextTile = tile;
	nextTile->setTicksToExplosion(this->ticksToExplosion + Constants::EXPLOSION_DURATION);
	for (int dir = 0; dir < 4; dir++) {
		int wallsDestroyed = 0;
		for (int i = 0; i < bombPowerLevel; i++) {
			switch (dir) {
				case 0:
					nextTile = game->map->getTileAbove(nextTile); break;
				case 1:
					nextTile = game->map->getTileLeft(nextTile); break;
				case 2:
					nextTile = game->map->getTileBelow(nextTile); break;
				case 3:
					nextTile = game->map->getTileRight(nextTile); break;
			}
			if (nextTile && !nextTile->isIndestructible() && wallsDestroyed < bombPenetration) {
				//If a wall was truly destroyed, increment the wallsDestroyed counter
				if (!nextTile->isIndestructible() && nextTile->isWall()) {
					wallsDestroyed++;
				}

				//Alert tile of bomb
				if (nextTile->getTicksToExplosion() < 1) {
					nextTile->setTicksToExplosion(this->ticksToExplosion + Constants::EXPLOSION_DURATION);
				}
			} else {
				break;
			}
		}
		nextTile = tile;
	}
}

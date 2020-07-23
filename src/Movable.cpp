#include "Movable.h"

#include "Game.h"
#include "Collision.h"
#include "Tile.h"
#include "Utils.h"

/*!
 * Creates the movable at the specified coordinates.
 */
Movable::Movable(SDL_Texture * texture, int initialX, int initialY, int width, int height)
: Entity(texture, initialX, initialY, width, height) {
	
}

void Movable::update(Game* game) {
	Entity::update(game);

	//Collision handling
	if (collision) {
		//For speeds >1 the entity moves by multiple units. To simulate how the movement would play out each 1 unit step the collision
		//handling is executed everytime. This way if an entity has a high speed it still reacts to collision as if it was moving slowly.
		//The collision system in general is rather rudimentary and ineffective but it should suffice for the needs of this game.
		for (int step = 0; step < speed; step++) {
			//Resolving horizontal, vertical and diagonal collisions
			
			SDL_Point movementVector;
			movementVector.x = movementDirection.x;
			movementVector.y = movementDirection.y;

			bool diagonalCollision = false;
			bool horizontalCollision = false;
			bool verticalCollision = false;

			SDL_Rect newDiagPos = position;
			SDL_Rect newHorizontalPos = position;
			SDL_Rect newVerticalPos = position;
			
			std::vector<std::shared_ptr<Entity>> verticalCollisionCollidingEntities;
			std::vector<std::shared_ptr<Entity>> horizontalCollisionCollidingEntities;
			
			if (movementVector.x != 0 && movementVector.y != 0) {
				Utils::addVectorToRect(newDiagPos, movementVector);
				diagonalCollision = Collision::checkCollision(newDiagPos, &collidingEntities, nullptr, this, game);
			}
			if (movementVector.x != 0) {
				newHorizontalPos.x += movementVector.x;
				horizontalCollision = Collision::checkCollision(newHorizontalPos, &collidingEntities, &horizontalCollisionCollidingEntities, this, game);
			}
			if (movementVector.y != 0) {
				newVerticalPos.y += movementVector.y;
				verticalCollision = Collision::checkCollision(newVerticalPos, &collidingEntities, &verticalCollisionCollidingEntities, this, game);
			}


			//Tile corner evasion
		
			//If a movable is near a tile corner, the following code will force a movement direction
			//towards the corner of the tile to make navigating grid like pathways easier.
			//(Mainly to make it easier for the player)
			
			bool verticalCorner = false;
			Entity* cornerEntity = nullptr;
			if (verticalCollisionCollidingEntities.size() == 1 && movementVector.x == 0 && horizontalCollisionCollidingEntities.size() != 1) {
				cornerEntity = verticalCollisionCollidingEntities[0].get();
				verticalCorner = true;
			}
			if (horizontalCollisionCollidingEntities.size() == 1 && movementVector.y == 0 && verticalCollisionCollidingEntities.size() != 1) {
				cornerEntity = horizontalCollisionCollidingEntities[0].get();
				verticalCorner = false;
			}

			if (cornerEntity) {
				if (dynamic_cast<Tile*>(cornerEntity)) {
					const int cornerWidth = cornerEntity->getRect()->w / 2;
					const int cornerRemainingWidth = cornerEntity->getRect()->w - cornerWidth;
					const int cornerHeight = cornerEntity->getRect()->h / 2;
					const int cornerRemainingHeight = cornerEntity->getRect()->h - cornerHeight;

					//Creating corner zones of the tile
					SDL_Rect topLeftCorner{ cornerEntity->getRect()->x, cornerEntity->getRect()->y, cornerWidth, cornerHeight };
					SDL_Rect bottomLeftCorner{ cornerEntity->getRect()->x, cornerEntity->getRect()->y + cornerRemainingHeight, cornerWidth, cornerHeight };
					SDL_Rect bottomRightCorner{ cornerEntity->getRect()->x + cornerRemainingWidth, cornerEntity->getRect()->y + cornerRemainingHeight, cornerWidth, cornerHeight };
					SDL_Rect topRightCorner{ cornerEntity->getRect()->x + cornerRemainingWidth, cornerEntity->getRect()->y, cornerWidth, cornerHeight };

					//If the movable intersection rectangle is contained within a corner zone,
					//an appropriate direction towards the corner is forced
					SDL_Rect intersectionRectValue = SDL_Rect();
					SDL_Rect* intersectionRect = &intersectionRectValue;
					const SDL_Rect* cornerEntityRect = verticalCorner ? verticalCollisionCollidingEntities[0]->getRect() : horizontalCollisionCollidingEntities[0]->getRect();
					SDL_Rect* newMovablePositionRect = verticalCorner ? &newVerticalPos : &newHorizontalPos;
					if (SDL_IntersectRect(newMovablePositionRect, cornerEntityRect, intersectionRect) == SDL_TRUE) {
						if (Utils::rectInRect(intersectionRect, &topLeftCorner)) {
							if (verticalCorner) {
								movementVector.x = -1;
							} else {
								movementVector.y = -1;
							}
						} else
						if (Utils::rectInRect(intersectionRect, &bottomLeftCorner)) {
							if (verticalCorner) {
								movementVector.x = -1;
							} else {
								movementVector.y = 1;
							}
						} else
						if (Utils::rectInRect(intersectionRect, &bottomRightCorner)) {
							if (verticalCorner) {
								movementVector.x = 1;
							} else {
								movementVector.y = 1;
							}
						} else
						if (Utils::rectInRect(intersectionRect, &topRightCorner)) {
							if (verticalCorner) {
								movementVector.x = 1;
							} else {
								movementVector.y = -1;
							}
						}
					}
				}
			}

			//Resolving response movement

			bool movementOccurred = false;
			
			//Since we are working with integer coordinates the entity can get "stuck" on corners of other entities quite often.
			//Although it is logical to stop since going in either direction is equally adequate,
			//I'm going to force horizontal movement to resolve the collision.
			if (diagonalCollision && !horizontalCollision && !verticalCollision) {
				this->move(movementVector.x, 0); //Force horizontal movement
				movementOccurred = true;
			} else {
				if (!horizontalCollision) {
					this->move(movementVector.x, 0);
					movementOccurred = true;
				}
				if (!verticalCollision) {
					this->move(0, movementVector.y);
					movementOccurred = true;
				}
			}

			//Process collision at the new valid location to update current colliding entities.
			Collision::checkCollision(position, nullptr, &collidingEntities, this, game);
			
			if (!movementOccurred) break; //Break the collision step loop if no movement occurred
		}
	} else {
		//Just move the movable regardless of anything else
		this->move(movementDirection.x * speed, movementDirection.y * speed);
	}
}

SDL_Point Movable::getMovementDirection() const {
	return movementDirection;
}

void Movable::setMovementDirection(const SDL_Point& movementDirection) {
	this->movementDirection = movementDirection;
}

int Movable::getSpeed() const {
	return speed;
}

void Movable::setSpeed(int speed) {
	this->speed = speed;
}

bool Movable::isCollisionEnabled() const {
	return collision;
}

void Movable::setCollisionEnabled(bool collision) {
	this->collision = collision;
}
	


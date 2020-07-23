#include "Player.h"

#include "Game.h"

Player::Player(char playerId, int initialX, int initialY, SDL_Scancode actionKey, SDL_Scancode upKey, SDL_Scancode leftKey, SDL_Scancode downKey, SDL_Scancode rightKey)
: Bomberman(initialX, initialY), playerId(playerId), actionKey(actionKey), upKey(upKey), downKey(downKey), leftKey(leftKey), rightKey(rightKey) {

}

void Player::update(Game* game) {
	int dirX = 0;
	int dirY = 0;
	
	if (inputUp) dirY -= 1;
	if (inputDown) dirY += 1;
	if (inputLeft) dirX -= 1;
	if (inputRight) dirX += 1;
	
	this->movementDirection.x = dirX;
	this->movementDirection.y = dirY;

	Bomberman::update(game);
}

void Player::handleKeyDown(SDL_KeyboardEvent* event) {
	if (event->repeat == 0) {
		if (event->keysym.scancode == upKey) {
			inputUp = true;
		}
		if (event->keysym.scancode == downKey) {
			inputDown = true;
		}
		if (event->keysym.scancode == leftKey) {
			inputLeft = true;
		}
		if (event->keysym.scancode == rightKey) {
			inputRight = true;
		}
	}
}

void Player::handleKeyUp(SDL_KeyboardEvent* event) {
	if (event->repeat == 0) {
		if (event->keysym.scancode == upKey) {
			inputUp = false;
		}
		if (event->keysym.scancode == downKey) {
			inputDown = false;
		}
		if (event->keysym.scancode == leftKey) {
			inputLeft = false;
		}
		if (event->keysym.scancode == rightKey) {
			inputRight = false;
		}
	}
}

void Player::handleKeyHeldDown(const Uint8* keys) {
	if (keys[actionKey]) {
		if (availableBombs > 0) placeDownBombOnUpdate = true;	
	}
}

std::string Player::getName() const {
	return name;
}

void Player::setName(const std::string& name) {
	this->name = name;
}

std::string Player::getNameOrId() const {
	if (name.empty()) {
		return std::string(1, playerId);
	} else {
		return name;
	}
}

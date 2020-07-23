#include "Entity.h"

#include "Game.h"
#include "Utils.h"

Entity::Entity(SDL_Texture* texture, int initialX, int initialY, int width, int height)
: sprite(Sprite(texture)),
position(Utils::createSDLRect(initialX, initialY, width, height)) {
	
}

Entity::~Entity() = default;

void Entity::update(Game* game) {
	sprite.update(game->getTick());
}

void Entity::render(SDL_Renderer* renderer) {
	sprite.render(position, renderer);
}

bool Entity::isCollidable() const {
	return collidable;
}

void Entity::setCollidable(bool collidable) {
	this->collidable = collidable;
}

void Entity::setTexture(SDL_Texture * texture) {
	this->sprite.setTexture(texture);
}

int Entity::getX() const {
	return position.x;
}

int Entity::getY() const {
	return position.y;
}

int Entity::getWidth() const {
	return position.w;
}

int Entity::getHeight() const {
	return position.h;
}

int Entity::getCenterX() const {
	return position.x + position.w / 2;
}

int Entity::getCenterY() const {
	return position.y + position.h / 2;
}

const SDL_Rect* Entity::getRect() const {
	return &position;
}

void Entity::setX(int x) {
	position.x = x;
}

void Entity::setY(int y) {
	position.y = y;
}

void Entity::setWidth(int w) {
	position.w = w; 
}

void Entity::setHeight(int h) {
	position.h = h;
}

Entity* Entity::centerInRect(SDL_Rect rect) {
	Utils::centerRectInRect(&position, rect);
	return this;
}

void Entity::placeAt(int x, int y) {
	position.x = x;
	position.y = y;
}


void Entity::move(int x, int y) {
	position.x = position.x + x;
	position.y = position.y + y;
}

bool Entity::isRenderingExternal() const {
	return renderingExternal;
}

void Entity::setRenderingExternal(bool renderingExternal) {
	this->renderingExternal = renderingExternal;
}

bool Entity::isForegroundRender() const {
	return foregroundRender;
}

void Entity::setForegroundRender(bool foregroundRender) {
	this->foregroundRender = foregroundRender;
}

bool Entity::isToBeRemoved() const {
	return toBeRemoved;
}

void Entity::setToBeRemoved(bool toBeRemoved) {
	this->toBeRemoved = toBeRemoved;
}
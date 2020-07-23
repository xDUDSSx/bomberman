#include "Sprite.h"

#include "Constants.h"
#include "Tween.h"
#include "Utils.h"

Sprite::Sprite(SDL_Texture* texture) : activeTexture(texture) {
	textureCoordinates = Utils::createSDLRect(0, 0, 0, 0);
	if (texture) {
		SDL_QueryTexture(texture, NULL, NULL, &textureCoordinates.w, &textureCoordinates.h);
	}
}

void Sprite::update(unsigned int tick) {
	if (animated) {
		animationTick++;
		if (animationTick >= animationTicksPerFrame) {
			textureFrameIndex++;
			if (textureFrameIndex >= static_cast<int>(textureFrames.size())) {
				if (repeatAnimation) {
					textureFrameIndex = 0;
				} else {
					animationFinished = true;
					textureFrameIndex = static_cast<int>(textureFrames.size()) - 1;
				}
			}
			animationTick = 0;
			activeTexture = textureFrames[textureFrameIndex];
		}
	}
	effectTick++;
	switch (activeEffect) {
		case BREATHING:
			this->setScale(0.7 + Utils::range(Tween::sineBounce(effectTick, effectDuration), 0.0, 1.0, 0.0, 0.3));
			break;
		case EXPLOSION:
			scalingOrigin = BOTTOM;
			this->setScaleX(1.0 - Utils::range(Tween::linear(effectTick, effectDuration), 0.0, 1.0, 0.0, 0.8));
			this->setScaleY(1.0 + Utils::range(Tween::easeInExpo(effectTick, effectDuration),0.0, 1.0, 0.0, 0.8));
			break;
		case BLINKING:
			if (effectTick % effectDuration >= effectDuration / 2) {
				this->setScale(1);
			} else {
				this->setScale(0);
			}
			break;
		case NONE:
		default:;
	}
}

void Sprite::render(SDL_Rect& worldCoordinates, SDL_Renderer* renderer) const {
	if (activeTexture) {
		//Setting the sprite alpha value to the texture
		SDL_SetTextureAlphaMod(activeTexture, alpha);

		//Rendering the active texture, optionally rotating or flipping it according to the sprite settings
		SDL_Rect scaled = scaleRectangle(&worldCoordinates, scaleX, scaleY, scalingOrigin);
		SDL_RenderCopyEx(renderer, activeTexture, &textureCoordinates, &scaled, rotationAngle, rotationOrigin, renderFlip);
	}
}

void Sprite::setTexture(SDL_Texture * texture) {
	activeTexture = texture;
	textureCoordinates = Utils::createSDLRect(0, 0, 0, 0);
	SDL_QueryTexture(texture, NULL, NULL, &textureCoordinates.w, &textureCoordinates.h);
}

void Sprite::setRotationOrigin(SDL_Point point) {
	rotationOrigin = &point;
}

void Sprite::setRotation(int angle) {
	rotationAngle = angle;
}

void Sprite::setAlpha(int alpha) {
	this->alpha = alpha;
}

void Sprite::setScale(double scale) {
	this->scaleX = scale;
	this->scaleY = scale;
}

void Sprite::setScaleX(double scaleX) {
	this->scaleX = scaleX;
}
void Sprite::setScaleY(double scaleY) {
	this->scaleY = scaleY;
}

void Sprite::setFlip(SDL_RendererFlip flip) {
	renderFlip = flip;
}

void Sprite::setAnimated(bool animated) {
	this->animated = animated;
}

void Sprite::setRepeatAnimation(bool repeat) {
	repeatAnimation = repeat;
}

void Sprite::setAnimationSpeed(int ticksPerFrame) {
	animationTicksPerFrame = ticksPerFrame;
}

void Sprite::addAnimationFrame(SDL_Texture * frame) {
	textureFrames.push_back(frame);
}

void Sprite::clearAnimationFrames() {
	textureFrames.clear();
	textureFrameIndex = 0;
}

void Sprite::setEffect(SpriteEffect effect) {
	setEffect(effect, Constants::DEFAULT_EFFECT_DURATION);
}

void Sprite::setEffect(SpriteEffect effect, int effectDuration) {
	setEffect(effect, effectDuration, true);
}

void Sprite::setEffect(SpriteEffect effect, int effectDuration, bool resetEffectTick) {
	activeEffect = effect;
	this->effectDuration = effectDuration;
	if (resetEffectTick) effectTick = 0;
}

SDL_Texture* Sprite::getTexture() const {
	return activeTexture;
}

SDL_Point* Sprite::getRotationOrigin() const {
	return rotationOrigin;
}

double Sprite::getRotationAngle() const {
	return rotationAngle;
}

int Sprite::getAlpha() const {
	return alpha;
}

bool Sprite::isAnimated() const {
	return animated;
}

bool Sprite::isRepeatingAnimation() const {
	return repeatAnimation;
}

bool Sprite::isAnimationFinished() const {
	return animationFinished;
}

int Sprite::getAnimationSpeed() const {
	return animationTicksPerFrame;
}

SDL_Rect Sprite::scaleRectangle(SDL_Rect* rectangle, double scaleX, double scaleY, ScaleOrigin scaleOrigin) {
	SDL_Point origin;
	switch (scaleOrigin) {
		case TOP:
			origin.x = rectangle->x + rectangle->w / 2;
			origin.y = rectangle->y;
			break;
		case TOP_LEFT:
			origin.x = rectangle->x;
			origin.y = rectangle->y;
			break;
		case LEFT:
			origin.x = rectangle->x;
			origin.y = rectangle->y + rectangle->h / 2;
			break;
		case BOTTOM_LEFT:
			origin.x = rectangle->x;
			origin.y = rectangle->y + rectangle->h;
			break;
		case BOTTOM:
			origin.x = rectangle->x + rectangle->w / 2;
			origin.y = rectangle->y + rectangle->h;
			break;
		case BOTTOM_RIGHT:
			origin.x = rectangle->x + rectangle->w;
			origin.y = rectangle->y + rectangle->h;
			break;
		case RIGHT:
			origin.x = rectangle->x + rectangle->w;
			origin.y = rectangle->y + rectangle->h / 2;
			break;
		case TOP_RIGHT:
			origin.x = rectangle->x + rectangle->w;
			origin.y = rectangle->y;
			break;
		case CENTER:
		default:
			origin.x = rectangle->x + rectangle->w / 2;
			origin.y = rectangle->y + rectangle->h / 2;
	}

	SDL_Point tlCornerVector = { rectangle->x - origin.x, rectangle->y - origin.y };
	SDL_Point trCornerVector = { rectangle->x + rectangle->w - origin.x, rectangle->y - origin.y };
	SDL_Point blCornerVector = { rectangle->x - origin.x, rectangle->y + rectangle->h - origin.y };
	//SDL_Point brCornerVector = { rectangle->x + rectangle->w - origin.x, rectangle->y + rectangle->h - origin.y };

	tlCornerVector = Utils::multiplyVector(&tlCornerVector, scaleX, scaleY);
	trCornerVector = Utils::multiplyVector(&trCornerVector, scaleX, scaleY);
	blCornerVector = Utils::multiplyVector(&blCornerVector, scaleX, scaleY);

	tlCornerVector = Utils::addVectors(tlCornerVector, origin);
	trCornerVector = Utils::addVectors(trCornerVector, origin);
	blCornerVector = Utils::addVectors(blCornerVector, origin);

	const int x = tlCornerVector.x;
	const int y = tlCornerVector.y;
	const int width = trCornerVector.x - x;
	const int height = blCornerVector.y - y;

	const SDL_Rect scaledRect = {
		x, y, width, height
	};

	return scaledRect;
}

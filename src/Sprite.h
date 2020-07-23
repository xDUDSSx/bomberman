#pragma once

#include <SDL_render.h>

#include <vector>

/*!
 * A helper object packing data about a texture and offers means to manipulate it.
 * It handles its dimensions, rotation, animation and special effects.
 */
class Sprite {
public:
	/*!
	 * Sprite effects are animated transformations that are applied to the sprite texture when rendering.
	 * An effect can be set using the setEffect() method.
	*/
	enum SpriteEffect {
		NONE,	
		BREATHING,
		EXPLOSION,
		BLINKING
	};

	/*!
	 * An origin position used for scaling the sprite.
	 * \see scaleRectangle()
	 */
	enum ScaleOrigin {
		TOP, TOP_LEFT, LEFT, BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT, RIGHT, TOP_RIGHT, CENTER
	};
	
private:
	int animationTick = 0;
	int textureFrameIndex = 0;
	int effectTick = 0;
	
	/*! Set of textures used for animation */
	std::vector<SDL_Texture*> textureFrames{};

	/*! Rectangle specifying the texture region in texture space */
	SDL_Rect textureCoordinates;

	/*! The texture used when rendering this entity */
	SDL_Texture* activeTexture;

	SDL_Point* rotationOrigin = nullptr;
	double rotationAngle = 0;

	/*! Transparency value of the sprite. */
	int alpha = 255;

	double scaleX = 1.0;
	double scaleY = 1.0;

	ScaleOrigin scalingOrigin = CENTER;
	
	SDL_RendererFlip renderFlip = SDL_FLIP_NONE;
	
	/*!
	 * Flag indicating that the sprite is animated. The method addAnimationFrame
	 * can be used to load animation frames.
	 */
	bool animated = false;

	/*! Determines whether the animation will repeat or if it should stop at the last frame. */
	bool repeatAnimation = true;

	/*! Flag which is set to true when the sprite animation reaches the last frame and repeat is set to false. */
	bool animationFinished = false;

	/*! Number of update ticks each animation frame is displayed for. */
	int animationTicksPerFrame = 30;

	/*! Duration of a single effect loop. */
	int effectDuration = 60;

	/*! Current active effect */
	SpriteEffect activeEffect = NONE;
public:
	Sprite(SDL_Texture* texture);

	/*!
	 * Handling animation, switching to the next frame or reseting to the first one if necessary
	 */
	void update(unsigned int tick);
	
	/*!
	 * Renders the sprite using its current parameters.
	 * \param worldCoordinates Target world coordinates where the sprite texture should be rendered at.
	 * \param renderer The SDL renderer used for rendering.
	 */
	void render(SDL_Rect& worldCoordinates, SDL_Renderer* renderer) const;

	/*!
	 * Sets the sprite activeTexture, recalculating texture coordinates.
	 */
	void setTexture(SDL_Texture* texture);
	void setRotationOrigin(SDL_Point point);
	
	/*!
	 * Set angle of rotation in degrees, rotating clockwise.
	 */
	void setRotation(int angle);

	/*!
	 * Sets texture alpha in range 0-255
	 */
	void setAlpha(int alpha);

	void setScale(double scale);
	void setScaleX(double scaleX);
	void setScaleY(double scaleY);
	
	/*!
	 * Sets SDL texture flip
	 */
	void setFlip(SDL_RendererFlip flip);

	/*!
	 * Enables sprite animation. Use the method addAnimationFrame
	 * to set animation frames.
	 */
	void setAnimated(bool animated);
	
	/*!
	 * Determines whether the animation will repeat or if it should stop at the last frame.
	 */
	void setRepeatAnimation(bool repeat);

	/*!
	 * Sets the number of update ticks each animation frame is displayed for.
	 */
	void setAnimationSpeed(int ticksPerFrame);
	
	/*!
	 * Adds an animation frame to the sprite. The animation frames should be added
	 * in the order in which they are to be displayed.
	 */
	void addAnimationFrame(SDL_Texture* frame);
	void clearAnimationFrames();

	/*!
	 * Set the effect for this sprite.
	 * 
	 * \see SpriteEffect
	 * \param effect The effect type
	 */
	void setEffect(SpriteEffect effect);

	/*!
	 * Set the effect for this sprite.
	 * 
	 * \see SpriteEffect
	 * \param effect The effect type
 	 * \param effectDuration Effect duration. In essence its speed.
	 */
	void setEffect(SpriteEffect effect, int effectDuration);

	/*!
	 * Set the effect for this sprite.
	 * 
	 * \see SpriteEffect
	 * \param effect The effect type
	 * \param effectDuration Effect duration. In essence its speed.
	 * \param resetEffectTick Whether the internal effect tick should be reset or not.
	 *		  If set to true the effect will start from its beginning after calling this method,
	 *		  otherwise it might start in any phase depending on the game tick.
	 */
	void setEffect(SpriteEffect effect, int effectDuration, bool resetEffectTick);

	SDL_Texture* getTexture() const;
	
	SDL_Point* getRotationOrigin() const;
	double getRotationAngle() const;
	
	int getAlpha() const;
	
	bool isAnimated() const;
	bool isRepeatingAnimation() const;
	bool isAnimationFinished() const;
	int getAnimationSpeed() const;
private:

	/*!
	 * Scales a given rectangle along the x and y axis by a specified factor.
	 * The scaling is done from an origin point.
	 * Factors of 1.0 would return an identical rectangle.
	 *
	 * \param rectangle The original rectangle to scale.
	 * \param scaleX The X axis scaling factor.
	 * \param scaleY The Y axis scaling factor.
	 * \param scaleOrigin The scaling origin point position.
	 * \return A scaled version of the original rectangle.
	 * \see ScaleOrigin
	 */
	static SDL_Rect scaleRectangle(SDL_Rect* rectangle, double scaleX, double scaleY, ScaleOrigin scaleOrigin);
};

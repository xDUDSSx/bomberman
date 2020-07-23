#pragma once

#include "Entity.h"

/*!
 * A simple entity used as a visual effect. Can display static and animated textures.
 * Can be set to destroy itself after an animation has been played or after a certain time has elapsed.
 */
class Particle : public Entity {
protected:
	bool vanishEnabled = false;
	int ticksToVanish = 0;

	bool alphaVanish = false;
	int alphaVanishTick = 0;
public:
	/*!
	 * Creates the particle with a static texture and dimensions.
	 * 
	 * \param texture The particle texture
	 * \param x	Top left corner X coordinate
	 * \param y Top left corner Y coordinate
	 * \param width Width of the entity
	 * \param height Height of the entity
	 */
	Particle(SDL_Texture* texture, int x, int y, int width, int height);

	/*!
	 * Creates the particle with a static texture and dimensions.
	 * Furthermore the lifetime parameter specifies time before the particle destroyes itself. 
	 * 
	 * \param texture The particle texture
	 * \lifetime Lifetime of the particle
	 * \param x	Top left corner X coordinate
	 * \param y Top left corner Y coordinate
	 * \param width Width of the entity
	 * \param height Height of the entity
	 */
	Particle(SDL_Texture* texture, int lifetime, int x, int y, int width, int height);

	/*!
	 * Creates an animated particle with specified dimensions.
	 * 
	 * \param frames The frames of the animation in order
	 * \param frameDuration Number of frames each animation frame will be displayed on screen.
	 * \param repeat Whether to repeat the animation indefinetly. Setting this to false will destroy the entity once the animation finishes.
	 * \param x Top left corner X coordinate
	 * \param y Top left corner Y coordinate
	 * \param width Width of the entity
	 * \param height Height of the entity
	 */
	Particle(std::vector<SDL_Texture*> frames, int frameDuration, bool repeat, int x, int y, int width, int height);

	/*!
	 * Creates an animated particle with specified dimensions.
	 * Furthermore the lifetime parameter specifies time before the particle destroyes itself.
	 * Setting lifetime to other value than 0 will override the destruction of the particle on repeat.
	 * 
	* \param frames The frames of the animation in order
	 * \param frameDuration Number of frames each animation frame will be displayed on screen.
	 * \param repeat Whether to repeat the animation indefinetly. Setting this to false will destroy the entity once the animation finishes.
	 * \param lifetime Lifetime of the entity, will override the repeat destruction behaviour.
	 * \param x Top left corner X coordinate
	 * \param y Top left corner Y coordinate
	 * \param width Width of the entity
	 * \param height Height of the entity
	 */
	Particle(std::vector<SDL_Texture*> frames, int frameDuration, bool repeat, int lifetime, int x, int y, int width, int height);

	void update(Game* game) override;

	/*!
	 * Makes the entity fade out upon destruction. Only works if a specific lifetime other than 0 was set.
	 * 
	 * \param ticksRemaining Number of frames before destruction the fade out will start.
	 */
	void enableAlphaVanish(int ticksRemaining);
};
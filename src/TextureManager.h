#pragma once

#include "SDL.h"

#include <vector>

#include "Particle.h"

/*!
 * Class responsible for loading textures and provides static access to them. 
 */
class TextureManager {
public:
	static SDL_Texture* bombermanTexture;
	static SDL_Texture* bombermanTextureTinted;
	
	static SDL_Texture* bombNoFireTexture;
	static SDL_Texture* bombFrame1;
	static SDL_Texture* bombFrame2;
	static SDL_Texture* bombFrame3;

	static SDL_Texture* wallTexture;
	static SDL_Texture* indestructibleEdgeWallTexture;
	static SDL_Texture* indestructibleWallTexture;

	static std::vector<SDL_Texture*> wallExplosionAnimation;

	static SDL_Texture* tileTexture;
	static SDL_Texture* tileTextureDark;

	static SDL_Texture* defaultTexture;

	static SDL_Texture* bombCountDownTexture;
	static SDL_Texture* bombCountUpTexture;

	static SDL_Texture* bombCountIcon;
	static SDL_Texture* bombPowerIcon;
	
	static SDL_Texture* bombPowerDownTexture;
	static SDL_Texture* bombPowerUpTexture;

	static SDL_Texture* speedPowerUp;
	static SDL_Texture* speedPowerUpIcon;

	static SDL_Texture* explosionCenter;
	static SDL_Texture* explosionEnd;
	static SDL_Texture* explosionSide;

	static SDL_Texture* health;

	static SDL_Texture* playerIcon;
	static SDL_Texture* computerIcon;

	static SDL_Texture* cross;

	/*!
	 * Loads all textures if they aren't loaded already.
	 */
	static void loadAllTextures(SDL_Renderer* renderer);

	/*!
	 * Frees texture resources.
	 */
	static void dispose();

	static Particle* createWallDestructionEffect();
private:
	static bool texturesLoaded;

	/*!
	 * Loads a texture from an image file.
	 * 
	 * \param path Path to the image file.
	 * \param renderer The renderer.
	 */
	static SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
};
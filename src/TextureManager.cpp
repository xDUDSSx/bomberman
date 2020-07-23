#include "TextureManager.h"

#include "SDL_image.h"

#include <iostream>

SDL_Texture* TextureManager::bombermanTexture;
SDL_Texture* TextureManager::bombermanTextureTinted;

SDL_Texture* TextureManager::bombNoFireTexture;
SDL_Texture* TextureManager::bombFrame1;
SDL_Texture* TextureManager::bombFrame2;
SDL_Texture* TextureManager::bombFrame3;

SDL_Texture* TextureManager::wallTexture;
SDL_Texture* TextureManager::indestructibleEdgeWallTexture;
SDL_Texture* TextureManager::indestructibleWallTexture;

std::vector<SDL_Texture*> TextureManager::wallExplosionAnimation;

SDL_Texture* TextureManager::tileTexture;
SDL_Texture* TextureManager::tileTextureDark;

SDL_Texture* TextureManager::defaultTexture;

SDL_Texture* TextureManager::bombCountDownTexture;
SDL_Texture* TextureManager::bombCountUpTexture;

SDL_Texture* TextureManager::bombCountIcon;
SDL_Texture* TextureManager::bombPowerIcon;

SDL_Texture* TextureManager::bombPowerDownTexture;
SDL_Texture* TextureManager::bombPowerUpTexture;

SDL_Texture* TextureManager::speedPowerUp;
SDL_Texture* TextureManager::speedPowerUpIcon;

SDL_Texture* TextureManager::explosionCenter;
SDL_Texture* TextureManager::explosionEnd;
SDL_Texture* TextureManager::explosionSide;

SDL_Texture* TextureManager::health;

SDL_Texture* TextureManager::playerIcon;
SDL_Texture* TextureManager::computerIcon;

SDL_Texture* TextureManager::cross;

bool TextureManager::texturesLoaded = false;

void TextureManager::loadAllTextures(SDL_Renderer* renderer) {
	if (!texturesLoaded) {
		std::cout << "Loading textures" << std::endl;

		bombermanTexture = loadTexture("res/player16.png", renderer);
		bombermanTextureTinted = loadTexture("res/player16.png", renderer);
		SDL_SetTextureColorMod(bombermanTextureTinted, 255, 128, 128);

		bombNoFireTexture = loadTexture("res/bomb16_0.png", renderer);
		bombFrame1 = loadTexture("res/bomb16_1.png", renderer);
		bombFrame2 = loadTexture("res/bomb16_2.png", renderer);
		bombFrame3 = loadTexture("res/bomb16_3.png", renderer);

		wallTexture = loadTexture("res/wall16.png", renderer);
		indestructibleWallTexture = loadTexture("res/inwall16_dark.png", renderer);
		indestructibleEdgeWallTexture = loadTexture("res/inwall16_edge2.png", renderer);
		
		wallExplosionAnimation.push_back(loadTexture("res/wall2.png", renderer));
		wallExplosionAnimation.push_back(loadTexture("res/wall3.png", renderer));
		wallExplosionAnimation.push_back(loadTexture("res/wall4.png", renderer));
		wallExplosionAnimation.push_back(loadTexture("res/wall5.png", renderer));
		
		tileTexture = loadTexture("res/tile16.png", renderer);
		tileTextureDark = loadTexture("res/tile16.png", renderer);
		SDL_SetTextureColorMod(tileTextureDark, 128, 128, 128);

		defaultTexture = loadTexture("res/default16.png", renderer);

		bombCountDownTexture = loadTexture("res/bombCountDown16.png", renderer);
		bombCountUpTexture = loadTexture("res/bombCountUp16.png", renderer);

		bombCountIcon = loadTexture("res/bombCountIcon.png", renderer);
		bombPowerIcon = loadTexture("res/bombPowerIcon.png", renderer);
		
		bombPowerDownTexture = loadTexture("res/bombPowerDown16.png", renderer);
		bombPowerUpTexture = loadTexture("res/bombPowerUp16.png", renderer);

		speedPowerUp = loadTexture("res/speed16.png", renderer);
		speedPowerUpIcon = loadTexture("res/speedPowerUpIcon.png", renderer);

		explosionCenter = loadTexture("res/explosionCenter.png", renderer);
		explosionEnd = loadTexture("res/explosionEnd.png", renderer);
		explosionSide = loadTexture("res/explosionSide.png", renderer);

		health = loadTexture("res/health.png", renderer);

		playerIcon = loadTexture("res/player22.png", renderer);
		computerIcon = loadTexture("res/computer22.png", renderer);

		cross = loadTexture("res/cross64.png", renderer);
		
		texturesLoaded = true;
	} else {
		std::cout << "Textures loaded already!" << std::endl;
	}
}

void TextureManager::dispose() {
	if (texturesLoaded) {
		std::cout << "Disposing textures" << std::endl;

		SDL_DestroyTexture(bombermanTexture);
		SDL_DestroyTexture(bombermanTextureTinted);
		SDL_DestroyTexture(bombNoFireTexture);
		SDL_DestroyTexture(bombFrame1);
		SDL_DestroyTexture(bombFrame2);
		SDL_DestroyTexture(bombFrame3);
		SDL_DestroyTexture(wallTexture);
		SDL_DestroyTexture(indestructibleWallTexture);
		SDL_DestroyTexture(indestructibleEdgeWallTexture);
		for (auto& texture : wallExplosionAnimation) {
			SDL_DestroyTexture(texture);
		}
		SDL_DestroyTexture(tileTexture);
		SDL_DestroyTexture(tileTextureDark);
		SDL_DestroyTexture(defaultTexture);
		SDL_DestroyTexture(bombCountDownTexture);
		SDL_DestroyTexture(bombCountUpTexture);
		SDL_DestroyTexture(bombCountIcon);
		SDL_DestroyTexture(bombPowerIcon);
		SDL_DestroyTexture(bombPowerDownTexture);
		SDL_DestroyTexture(bombPowerUpTexture);
		SDL_DestroyTexture(speedPowerUp);
		SDL_DestroyTexture(speedPowerUpIcon);
		SDL_DestroyTexture(explosionCenter);
		SDL_DestroyTexture(explosionEnd);
		SDL_DestroyTexture(explosionSide);
		SDL_DestroyTexture(health);
		SDL_DestroyTexture(playerIcon);
		SDL_DestroyTexture(computerIcon);
		SDL_DestroyTexture(cross);
	} else {
		std::cout << "TextureManager dispose was called but textures haven't been loaded yet!" << std::endl;
	}
}

Particle * TextureManager::createWallDestructionEffect() {
	return new Particle(wallExplosionAnimation, 6, false, 0, 32, 32, 64, 64);
}

SDL_Texture * TextureManager::loadTexture(const char * path, SDL_Renderer* renderer) {
	std::cout << "Loading texture: " << path << std::endl;
	SDL_Surface* newSurface = IMG_Load(path);
	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, newSurface);
	SDL_FreeSurface(newSurface);

	return newTexture;
}
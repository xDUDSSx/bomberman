#include "GameInfoBar.h"

#include "Game.h"
#include "Player.h"
#include "Computer.h"
#include "TextureManager.h"
#include "Renderer.h"

GameInfoBar::GameInfoBar(int x, int y, int width, int height)
: position(SDL_Rect{x, y, width, height}), cellWidth(width / 4) {
	
}

void GameInfoBar::render(SDL_Renderer* renderer, const Game* game) const {
	int index = 0;
	for (auto player : game->players) {
		renderBombermanInfo(index, player.get(), renderer, game);
		index++;
	}
	for (auto computer : game->computers) {
		renderBombermanInfo(index, computer.get(), renderer, game);
		index++;
	}
}

void GameInfoBar::renderBombermanInfo(int index, Bomberman* bomberman, SDL_Renderer* renderer, const Game* game) const {
	if (!bomberman) return;
	
	const SDL_Rect positionRect = {index * cellWidth, 0, cellWidth, this->position.h};
	SDL_Rect iconRect = { positionRect.x + 10, positionRect.y + positionRect.h / 2 - 22, 44, 44};

	if (dynamic_cast<Player*>(bomberman)) {
		Renderer::renderTexture(TextureManager::playerIcon, iconRect, renderer);
	} else {
		Renderer::renderTexture(TextureManager::computerIcon, iconRect, renderer);
	}
	
	SDL_Rect healthIconRect = { iconRect.x + iconRect.w + 5, iconRect.y, 16, 16 };
	Renderer::renderTexture(TextureManager::health, healthIconRect, renderer);

	SDL_Rect healthTextRect{ healthIconRect.x + healthIconRect.w + 5, healthIconRect.y, 16, 16 };
	Renderer::renderText(std::to_string(bomberman->getHealth()), healthTextRect, Renderer::TextAlign::CenterLeft, game->getFont(), game->getFontColor(), renderer);

	SDL_Rect bombsRect = { iconRect.x + iconRect.w + 5, iconRect.y + 16 + 1, 16, 16 };
	Renderer::renderTexture(TextureManager::bombCountIcon, bombsRect, renderer);

	SDL_Rect bombTextRect{ bombsRect.x + bombsRect.w + 5, bombsRect.y, 16, 16 };
	Renderer::renderText(std::to_string(bomberman->getAvailableBombs() + bomberman->getPlacedBombs()), bombTextRect, Renderer::TextAlign::CenterLeft, game->getFont(), game->getFontColor(), renderer);

	SDL_Rect speedRect = { healthTextRect.x + healthTextRect.w + 5, healthTextRect.y, 16, 16 };
	Renderer::renderTexture(TextureManager::speedPowerUpIcon, speedRect, renderer);

	SDL_Rect speedTextRect{ speedRect.x + speedRect.w + 5, speedRect.y, 16, 16 };
	Renderer::renderText(std::to_string(bomberman->getSpeed()), speedTextRect, Renderer::TextAlign::CenterLeft, game->getFont(), game->getFontColor(), renderer);

	SDL_Rect bombPowerRect = { speedRect.x, speedRect.y + 16 + 1, 16, 16 };
	Renderer::renderTexture(TextureManager::bombPowerIcon, bombPowerRect, renderer);

	SDL_Rect bombPowerTextRect{ bombPowerRect.x + bombPowerRect.w + 5, bombPowerRect.y, 16, 16 };
	Renderer::renderText(std::to_string(bomberman->getBombPowerLevel()), bombPowerTextRect, Renderer::TextAlign::CenterLeft, game->getFont(), game->getFontColor(), renderer);
	
	SDL_Rect scoreTextRect{ iconRect.x + iconRect.w + 5, iconRect.y + 32 + 2, 16, 16 };
	Renderer::renderText(std::to_string(bomberman->getScore()), scoreTextRect, Renderer::TextAlign::CenterLeft, game->getFont(), game->getFontColor(), renderer);

	if (bomberman->getHealth() < 1) {
		Renderer::renderTexture(TextureManager::cross, iconRect, renderer);
	}

	if (Player* player = dynamic_cast<Player*>(bomberman)) {
		SDL_Rect nameTextRect{ positionRect.x + 6, positionRect.y + 2,positionRect.w - 12, 16 };
		Renderer::renderText(player->getNameOrId(), nameTextRect, Renderer::TextAlign::TopCenter, game->getFont(), game->getFontColor(), renderer);
	}

	if (Computer* computer = dynamic_cast<Computer*>(bomberman)) {
		SDL_Rect nameTextRect{ positionRect.x + 6, positionRect.y + 2,positionRect.w - 12, 16 };
		Renderer::renderText("Computer " + std::to_string(computer->getID()), nameTextRect, Renderer::TextAlign::TopCenter, game->getFont(), game->getFontColor(), renderer);
	}
}

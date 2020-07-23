#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>

#include "Bomberman.h"
#include "Entity.h"

class Computer;
class Player;

/*!
 * A UI element used to indicate player / computer status.
 */
class GameInfoBar {
private:
	SDL_Rect position{};
	int cellWidth;
public:
	GameInfoBar(int x, int y, int width, int height);
	
	void render(SDL_Renderer* renderer, const Game* game) const;

	/*!
	 * Displays game information for the bomberman. (Player or Computer)
	 * 
	 * \param index Index of the info display box.
	 * \param bomberman The bomberman.
	 * \param renderer The renderer.
	 * \param game The game.
	 */
	void renderBombermanInfo(int index, Bomberman* bomberman, SDL_Renderer* renderer, const Game* game) const;
};

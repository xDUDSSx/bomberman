#include "Renderer.h"

#include "TextureManager.h"

void Renderer::renderText(std::string text, SDL_Rect& targetRect, TextAlign textAlign, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
	const char* cText = text.c_str();
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, cText, color);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	int textWidth = 0;
	int textHeight = 0;
	TTF_SizeText(font, cText, &textWidth, &textHeight);

	int textureX;
	int textureY;
	const int textureWidth = textWidth;
	const int textureHeight = textHeight;
	
	switch (textAlign) {
		case TopLeft:
			textureX = targetRect.x;
			textureY = targetRect.y;
			break;
		case TopCenter:
			textureX = targetRect.x + targetRect.w / 2 - textWidth / 2;
			textureY = targetRect.y;
			break;
		case CenterCenter:
			textureX = targetRect.x + targetRect.w / 2 - textWidth / 2;
			textureY = targetRect.y + targetRect.h / 2 - textHeight / 2;
			break;
		case CenterLeft:
			textureX = targetRect.x;
			textureY = targetRect.y + targetRect.h / 2 - textHeight / 2;
			break;
		default:
			textureX = targetRect.x;
			textureY = targetRect.y;
			break;
	}
	
	SDL_Rect textTextureRect{ textureX, textureY, textureWidth, textureHeight };
	SDL_RenderCopy(renderer, textTexture, NULL, &textTextureRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

void Renderer::renderTexture(SDL_Texture* texture, SDL_Rect& rect, SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

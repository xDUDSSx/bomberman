#pragma once

#include <SDL_render.h>
#include <SDL_ttf.h>

#include <string>

/*!
 * Utility class for static helper render functions.
 */
class Renderer {
public:
	/*!
	 * TopLeft = Aligns top vertically, left horizontally
	 * TopCenter = Aligns top vertically, center horizontally
	 * CenterCenter = Centers text in the bounding rectangle
	 * CenterLeft = Centered vertically, left horizontally
	 */
	enum TextAlign {
		TopLeft, TopCenter, CenterCenter, CenterLeft
	};

	/*!
	 * Renders aligned text.
	 * 
	 * \param text The text content.
	 * \param targetRect Destination rectangle, width and height may be optional based on alignment.
	 * \param textAlign Text alignment.
	 * \param font The font of the text.
	 * \param color Color of the text.
	 * \param renderer The renderer.
	 * \see TextAlign
	 */
	static void renderText(std::string text, SDL_Rect& targetRect, TextAlign textAlign,TTF_Font* font, SDL_Color color, SDL_Renderer* renderer);

	/*!
	 * Renders a texture.
	 * 
	 * \param texture The texture to render.
	 * \param rect Position and dimensions of the texture.
	 * \param renderer The renderer.
	 */
	static void renderTexture(SDL_Texture* texture, SDL_Rect& rect, SDL_Renderer* renderer);
};

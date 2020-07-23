#pragma once

#include "Sprite.h"

class Game;

/*!
 *	Entity resembles an object in the game world.
 */
class Entity {
protected:
	/*! The texture representation of the entity. */
	Sprite sprite;
	
	/*!	Rectangle representing the position of the entity in the game world. */
	SDL_Rect position;
	
	/*! Whether collision aware entities can collide with this entity. */
	bool collidable = true;

	/*! Indicates where the entity should be drawn during a render call. */
	bool foregroundRender = false;

	/*! Boolean flag indicating that this entity should be removed from its manager. An optimisation effort.*/
	bool toBeRemoved = false;
	
	/*!
	 *	Since rendering order is very important as it can separate the renderer image
	 *	into layers, there is the renderingExternal flag that can inform an entity manager
	 *	that the entity rendering is handled by a different manager.
	 */
	bool renderingExternal = false;
public:
	/*!
	 * Creates an entity with specified dimensions and position.
	 * 
	 * \param texture Texture representing the entity.
	 * \param initialX X coordinate
	 * \param initialY Y coordinate
	 * \param width
	 * \param height
	 */
	Entity(SDL_Texture* texture, int initialX, int initialY, int width, int height);

	virtual ~Entity();

	/*!
	 * Calls an update on the entity. The entity can do game logic here.
	 * \param game Game object used to access information about the game world.
	 */
	virtual void update(Game* game);
	virtual void render(SDL_Renderer* renderer);

	/*! Whether collision aware entities can collide with this entity. */
	bool isCollidable() const;
	void setCollidable(bool collidable);

	/*!
	 * Replaces the current texture of the entity.
	 */
	void setTexture(SDL_Texture* texture);

	/*! Returns the top-left corner X coordinate. */
	int getX() const;
	/*! Returns the top-left corner Y coordinate. */
	int getY() const;

	int getWidth() const;
	int getHeight() const;

	/*! Returns the entity rect center X coordinate. */
	int getCenterX() const;
	/*! Returns the entity rect center Y coordinate. */
	int getCenterY() const;

	/*!
	 * Returns the entity position and dimensions in an SDL_Rect.
	 */
	const SDL_Rect* getRect() const;

	/*! Set the top-left corner X coordinate. */
	void setX(int x);
	/*! Set the top-left corner Y coordinate. */
	void setY(int y);
	
	void setWidth(int w);
	void setHeight(int h);
	
	/*!
	 * Centers the entity position in a specified rectangle.
	 * \return This entity for chaining.
	 */
	Entity* centerInRect(SDL_Rect rect);

	/*!
	 * Moves the entity to the desired coordinates.
	 */
	void placeAt(int x, int y);

	/*!
	 * Moves the entity by a vector.
	 */
	void move(int x, int y);

	/*! Whether the entity rendering is handled by a different manager. */
	bool isRenderingExternal() const;

	/*! Sets whether the entity rendering is handled by a different manager. */
	void setRenderingExternal(bool renderingExternal);

	/*! Indicates whether the entity should be rendered in the foreground. */
	bool isForegroundRender() const;

	/*! Sets whether the entity should be rendered in the foreground. */
	void setForegroundRender(bool foregroundRender);

	/*! Whether this entity should be removed from its manager. */
	bool isToBeRemoved() const;

	/*! Sets whether this entity should be removed from its manager. */
	void setToBeRemoved(bool toBeRemoved);
};
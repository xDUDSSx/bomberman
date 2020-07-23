#pragma once

#include "SDL.h"

#include <cmath>
#include <iostream>

/*!
 * A class for static utility functions.
 */
class Utils {
public:
	/*!
	 * Aligns a rectangle in center of another.
	 */
	static void centerRectInRect(SDL_Rect* centerRect, SDL_Rect outerRect) {
		centerRect->x = (outerRect.x + outerRect.w / 2) - centerRect->w / 2;
		centerRect->y = (outerRect.y + outerRect.h / 2) - centerRect->h / 2;
	}

	/*!
	 * Moves a rectangle by a vector.
	 */
	static void addVectorToRect(SDL_Rect& rect, SDL_Point& vector) {
		rect.x += vector.x;
		rect.y += vector.y;
	}

	/*!
	 * Adds two vectors.
	 *
	 * \return The new vector.
	 */
	static SDL_Point addVectors(const SDL_Point& v1, const SDL_Point& v2) {
		return SDL_Point{ v1.x + v2.x, v1.y + v2.y };
	}

	/*!
	 * Creates a vector from two points.
	 *
	 * \return The new vector. 
	 */
	static SDL_Point vectorFromPoints(const SDL_Point& p1, const SDL_Point& p2) {
		return SDL_Point{ p1.x - p2.x, p1.y - p2.y };
	}
	
	/*!
	 * Checks whether a rectangle rect1 is fully contained within a rectangle rect2.
	 */
	static bool rectInRect(const SDL_Rect* rect1, const SDL_Rect* rect2) {
		SDL_Point topLeft{ rect1->x, rect1->y };
		SDL_Point bottomRight{ rect1->x + rect1->w, rect1->y + rect1->h };
		return pointInRect(&topLeft, rect2) && pointInRect(&bottomRight, rect2);
	}

	/*!
	 * Checks if a point is located within a rectangle. 
	 */
	static bool pointInRect(const SDL_Point* point, const SDL_Rect* rect) {
		if (point->x < rect->x) return false;
		if (point->y < rect->y) return false;
		if (point->x > rect->x + rect->w) return false;
		if (point->y > rect->y + rect->h) return false;
		return true;
	}

	/*!
	 * Multiples a vector by a factor.
	 * The resulting values are rounded to the nearest integer.
	 * 
	 * \return The new vector.
	 */
	static SDL_Point multiplyVector(SDL_Point* vector, double mult) {
		return multiplyVector(vector, mult, mult);
	}

	/*!
	 * Multiples a vector X and Y parts by their respective factors.
	 * The resulting values are rounded to the nearest integer.
	 *
	 * \return The new vector.
	 */
	static SDL_Point multiplyVector(SDL_Point* vector, double multX, double multY) {
		return SDL_Point{ static_cast<int>(std::round(vector->x * multX)), static_cast<int>(std::round(vector->y * multY)) };
	}
	
	/*!
	 * Creates a SDL_Rect with the specified dimensions.
	 */
	static SDL_Rect createSDLRect(int x, int y, int width, int height) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;
		return rect;
	}

	/*!
	 * Creates a SDL_Point with the specified values.
	 */
	static SDL_Point createSDLPoint(int x, int y) {
		return SDL_Point{ x, y };
	}
	
	/*!
	 * Euclidean distance between two points.
	 */
	static float distance(const SDL_Point& p1, const SDL_Point& p2) {
		return static_cast<float>(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
	}

	/*!
	 * Euclidean distance between two points.
	 */
	static float distance(int p1x, int p1y, int p2x, int p2y) {
		return static_cast<float>(sqrt(pow(p1x - p2x, 2) + pow(p1y - p2y, 2)));
	}

	static int manhattanDistance(int p1x, int p1y, int p2x, int p2y) {
		return abs(p1x - p2x) + abs(p1y - p2y);
	}

	/*!
	 * Recalculates a number range into a different one preserving the relative value.
	 */
	static double range(double OldValue, double OldMin, double OldMax, double NewMin, double NewMax) {
		return (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;
	}
	
	/*!
	 * Recalculates a number range into a different one preserving the relative value.
	 */
	static float range(float OldValue, float OldMin, float OldMax, float NewMin, float NewMax) {
		return (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;
	}

	/*!
	 * Recalculates a number range into a different one preserving the relative value.
	 */
	static int range(int OldValue, int OldMin, int OldMax, int NewMin, int NewMax) {
		return (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin;
	}

	/*!
	 * Returns a pseudo-random number in the specified inclusive number range.
	 * If min is greater than max, min and max are switched.
	 */
	static int getRandomIntNumberInRange(int min, int max) {
		if (min > max) {
			int tmp = min;
			min = max;
			max = tmp;
			std::cout << "Random number min/max switched!" << std::endl;
		}
		
		return (rand() % (max - min + 1)) + min;
	}
};

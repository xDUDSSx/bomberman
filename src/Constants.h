#pragma once

/*!
 * A utility class declaring static game constants.
 */
class Constants {

#define SECOND 60
	
public:	
	static const int TILE_SIZE = 32;
	static const int BOMBERMAN_SIZE = 30;
	static const int BOMB_SIZE = 32;
	
	static const int PLAYER_MAX_COUNT = 2;
	static const int PLAYER_OR_COMPUTER_MAX_COUNT = 4;
	
	static const int TARGET_FPS = 60;
	static const int SLOWMO_FPS = 6;
	static const int DEFAULT_MOVABLE_SPEED = 2;

	static const int WALL_DESTROY_SCORE = 10;
	static const int POWERUP_PICKUP_SCORE = 50;
	static const int BOMBERMAN_DAMAGE_SCORE = 200;
	static const int BOMBERMAN_SELF_DAMAGE_SCORE = -100;
	
	static const int DEFAULT_EFFECT_DURATION = SECOND;
	static const int DEFAULT_BLINKING_EFFECT_DURATION = 12;
	
	static const int POWERUP_SIZE = 28;
	static const int POWERUP_DROP_CHANCE = 20;
	static const int NEGATIVE_POWERUP_CHANCE = 20;
	static const int POWERUP_LIFETIME = 16 * SECOND;
	static const int POWERUP_BLINK_LIFETIME = 4 * SECOND;

	static const int BOMBERMAN_CONCUSSION_DURATION = 2*SECOND;
	static const int BOMBERMAN_MAX_HEALTH = 3;
	static const int BOMBERMAN_MAX_BOMB_COUNT = 5;
	static const int BOMBERMAN_MAX_SPEED = 3;
	static const int DEFAULT_BOMBERMAN_HEALTH = 3;
	static const int DEFAULT_BOMBERMAN_BOMB_COUNT = 1;
	
	static const int DEFAULT_BOMB_POWER_LEVEL = 3;
	static const int DEFAULT_BOMB_PENETRATION = 1;
	
	static const int BOMB_EXPLOSION_TIME = 3*SECOND;
	static const int BOMB_BREATHING_EFFECT_DURATION = 40;
	static const int BOMB_EXPLOSION_EFFECT_DURATION = 12;

	static const int EXPLOSION_SIZE = TILE_SIZE / 2;
	static const int EXPLOSION_PARTICLE_SIZE = TILE_SIZE;
	static const int EXPLOSION_DURATION = 16;
};
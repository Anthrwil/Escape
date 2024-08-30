#pragma once

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

class sprite
{
public:
	sprite(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer* ren);
	~sprite();
	void render(SDL_Renderer *ren);
	void render(SDL_Renderer *ren, bool isAnimated);
	enum state { idle, movingLeft, movingRight, climbing, climbingIdle, jumping, falling };
	enum spriteType { player, platform, ladder, egg, grain };
	void movement(); 
	state playerState = idle;
	spriteType type; 
	bool isGrounded = false; 
	int playerScore; 
	SDL_RendererFlip flipSprite;

	int animFrameBuffer = 0;
	int bufferMax = 3;
	int playerLives = 3;



	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Rect srcRect;
};


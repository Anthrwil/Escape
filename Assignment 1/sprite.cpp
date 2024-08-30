#include "sprite.h"

sprite::sprite(std::string imgPath, float rectX, float rectY, float rectW, float rectH, SDL_Renderer *ren)
{
	rect.x = rectX;
	rect.y = rectY;
	rect.w = rectW;
	rect.h = rectH;

	surface = IMG_Load(imgPath.c_str());
	if (surface == nullptr) {
		std::cout << "SDL IMG_Load Error: " << IMG_GetError() << std::endl;
		//cleanExit(1);
	}

	texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	if (texture == nullptr) {
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		//cleanExit(1);
	}
	//std::cout << "Created sprite object sucessfully" << std::endl;
}


sprite::~sprite()
{
	SDL_DestroyTexture(texture);
}

void sprite::render(SDL_Renderer *ren)
{
	//std::cout << "Render called" << std::endl;
	SDL_RenderCopy(ren, texture, NULL, &rect);
}

void sprite::render(SDL_Renderer *ren, bool isAnimated)
{
	//std::cout << "Render called" << std::endl;
	SDL_RenderCopyEx(ren, texture, &srcRect, &rect, NULL, NULL, flipSprite);
}

void sprite::movement()
{
	switch (playerState)
	{
	case idle:		
		srcRect.x = 0.0f;
		animFrameBuffer = 0;
		break;
	case movingLeft:
		flipSprite = SDL_FLIP_HORIZONTAL;
		//srcRect.x = 0.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 54.0f)
			{
				srcRect.x = 0.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		break;

	case movingRight:
		flipSprite = SDL_FLIP_NONE;
		//srcRect.x = 0.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 54.0f)
			{
				srcRect.x = 0.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		break;

	case climbing:
		//srcRect.x = 81.0f;
		if (animFrameBuffer >= bufferMax)
		{
			if (srcRect.x == 189.0f)
			{
				srcRect.x = 81.0f;
			}
			else
			{
				srcRect.x += 27.0f;
			}
			animFrameBuffer = 0;
		}
		break;

	case climbingIdle:
		srcRect.x = 81.0f;
		animFrameBuffer = 0;
		break;

	case jumping:

		break;

	case falling:

		break;

	default:
		break;
	}
}

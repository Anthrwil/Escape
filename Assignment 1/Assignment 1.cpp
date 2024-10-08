#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#ifdef _WIN32 // compiling on windows
#include <SDL.h>
#include "SDL_image.h"
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_audio.h>
#include "sprite.h"
#include "text.h"
#include "enemy.h"
#include <Windows.h>
#include "timer.h"
using namespace std;

#else // NOT compiling on windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

std::string exeName;
SDL_Window *win; 
SDL_Renderer *ren; 
SDL_Surface *surface; 
SDL_Texture *tex; 

sprite* player;
text* scoreCount;
text* lifeCount;

const int screenW = 800; 
const int screenH = 600; 

float dt = 0.0f; 
float moveX = 0.0f;			
float moveY = 0.0f;			
float moveSpeed = 25.0f;	
float gravity = 10.0f;		
bool onLadder = false; 
bool isFullscreen = false; 
bool isMusPaused = false;
int volume = 10;
bool done = false; 

std::vector<sprite*> sprites; 
std::vector<enemy*> hens;
sprite* tileSprite; 
text* loading;
text* controls;
enemy* hen1;
enemy* hen2;
enemy* hen3;

Mix_Chunk* jumpEffect;
Mix_Chunk* pickup1;
Mix_Chunk* pickup2;
Mix_Music* background;

int tileMap[27][20] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 3, 2, 0, 4, 0, 0, 0, 3, 4, 0, 0, 0, 0, 3 },
{ 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 0, 3, 0, 0, 2, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 3, 2, 0, 4, 0, 2, 0, 1, 1, 0, 0, 0, 0, 4, 0, 3, 0, 0 },
{ 0, 0, 1, 2, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
{ 0, 3, 0, 2, 0, 4, 0, 2, 0, 0, 0, 2, 0, 3, 4, 0, 2, 0, 3, 0 },
{ 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0 },
{ 0, 0, 4, 0, 3, 0, 0, 2, 0, 0, 0, 2, 0, 4, 0, 0, 2, 0, 0, 0 },
{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

void drawTileMap()
{
	float spriteW = screenW / 20;
	float spriteH = screenH / 27;
	float spriteX = 0.0f;
	float spriteY = 0.0f;
	int count = 0;
	for (int i = 0; i <= 26; i++)
	{
		//std::cout << std::endl;
		for (int j = 0; j <= 19; j++)
		{
			//std::cout << std::to_string(tileMap[i][j]);
			if (tileMap[i][j] == 1){
				// platform
				tileSprite = new sprite("../assets/floor.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::platform;
				sprites.push_back(tileSprite);
			}else if (tileMap[i][j] == 2){
				// ladder
				tileSprite = new sprite("../assets/ladder.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::ladder;
				sprites.push_back(tileSprite);
			}else if (tileMap[i][j] == 3){
				// egg
				tileSprite = new sprite("../assets/gem.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::egg;
				sprites.push_back(tileSprite);
			}else if (tileMap[i][j] == 4){
				// grain
				tileSprite = new sprite("../assets/coin.png", spriteX, spriteY, spriteW, spriteH, ren);
				tileSprite->type = sprite::grain;
				sprites.push_back(tileSprite);
			}

			if (j == 19){
				spriteX = 0.0f;
				spriteY += spriteH;
			}else{
				spriteX += spriteW;
			}
		}
	}
	//std::cout << std::endl;
	//std::cout << std::to_string(sprites.size());
}

float getDeltaTime()
{
	auto t1 = Clock::now();
	auto t2 = Clock::now();
	float nano = (float)((t2 - t1).count());
	float dt = nano / 1000000000;
	//std::cout << "Delta t2-t1: "
	//	<< dt
	//	<< " nanoseconds" << std::endl;
	return dt;
}

bool boundaryCollide(sprite* object)
{
	float playerMinX = player->rect.x;
	float playerMinY = player->rect.y;
	float playerMaxX = playerMinX + player->rect.w;
	float playerMaxY = playerMinY + player->rect.h;

	float objectMinX = object->rect.x;
	float objectMinY = object->rect.y;
	float objectMaxX = objectMinX + object->rect.w;
	float objectMaxY = objectMinY + object->rect.h;
	if ((((playerMinX > objectMinX) && (playerMinX < objectMaxX)) && ((playerMaxY >= objectMinY - 1) && (playerMaxY <= objectMaxY + 1))) ||
		(((playerMaxX > objectMinX) && (playerMaxX < objectMaxX)) && ((playerMaxY >= objectMinY - 1) && (playerMaxY <= objectMaxY + 1))))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void checkCollision(sprite* object, int count)
{
	switch (object->type)
	{
	case sprite::platform: 
		if (boundaryCollide(object))
		{
			player->rect.y = object->rect.y - player->rect.h - 1; 
			player->isGrounded = true;
			gravity = 0.0f;
			//std::cout << "Collide";
		}
		break;

	case sprite::egg:
		if (boundaryCollide(object))
		{

			player->playerScore += 200;
			scoreCount->setScore(ren, player->playerScore);
			//std::cout << "Score: " << player->playerScore << std::endl;
			Mix_PlayChannel(-1, pickup1, 0);
			sprites.erase(sprites.begin() + count);

		}
		break;

	case sprite::ladder:
		if ((player->rect.x > object->rect.x) && ((player->rect.x + player->rect.w) < (object->rect.x + object->rect.w)) &&
			((player->rect.y + player->rect.h) > object->rect.y) && (player->rect.y + player->rect.h) < ((object->rect.y + object->rect.h)))
			//if(boundaryCollide(object))
		{
			onLadder = true;
			player->isGrounded = false;
			//std::cout << "ON LADDER" << std::endl;
		}
		//onLadder = false;
		break;
	case sprite::grain:
		if (boundaryCollide(object))
		{
			player->playerScore += 50; // 50 Points for gem
			scoreCount->setScore(ren, player->playerScore);

			//std::cout << "Score: " << player->playerScore << std::endl;
			Mix_PlayChannel(-1, pickup2, 0);
			sprites.erase(sprites.begin() + count);
		}
	default:
		break;
	}
}


void handleInput()
{
	SDL_Event event; 
					
	while (SDL_PollEvent(&event)) 
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; 						 
			break;

			
		case SDL_KEYDOWN:			
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{					
				case SDLK_ESCAPE: done = true;

				case SDLK_d:					 
					player->playerState = sprite::movingRight;
					moveX = 10.0f; 
					break;

				case SDLK_a:					
					player->playerState = sprite::movingLeft;
					moveX = -10.0f; 
					break;

				case SDLK_SPACE:					
					Mix_PlayChannel(-1, jumpEffect, 0);
					player->playerState = sprite::jumping;
					player->rect.y -= 40.0f;
					break;

				case SDLK_w:
					player->playerState = sprite::climbing;
					gravity = 0.0f;
					moveY = -10.0f;
					break;

				case SDLK_s:
					player->playerState = sprite::climbing;
					moveY = 10.0f;
					break;

				case SDLK_b:
					isFullscreen = !isFullscreen; 
					break;

				case SDLK_m: 
					isMusPaused = !isMusPaused;
					break;
				case SDLK_9: 
					volume -= 5;
					break;
				case SDLK_0: 
					volume += 5;
					break;
				}
			break;

		case SDL_KEYUP:			
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					
				case SDLK_ESCAPE: done = true;

				case SDLK_d:
					player->playerState = sprite::idle;
					moveX = 0.0f;
					break;

				case SDLK_a:
					player->playerState = sprite::idle;
					moveX = 0.0f;
					break;

				case SDLK_w:
					player->playerState = sprite::climbingIdle;
					gravity = 10.0f;
					moveY = 0.0f;
					break;

				case SDLK_s:
					player->playerState = sprite::climbingIdle;
					gravity = 10.0f;
					moveY = 0.0f;
					break;
				}
			break;
		}
	}
}
// end::handleInput[] 

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) 
{
	dt = getDeltaTime();
	player->movement();

	player->animFrameBuffer++;

	player->rect.x += moveX * moveSpeed * simLength;

	int deletedSpritesCount = 0;
	for (int i = 0; i < sprites.size() - deletedSpritesCount; i++)
	{
		checkCollision(sprites[i], i);
	}

	for (auto hen : hens)
	{
		if (boundaryCollide(hen))
		{			
			player->rect.x = 50;
			player->rect.y = 540;
			player->playerLives -= 1;
			lifeCount->setLife(ren, player->playerLives);
		}
		float startPoint = hen->rect.x;
		float endPoint = hen->xTarget1;

		if (hen->rect.x < endPoint)
		{
			hen->rect.x += 25 * simLength;
		}
		else
		{

			hen->rect.x -= 25 * simLength;
		}

	}
	//hen1->animateEnemy();

#pragma region On Ladder Move Check
	if (onLadder)
	{
		gravity = 0.0f;
		player->rect.y += moveY * simLength * moveSpeed; 
	}
	else
	{
		player->rect.y += gravity * simLength * moveSpeed / 2; 
	}
#pragma endregion

#pragma region Platform collision	
	if ((player->rect.y + player->rect.h) >= screenH - 1)
	{
		gravity = 0.0f;
		//std::cout << "Collision Detected" << std::endl;
	}
	else
	{
		gravity = 10.0f;
	}
#pragma endregion

	onLadder = false;

	Mix_VolumeMusic(volume);

	if (isMusPaused)
	{
		Mix_PauseMusic();
	}
	else
	{
		Mix_ResumeMusic();
	}
}

void render()
{	
	SDL_RenderClear(ren);
	if (loading->isShowing)
	{
		loading->render(ren);
	}
	if (controls->isShowing)
	{
		controls->render(ren);
	}
	
	for (auto object : sprites)
	{
		object->render(ren);
	}
	
	scoreCount->render(ren);
	lifeCount->render(ren);

	player->render(ren, true);
	for (auto hen : hens)
	{
		hen->render(ren, true);
	}
	
	SDL_RenderPresent(ren);
	SDL_RenderSetLogicalSize(ren, screenW, screenH);
	if (isFullscreen)
	{
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		SDL_SetWindowFullscreen(win, NULL);
	}
}

void cleanExit(int returnValue)
{
	if (background != nullptr) Mix_FreeMusic(background);
	if (jumpEffect != nullptr) Mix_FreeChunk(jumpEffect);
	if (tex != nullptr) SDL_DestroyTexture(tex);
	if (ren != nullptr) SDL_DestroyRenderer(ren);
	if (win != nullptr) SDL_DestroyWindow(win);
	SDL_Quit();
	IMG_Quit();
	//exit(returnValue);
}

void Load()
{
	SDL_RenderClear(ren);

	SDL_RenderCopy(ren, loading->textTexture, NULL, &loading->textRect);

	SDL_RenderCopy(ren, controls->textTexture, NULL, &controls->textRect);

	SDL_RenderPresent(ren);

	SDL_Delay(500);
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	std::cout << "SDL initialised OK!\n";


	if (!IMG_Init(SDL_INIT_EVERYTHING))
	{
		std::cout << "IMG_Init Error:" << IMG_GetError() << std::endl;
		cleanExit(1);
	}

	std::cout << "IMG initialised OK!\n";
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		std::cout << "SDL_Mixer init error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDP_Mixer Initialised OK!" << std::endl;	
	win = SDL_CreateWindow("Alex McGregor(17638828), CGP2011M Games Programming, 'Escape!'", 100, 100, screenW, screenH, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}
	std::cout << "SDL CreatedWindow OK!\n";

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		cleanExit(1);
	}

	player = new sprite("../assets/spritesheet.png", 50.0f, 520.0f, 24.0f, 32.0f, ren);
	player->srcRect.x = 0.0f;
	player->srcRect.y = 96.0f;
	player->srcRect.w = 24.0f;
	player->srcRect.h = 32.0f;
	player->playerScore = 0;
	player->bufferMax = 5;

#pragma region Hens
	hen1 = new enemy("../assets/spritesheet.png", 500, 100, 24, 40, ren);
	hens.push_back(hen1);
	hen1->state = enemy::moveRight;
	hen1->flipDirection = true;
	hen1->xTarget1 = 200.0f;
	hen1->xTarget2 = 250.0f;
	hen1->srcRect.x = 0.0f;
	hen1->srcRect.y = 0.0f;
	hen1->srcRect.w = 24.0f;
	hen1->srcRect.h = 40.0f;

	hen2 = new enemy("../assets/spritesheet.png", 700, 530, 24, 40, ren);
	hens.push_back(hen2);
	hen2->state = enemy::moveLeft;
	hen2->xTarget1 = 100.0f;
	hen2->xTarget2 = 750.0f;
	hen2->srcRect.x = 0.0f;
	hen2->srcRect.y = 0.0f;
	hen2->srcRect.w = 24.0f;
	hen2->srcRect.h = 40.0f;

	hen3 = new enemy("../assets/spritesheet.png", 600, 280, 24, 40, ren);
	hens.push_back(hen3);
	hen3->state = enemy::moveRight;
	hen3->xTarget1 = 100.0f;
	hen3->xTarget2 = 750.0f;
	hen3->srcRect.x = 0.0f;
	hen3->srcRect.y = 0.0f;
	hen3->srcRect.w = 24.0f;
	hen3->srcRect.h = 40.0f;
#pragma endregion

#pragma region Audio stuff	
	jumpEffect = Mix_LoadWAV("../assets/jump.wav");
	if (jumpEffect == nullptr)
	{
		std::cout << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	pickup1 = Mix_LoadWAV("../assets/pickup1.wav");
	if (pickup1 == nullptr)
	{
		std::cout << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	pickup2 = Mix_LoadWAV("../assets/pickup2.wav");
	if (pickup2 == nullptr)
	{
		std::cout << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}

	background = Mix_LoadMUS("../assets/background_music.mp3");
	if (background == nullptr)
	{
		std::cout << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
		cleanExit(1);
	}
#pragma endregion


	if (TTF_Init() == -1)
	{
		std::cout << "TTF_Init Failed: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}

	TTF_Font* sans = TTF_OpenFont("../assets/font.ttf", 96);
	if (sans == nullptr)
	{
		std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
		cleanExit(1);
	}
	SDL_Color White = { 255, 255, 255 };
	SDL_Color Purple = { 165, 0, 220 };

	std::string lives = "Lives: " + std::to_string(player->playerLives);
	const char* lifeCounts = lives.c_str();

	loading = new text(ren, "Loading: Escape!", 200, 200, 400, 65, sans, White);
	controls = new text(ren, "Controls: Space: Jump W: Up S: Down A: Left D: Right", 100, 300, 600, 30, sans, White);
	Load();
	SDL_Delay(4000);

	scoreCount = new text(ren, player->playerScore, 0, 0, 180, 60, sans, Purple);
	lifeCount = new text(ren, lifeCounts, 600, 0, 180, 60, sans, Purple);

	drawTileMap();

	loading->isShowing = false;
	controls->isShowing = false;
	Mix_PlayMusic(background, -1);

	while (!done) 
	{
		handleInput(); 

		updateSimulation(); 

		render(); 

		SDL_Delay(20); 
	}

	cleanExit(0);
	return 0;
}




#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

bool quit = false;

float deltaTime = 0.0;
int thisTime = 0;
int lastTime = 0;

float playerSpeed = 500.0f;
float yDir;
float pos_X, pos_Y;
SDL_Rect playerPos;

#include "enemy.h"
#include <vector>
#include "bullet.h"

vector<Enemy> enemyList;
vector<Bullet> bulletList;

Mix_Chunk* laser;
Mix_Chunk* explosion;
Mix_Music* bgm;


void CreateBullet()
{
	for (int x = 0; x < bulletList.size(); x++)
	{
		if (bulletList[x].active == false)
		{
			Mix_PlayChannel(-1, laser, 0);

			bulletList[x].active = true;
			bulletList[x].posRect.y = (pos_Y + (playerPos.h / 2));
			bulletList[x].posRect.y = (bulletList[x].posRect.y - (bulletList[x].posRect.h / 2));
			bulletList[x].posRect.x = playerPos.x ;
			bulletList[x].pos_Y = pos_Y;
			bulletList[x].pos_X = playerPos.x + 65;
			break;
		}
	}
}

int playerScore, oldScore, playerLives, oldLives;

TTF_Font* font;
SDL_Color colorP1 = { 0,0,255,255 };
SDL_Surface* scoreSurface, * livesSurface;
SDL_Texture* scoreTexture, * livesTexture;
SDL_Rect scorePos, livesPos;
string tempScore, tempLives;

void UpdateScore(SDL_Renderer* renderer)
{
	tempScore = "Player Score: " + std::to_string(playerScore);
	scoreSurface = TTF_RenderText_Solid(font, tempScore.c_str(), colorP1);
	scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
	SDL_QueryTexture(scoreTexture, NULL, NULL, &scorePos.w, &scorePos.h);
	SDL_FreeSurface(scoreSurface);
	oldScore = playerScore;
}

void UpdateLives(SDL_Renderer* renderer)
{
	tempLives = "Player Lives: " + std::to_string(playerLives);
	livesSurface = TTF_RenderText_Solid(font, tempLives.c_str(), colorP1);
	livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);
	SDL_QueryTexture(livesTexture, NULL, NULL, &livesPos.w, &livesPos.h);
	SDL_FreeSurface(livesSurface);
	oldLives = playerLives;
}

int main(int argc, char* argv[])
{
	SDL_Window* window;

	SDL_Renderer* renderer = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow
	(
		"Space War",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1024,
		768,
		SDL_WINDOW_OPENGL
	);

	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface* surface = IMG_Load("./Assets/background.png");

	SDL_Texture* bkgd;

	bkgd = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	SDL_Rect bkgdPos;

	bkgdPos.x = 0;
	bkgdPos.y = 0;
	bkgdPos.w = 1024;
	bkgdPos.h = 768;

	surface = IMG_Load("./Assets/player.png");
	SDL_Texture* player;
	player = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	playerPos.x = 10;
	playerPos.y = 384; // player position on screen test. doesnt work
	playerPos.w = 70;
	playerPos.h = 55;



	SDL_Event event;

	for (int x = 0; x < 10; x++)
	{
		Bullet tempBullet(renderer, x + 5, x + 5);
		bulletList.push_back(tempBullet);
	}

	enemyList.clear();
	for (int x = 0; x < 8; x++)
	{
		Enemy tempEnemy(renderer);
		enemyList.push_back(tempEnemy);
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	laser = Mix_LoadWAV("./Assets/laser.wav");
	explosion = Mix_LoadWAV("./Assets/explosion.wav");
	bgm = Mix_LoadMUS("./Assets/bkgdloop.flac");

	if (!Mix_PlayingMusic())
	{
		Mix_PlayMusic(bgm, -1);
	}

	oldScore = 0;
	playerScore = 0;
	oldLives = 0;
	playerLives = 5;

	TTF_Init();
	font = TTF_OpenFont("./Assets/font.ttf", 20); //need font for score/lives?

	scorePos.x = 800;//fix score position
	scorePos.y = 20;
	livesPos.x = 800;
	livesPos.y = 40;

	UpdateLives(renderer);
	UpdateScore(renderer);

	enum GameState {GAME, WIN, LOSE };
	GameState gameState = GAME;
	bool game, win, lose;



	while (!quit)
	{
		switch (gameState)
		{
		case GAME:
		{
			game = true;
			std::cout << "The Game State is GAME" << endl;

			surface = IMG_Load("./Assets/background.png");

			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);

			enemyList.clear();

			for (int x = 0; x < 8; x++)
			{
				Enemy tempEnemy(renderer);
				enemyList.push_back(tempEnemy);
			}

			playerScore = 0;
			playerLives = 5;

			while (game)
			{
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				if (SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						quit = true;
						game = false;
						break;
					}

					switch (event.type)
					{
					case SDL_KEYUP:

						switch (event.key.keysym.sym)
						{
						case SDLK_SPACE:
							CreateBullet();
							break;
						default:
							break;
						}
					}
				}

				const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

				if (currentKeyStates[SDL_SCANCODE_UP]) //PLAYER DIRECTION
				{
					yDir = -1;
				}
				else if (currentKeyStates[SDL_SCANCODE_DOWN])
				{
					yDir = 1;
				}
				else
				{
					yDir = 0;
				}

				pos_Y += (playerSpeed * yDir) * deltaTime;

				playerPos.y = (int)(pos_Y + 0.5f);

				if (playerPos.y < 0)
				{
					playerPos.y = 0;
					pos_Y = 0;
				}

				if (playerPos.y > 768 - playerPos.h)
				{
					playerPos.y = 768 - playerPos.h;
					pos_Y = 768 - playerPos.h;
				}

				for (int x = 0; x < bulletList.size(); x++)
				{
					if (bulletList[x].active == true)
					{
						bulletList[x].Update(deltaTime);
					}
				}

				for (int x = 0; x < 8; x++)
				{
					enemyList[x].Update(deltaTime);
				}

				for (int x = 0; x < bulletList.size(); x++)
				{
					if (bulletList[x].active == true)
					{
						for (int y = 0; y < enemyList.size(); y++)
						{
							if (SDL_HasIntersection(&bulletList[x].posRect, &enemyList[y].posRect))
							{
								Mix_PlayChannel(-1, explosion, 0);

								enemyList[y].Reset();
								bulletList[x].Reset();

								playerScore += 10;

								if (playerScore >= 500)
								{
									game = false;
									gameState = WIN;
								}
							}
						}
					}
				}

				for (int x = 0; x < enemyList.size(); x++)
				{
					if (SDL_HasIntersection(&playerPos, &enemyList[x].posRect))
					{
						Mix_PlayChannel(-1, explosion, 0);

						enemyList[x].Reset();

						playerLives -= 1;

						if (playerLives <= 0)
						{
							game = false;
							gameState = LOSE;
						}
					}
				}

				if (playerScore != oldScore)
				{
					UpdateScore(renderer);
				}

				if (playerLives != oldLives)
				{
					UpdateLives(renderer);
				}

				SDL_RenderClear(renderer);

				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);

				SDL_RenderCopy(renderer, player, NULL, &playerPos);

				for (int x = 0; x < bulletList.size(); x++)
				{
					if (bulletList[x].active == true)
					{
						bulletList[x].Draw(renderer);
					}
				}

				for (int x = 0; x < 8; x++)
				{
					enemyList[x].Draw(renderer);
				}

				SDL_RenderCopy(renderer, scoreTexture, NULL, &scorePos);
				SDL_RenderCopy(renderer, livesTexture, NULL, &livesPos);

				SDL_RenderPresent(renderer);


			}
			break;
		}
		
		case WIN:
			win = true;
			std::cout << "The Game State is WIN" << endl;
			std::cout << "Press the R key replay." << endl;
			std::cout << "Press the Q key to quit the game." << endl;
			std::cout << endl;

			surface = IMG_Load("./Assets/win.png");

			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);

			while (win)
			{
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;



				if (SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						quit = true;
						win = false;
						break;
					}

					switch (event.type)
					{
					case SDL_KEYUP:
						switch (event.key.keysym.sym)
						{
						case SDLK_r:
							win = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							win = false;
							break;
						default:
							break;
						}
					}
				}

				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);
				SDL_RenderPresent(renderer);
			}
			break;

		case LOSE:
			lose = true;
			std::cout << "The Game State is LOSE" << endl;
			std::cout << "Press the R key to start over." << endl;
			std::cout << "Press the Q key to quit the game." << endl;
			std::cout << endl;

			surface = IMG_Load("./Assets/lose.png");

			bkgd = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);

			while (lose)
			{
				thisTime = SDL_GetTicks();
				deltaTime = (float)(thisTime - lastTime) / 1000;
				lastTime = thisTime;

				if (SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						quit = true;
						lose = false;
						break;
					}

					switch (event.type)
					{
					case SDL_KEYUP:
						switch (event.key.keysym.sym)
						{
						case SDLK_r:
							lose = false;
							gameState = GAME;
							break;
						case SDLK_q:
							quit = true;
							lose = false;
							break;
						default:
							break;
						}
					}
				}

				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, bkgd, NULL, &bkgdPos);
				SDL_RenderPresent(renderer);
			}
			break;
		}
	}


	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
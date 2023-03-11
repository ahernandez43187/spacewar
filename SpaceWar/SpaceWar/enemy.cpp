#include "enemy.h"

Enemy::Enemy(SDL_Renderer* renderer)
{
	SDL_Surface* surface = IMG_Load("./Assets/enemy.png");

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	int w, h;

	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	posRect.w = w;
	posRect.h = h;

	Reset();

	xDir = -1;//mess with this
	yDir = 0;


}

void Enemy::Reset()
{
	speed = rand() % (5) + 1;
	speed *= 100;

	posRect.x = 1024 + posRect.w;
	posRect.y = rand() % (768 - posRect.h) + 1; //and this

	pos_X = posRect.x;
	pos_Y = posRect.y;
}

void Enemy::Update(float deltaTime)//and this
{
	pos_X += (speed * xDir) * deltaTime;

	posRect.x = (int)(pos_X + 0.5f);

	if (posRect.x < 0 - posRect.w)//flip?
	{
		Reset();
	}
}

void Enemy::Draw(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, texture, NULL, &posRect);
}

Enemy::~Enemy()
{
	//SDL_DestroyTexture(texture);
}
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>

#ifdef __MINGW32__
#undef main
#endif

int const CELL_SIZE = 16;
int const GRID_SIZE = 16;

int const CASE_X[16] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};
int const CASE_Y[16] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};
int const STARTING_SNAKE_SIZE = 2;
double const STARTING_SNAKE_SPEED = 0.15;

int scores(0);

int loop;

int appleX;
int appleY;

int snakeDirection;
int snakeSize;
double snakeSpeed;

int snakeX;
int snakeY;
int snakeW;
int snakeH;

std::vector<int> tailX;
std::vector<int> tailY;

int r(255);
int g(255);
int b(255);

int randomNum;
bool winLoop(1);

SDL_Window* window = SDL_CreateWindow("YeetSnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (GRID_SIZE * CELL_SIZE), (GRID_SIZE * CELL_SIZE), SDL_WINDOW_SHOWN); // Case 16x16
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

clock_t previousClock = 0;

Mix_Chunk *popSound = NULL;

int random(int max) // Retourne un nombre aléatoire entre 0 et max
{
	randomNum = rand() % max + 0;
	return randomNum;
}

int randomRange(int min, int max) // Retourne un nombre aléatoire entre min et max
{
	randomNum = rand() % max + min;
	return randomNum;
}

void drawRectangle(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_Rect rectangle;
	rectangle.x = x;
	rectangle.y = y;
	rectangle.w = w;
	rectangle.h = h;

	SDL_RenderFillRect(renderer, &rectangle);
	//SDL_RenderPresent(renderer);
}

void drawSquare(SDL_Renderer* renderer, int x, int y, int w, Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_Rect square;
	square.x = x;
	square.y = y;
	square.w = w;
	square.h = w;

	SDL_RenderFillRect(renderer, &square);
	//SDL_RenderPresent(renderer);
}

void showApple()
{
	SDL_RenderPresent(renderer);
}

void placeApple()
{
	int x = random(16);
	int y = random(16);

	appleX = x;
	appleY = y;

	drawSquare(renderer, CASE_X[appleX], CASE_Y[appleY], 16, 255, 0, 0);
}

void showSnake()
{
	SDL_RenderPresent(renderer);
}

void placeSnake()
{
	drawRectangle(renderer, CASE_X[snakeX], CASE_Y[snakeY], snakeW, snakeH, r, g, b);
	for (int i = 1; i < snakeSize; i++)
	{
		drawRectangle(renderer, CASE_X[tailX[i]], CASE_Y[tailY[i]], snakeW, snakeH, r, g, b);
	}
}

void clearSnake()
{
	r = 0; g = 0; b = 0;
	drawRectangle(renderer, CASE_X[snakeX], CASE_Y[snakeY], snakeW, snakeH, r, g, b);
	drawRectangle(renderer, CASE_X[tailX.back()], CASE_Y[tailY.back()], snakeW, snakeH, r, g, b);
	r = 255; g = 255; b = 255;
}

void initVect(char xy)
{
	if (xy == 'x')
	{
		tailX[0] = snakeX;
		for (int i = snakeSize; i > 0; i--)
		{
			tailX[i] = tailX[i - 1];
			tailY[i] = snakeY;
		}
	}
	else if (xy == 'y')
	{
		tailY[0] = snakeY;
		for (int i = snakeSize; i > 0; i--)
		{
			tailY[i] = tailY[i - 1];
			tailX[i] = snakeX;
		}
	}
	else
	{

	}
}

void snakeMove()
{
	switch(snakeDirection)
	{
	case 1: // W
		clearSnake();
		initVect('y');
		tailY[0] = snakeY;
		snakeY--;
		placeSnake();
		break;
	case 2: // A
		clearSnake();
		initVect('x');
		tailX[0] = snakeX;
		snakeX--;
		placeSnake();
		break;
	case 3: // S
		clearSnake();
		initVect('y');
		tailY[0] = snakeY;
		snakeY++;
		placeSnake();
		break;
	case 4: // D
		clearSnake();
		initVect('x');
		tailX[0] = snakeX;
		snakeX++;
		placeSnake();
		break;
	default:
		break;
	}
	// 1 = W : UP
	// 2 = A : LEFT
	// 3 = S : DOWN
	// 4 = D : RIGHT
}

void initGame()
{
	snakeSize = STARTING_SNAKE_SIZE;
	snakeDirection = 0;
	snakeSpeed = STARTING_SNAKE_SPEED;

	snakeX = 8;
	snakeY = 8;
	snakeW = 16;
	snakeH = 16;

	tailX.push_back(snakeX);
	tailY.push_back(snakeY);
	tailX.push_back(snakeX);
	tailY.push_back(9);

	drawRectangle(renderer, CASE_X[snakeX], CASE_Y[snakeY], snakeW, snakeH, r, g, b);
	drawRectangle(renderer, CASE_X[tailX[1]], CASE_Y[tailY[1]], snakeW, snakeH, r, g, b);
}

int main()
{
	SDL_Event e;

	initGame();

	placeApple();

	while (winLoop)
	{
		while(SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_LALT && e.key.keysym.sym == SDLK_F4)
				{
					winLoop = 0;
				}
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP)
				{
					snakeDirection = 1;
				}
				if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT)
				{
					snakeDirection = 2;
				}
				if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)
				{
					snakeDirection = 3;
				}
				if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT)
				{
					snakeDirection = 4;
				}
				else
				{

				}
				SDL_RenderPresent(renderer);
			}
			else if (e.type == SDL_QUIT)
			{
				winLoop = 0;
			}
		}

		clock_t currentClock = clock();
		clock_t elapsedClock = currentClock - previousClock;
		float elapsedSeconds = float(elapsedClock) / float(CLOCKS_PER_SEC);
		if(elapsedSeconds >= snakeSpeed)
		{
			snakeMove();
			previousClock = currentClock;
		}

		std::cout << snakeX << " : " << snakeY << " : scores = " << scores << " : size = " << snakeSize << " : " << tailX[1] << " : " << tailY[1] << std::endl;

		if (snakeX == appleX && snakeY == appleY)
		{
			scores++;
			snakeSpeed -= 0.001;
			snakeSize++;

			placeApple();
			placeSnake();
		}
		showApple();
		showSnake();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

// TODO :
// 1 : grandeur du serpent
// 2 : le corp qui suit le serpent
// 3 : serpent grandit quand il mange une pomme
// 4 : collision
// 5 : musique


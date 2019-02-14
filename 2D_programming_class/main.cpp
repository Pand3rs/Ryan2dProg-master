#include <iostream>
#include <assert.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

SDL_Renderer *renderer = NULL;

int screen_width = 800;
int screen_height = 600;

const int max_balls = 5;
int ball_size = 20;

const int num_bricks = 48;
int brick_w = 40;
int brick_h = 20;

float player_x = 360.0;
float player_y = 520.0;
int player_w = 80;
int player_h = 20;

int border_w = 10;

SDL_Window *window = NULL;

struct Pixel
{
	unsigned char r, g, b, a;
};

struct ball
{
	float x, y, fx, fy;
	unsigned char r, g, b, a;
};

struct brick
{
	int x, y, alive;
	int r, g, b, a;
};

void set_pixel(unsigned char* buffer, int width, int x, int y, int r, int g, int b, int a)
{
	Pixel *p = (Pixel*)buffer;
	p[y*width + x].r = r;
	p[y*width + x].g = g;
	p[y*width + x].b = b;
	p[y*width + x].a = a;
}

void fill_rectangle(unsigned char*buffer, int buffer_width, int buffer_height, int rect_x, int rect_y, int rect_w, int rect_h, int r, int g, int b, int a)
{
	for (int i = 0; i < rect_w; i++)
	{
		for (int j = 0; j < rect_h; j++)
		{
			int x = i + rect_x;
			int y = j + rect_y;
			set_pixel(buffer, screen_width, x, y, r, g, b, a);
		}
	}
}

void spawn_ball(ball *a)
{
	a->x = player_x + player_w / 2 - ball_size / 2;
	a->y = player_y - ball_size;
	a->fx = 1 - 2.0 * rand() / RAND_MAX;
	a->fy = 1 - 2.0 * rand() / RAND_MAX;
	a->r = rand() % 255;
	a->g = rand() % 255;
	a->b = rand() % 255;
	a->a = 255;
}

void populate_bricks(unsigned char*buffer, brick *bricks)
{
	for (int i = 0; i < num_bricks; i++)
	{
		if (i <= 15)
		{
			bricks[i].x = (screen_width - brick_w * 16) / 2 + i * brick_w;
			bricks[i].y = 80;
		}
		else if (i > 15 && i <= 31)
		{
			bricks[i].x = (screen_width - brick_w * 16) / 2 + (i - 16) * brick_w;
			bricks[i].y = 80 + brick_h;
		}
		else if (i > 31)
		{
			bricks[i].x = (screen_width - brick_w * 16) / 2 + (i - 32) * brick_w;
			bricks[i].y = 80 + brick_h * 2;
		}

		bricks[i].alive = 1;
		bricks[i].r = rand() % 255;
		bricks[i].g = rand() % 255;
		bricks[i].b = rand() % 255;
		bricks[i].a = 255;

		fill_rectangle(buffer, screen_width, screen_height, bricks[i].x, bricks[i].y,
			brick_w, brick_h, bricks[i].r, bricks[i].g, bricks[i].b, bricks[i].a);
	}
}

int collision_with_dir(ball *a, brick *b)
{
	float w = 0.5 * (ball_size + brick_w);
	float h = 0.5 * (ball_size + brick_h);
	float dx = a->x - b->x + 0.5*(ball_size - brick_w);
	float dy = a->y - b->y + 0.5*(ball_size - brick_h);

	if (dx*dx <= w * w && dy*dy <= h * h)
	{
		float wy = w * dy;
		float hx = h * dx;
		if (wy > hx)
		{
			return (wy + hx > 0) ? 3 : 4;
		}
		else
		{
			return (wy + hx > 0) ? 2 : 1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	srand(0);

	window = SDL_CreateWindow("I did eet", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	SDL_Surface *your_draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_SetSurfaceBlendMode(your_draw_buffer, SDL_BLENDMODE_NONE);
	unsigned char *my_own_buffer = (unsigned char*)malloc(sizeof(unsigned char)*screen_width*screen_height * 4);
	ball balls_array[max_balls] = {};
	brick bricks[num_bricks] = {};

	int active_balls = 1;
	spawn_ball(&balls_array[0]);
	populate_bricks(my_own_buffer, bricks);

	for (;;)
	{
		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}	
		}

		//borders
		{
			fill_rectangle(my_own_buffer, screen_width, screen_height, 0, 0,
				border_w, screen_height, 100, 200, 100, 255);
			fill_rectangle(my_own_buffer, screen_width, screen_height, 0, 0,
				screen_width, border_w, 100, 200, 100, 255);
			fill_rectangle(my_own_buffer, screen_width, screen_height, 790, 0,
				border_w, screen_height, 100, 200, 100, 255);
			fill_rectangle(my_own_buffer, screen_width, screen_height, 0, 590,
				screen_width, border_w, 100, 200, 100, 255);
		}

		//player
		{
			const Uint8 *state = SDL_GetKeyboardState(NULL);

			if (state[SDL_SCANCODE_A] && player_x > border_w)
			{
				player_x -= 1;
			}
			if (state[SDL_SCANCODE_D] && player_x < screen_width - player_w - border_w)
			{
				player_x += 1;
			}

			fill_rectangle(my_own_buffer, screen_width, screen_height, player_x, player_y,
				player_w, player_h, 255, 50, 50, 255);
		}

		//update bricks
		for (int i = 0; i < num_bricks; i++)
		{
			if (bricks[i].alive == 1)
			{
				fill_rectangle(my_own_buffer, screen_width, screen_height, bricks[i].x, bricks[i].y,
					brick_w, brick_h, bricks[i].r, bricks[i].g, bricks[i].b, bricks[i].a);
			}
		}

		//update balls
		for (int i = 0; i < active_balls; i++)
		{
			//bounds and player collision
			if (balls_array[i].x <= border_w || balls_array[i].x >= screen_width - ball_size - border_w)
			{
				balls_array[i].fx *= -1;
			}
			if (balls_array[i].y <= border_w || balls_array[i].y >= screen_height - ball_size - border_w)
			{
				balls_array[i].fy *= -1;
			}

			if (balls_array[i].y + ball_size >= player_y && balls_array[i].x + ball_size >= player_x && balls_array[i].x <= player_x + player_w)
			{
				balls_array[i].fy *= -1;
			}

			//brick collision
			for (int i = 0; i < active_balls; i++)
			{
				for (int j = 0; j < num_bricks; j++)
				{
					if (bricks[j].alive == 1)
					{
						int x = collision_with_dir(&balls_array[i], &bricks[j]);
						if (x == 1 || x == 3)
						{
							balls_array[i].fy *= -1;

							bricks[j].alive = 0;
							if (active_balls < max_balls)
							{
								spawn_ball(&balls_array[active_balls]);
								active_balls += 1;
							}
							break;
						}

						else if (x == 2 || x == 4)
						{
							balls_array[i].fx *= -1;

							bricks[j].alive = 0;
							if (active_balls < max_balls)
							{
								spawn_ball(&balls_array[active_balls]);
								active_balls += 1;
							}
							break;
						}
					}
				}
			}

			//add force
			balls_array[i].x += balls_array[i].fx;
			balls_array[i].y += balls_array[i].fy;

			//draw ball to buffer
			fill_rectangle(my_own_buffer, screen_width, screen_height, balls_array[i].x, balls_array[i].y,
					ball_size, ball_size, balls_array[i].r, balls_array[i].g, balls_array[i].b, balls_array[i].a);
		}

		memcpy(your_draw_buffer->pixels, my_own_buffer, sizeof(unsigned char)*screen_width*screen_height * 4);

		//BLIT BUFFER TO SCREEN
		SDL_BlitScaled(your_draw_buffer, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);

		//clear screen
		for (int i = 0; i < screen_width*screen_height; i++)
		{
			my_own_buffer[i * 4] = 0;
			my_own_buffer[i * 4 + 1] = 0;
			my_own_buffer[i * 4 + 2] = 0;
			my_own_buffer[i * 4 + 3] = 0;
		}
	}
	return 0;
}
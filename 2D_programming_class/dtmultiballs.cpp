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
int num_balls = 1000;

unsigned char prev_key_state[256];
unsigned char *keys = NULL;
SDL_Window *window = NULL;

//Alternative example
struct Pixel
{
	unsigned char r, g, b, a;
};

struct ball
{
	float x, y, fx, fy;
	unsigned char r, g, b, a;
};

void set_Pixel_Alternative(unsigned char* buffer, int width, int x, int y, int r, int g, int b, int a)
{
	Pixel *p = (Pixel*)buffer;
	p[y*width + x].r = r;
	p[y*width + x].g = g;
	p[y*width + x].b = b;
	p[y*width + x].a = a;
}

void fill_Rectangle(unsigned char*buffer, int buffer_width, int buffer_height, int rect_x, int rect_y, int rect_w, int rect_h, int r, int g, int b, int a)
{
	for (int i = 0; i < rect_w; i++)
	{
		for (int j = 0; j < rect_h; j++)
		{
			int x = i + rect_x;
			int y = j + rect_y;
			set_Pixel_Alternative(buffer, screen_width, x, y, r, g, b, a);
		}
	}
}

void drawLine(unsigned char* buffer, float x1, float y1, float x2, float y2, int r, int g, int b, int a)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float k, l;
	if (x1 < x2)
	{
		k = x1;
		l = x2;
	}
	else if (x1 == x2)
	{
		if (y1 < y2)
		{
			k = y1;
			l = y2;
		}
		else
		{
			k = y2;
			l = y1;
		}
		for (int i = k; i < l; i++)
		{
			int x = x1;
			int y = i;
			set_Pixel_Alternative(buffer, screen_width, x, y, r, g, b, a);
		}
	}
	else
	{
	k = x2;
	l = x1;
	}
	for (int i = k; i < l; i++)
	{
		int x = i;
		int y = y1 + dy * (i - x1) / dx;
		set_Pixel_Alternative(buffer, screen_width, x, y, r, g, b, a);
	}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	prev_key_state[256];
	keys = (unsigned char*)SDL_GetKeyboardState(NULL);

	window = SDL_CreateWindow(
		"I did eet",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	SDL_Surface *your_draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_SetSurfaceBlendMode(your_draw_buffer, SDL_BLENDMODE_NONE);
	unsigned char *my_own_buffer = (unsigned char*)malloc(sizeof(unsigned char)*screen_width*screen_height * 4);
	ball *balls_array = (ball*)malloc(sizeof(ball)*num_balls);

	int bsize = 10;
	int counter = 0;
	int t1 = SDL_GetTicks();
	for (int i = 0; i < num_balls; i++)
	{
		balls_array[i].x = rand() % (screen_width - bsize);
		balls_array[i].y = rand() % (screen_height - bsize);
		balls_array[i].fx = 1 - 2.0 * rand() / RAND_MAX;
		balls_array[i].fy = 1 - 2.0 * rand() / RAND_MAX;
		balls_array[i].r = rand() % 255;
		balls_array[i].g = rand() % 255;
		balls_array[i].b = rand() % 255;
		balls_array[i].a = 255;
	}

	for (;;)
	{
		
		memcpy(prev_key_state, keys, 256);

		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		//ball
		for (int i = 0; i < num_balls; i++)
		{
			if (balls_array[i].x <= 0 || balls_array[i].x >= screen_width - bsize)
			{
				balls_array[i].fx *= -1;
			}
			if (balls_array[i].y <= 0 || balls_array[i].y >= screen_height - bsize)
			{
				balls_array[i].fy *= -1;
			}
			balls_array[i].x += balls_array[i].fx;
			balls_array[i].y += balls_array[i].fy;
			fill_Rectangle(my_own_buffer, screen_width, screen_height, balls_array[i].x, balls_array[i].y,
				bsize, bsize, balls_array[i].r, balls_array[i].g, balls_array[i].b, balls_array[i].a);
		}

		memcpy(your_draw_buffer->pixels, my_own_buffer, sizeof(unsigned char)*screen_width*screen_height * 4);

		//BLIT BUFFER TO SCREEN
		SDL_BlitScaled(your_draw_buffer, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);

		for (int i = 0; i < screen_width*screen_height; i++)
		{
			my_own_buffer[i * 4] = 0;
			my_own_buffer[i * 4 + 1] = 0;
			my_own_buffer[i * 4 + 2] = 0;
			my_own_buffer[i * 4 + 3] = 0;
		}

		if (counter++ >= 100)
		{
			int t2 = SDL_GetTicks();
			float time = (t2 - t1) / 100.0;
			printf("%f\n", time);
			counter = 0;
			t1 = SDL_GetTicks();
		}

	}
	return 0;
}
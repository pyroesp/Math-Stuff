#ifndef _DRAW_H

#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

typedef struct{
	float x;
	float y;
}Point;

// Basic SDL surface drawing functions, google is your friend
void draw_Pixel(SDL_Surface *s, Point p, uint32_t pixel);
void draw_Line(SDL_Surface *s, Point start, Point end, uint32_t pixel);
void draw_Dot(SDL_Surface *s, int size, Point p, uint32_t pixel);

#endif

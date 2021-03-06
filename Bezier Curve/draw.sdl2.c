#include <math.h>
#include <SDL2/SDL.h>

#include "draw.h"


void draw_Pixel(void *surface, Point p, uint32_t pixel){
	SDL_Surface *s = (SDL_Surface*)surface;
	uint32_t *tp = (uint32_t*)(((uint8_t *)s->pixels) + (int)p.y * s->pitch + (int)p.x * sizeof(*tp));
	*tp = pixel;
}

// Ugly but it works
void draw_Line(void *surface, Point start, Point end, uint32_t pixel){
	SDL_Surface *s = (SDL_Surface*)surface;
	SDL_Rect r;
	Point p;
	float m, b, offset;
	float width, height;

	if (end.x == start.x){ //vertical line
		r.w = 1;
		r.h = (int)(end.y - start.y);
		r.x = (int)start.x;
		r.y = (int)start.y;
		SDL_FillRect(s, &r, pixel);
	}else if (end.y == start.y){ //horizontal line
		r.w = (int)(end.x - start.x);
		r.h = 1;
		r.x = (int)start.x;
		r.y = (int)start.y;
		SDL_FillRect(s, &r, pixel);
	}else{
		m = (end.y - start.y)/(end.x - start.x);
		b = start.y - start.x * m;

		width = fabs(end.x - start.x);
		height = fabs(end.y - start.y);

		if (width >= height){
			if (end.x > start.x)
				offset = 0.1;
			else
				offset = -0.1;
			// Get Y position from X position and draw pixel, not efficient but works
			for (p.x = start.x; (offset == 0.1f && p.x < end.x) || (offset == -0.1f && p.x > end.x); p.x += offset){
				p.y = p.x * m + b;
				if ((p.y >= 0 && p.y <= s->h) && (p.x >= 0 && p.x <= s->w))
					draw_Pixel(s, p, pixel);
			}
		}else if (width < height){
			if (end.y > start.y)
				offset = 0.1;
			else
				offset = -0.1;
			// Get X position from Y position and draw pixel, not efficient but works
			for (p.y = start.y; (offset == 0.1f && p.y < end.y) || (offset == -0.1f && p.y > end.y); p.y += offset){
				p.x = (p.y - b) / m;
				if ((p.y >= 0 && p.y <= s->h) && (p.x >= 0 && p.x <= s->w))
					draw_Pixel(s, p, pixel);
			}
		}
	}
}

// Dot is just a filled rectangle of equal width and height
void draw_Dot(void *surface, int size, Point p, uint32_t pixel){
	SDL_Surface *s = (SDL_Surface*)surface;
	if ((size % 2) == 0)
		size++;

	SDL_Rect r;
	r.h = size;
	r.w = size;
	r.x = p.x - size / 2;
	r.y = p.y - size / 2;

	SDL_FillRect(s, &r, pixel);
}

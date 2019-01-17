#ifndef _DRAW_H
#define _DRAW_H

#include <stdint.h>

#include "bezier.h"

// Basic drawing functions, google is your friend
void draw_Pixel(void *surface, Point p, uint32_t pixel);
void draw_Line(void *surface, Point start, Point end, uint32_t pixel);
void draw_Dot(void *surface, int size, Point p, uint32_t pixel);

#endif

#ifndef _BEZIER_H
#define _BEZIER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "draw.h"

#define DOT_SIZE (5)

enum Direction {NONE, DOWN, UP, RIGHT, LEFT};

typedef struct Bezier{
	Point p;
	float m,b; // y = mx+b
	enum Direction dir;
	uint8_t locked;
	struct Bezier *parent[2];
}Bezier;

Bezier *bezier_Create(uint8_t locked, Point p, Bezier *mom, Bezier *dad);
void Bezier_Initialize(Bezier **b, Point *p, uint8_t bezier_level);
void bezier_SetPoint(Bezier *b, Point p);
void bezier_GetLinearEquation(Bezier *b);
void bezier_GetDirection(Bezier *b);
void bezier_UpdatePoint(Bezier *b, float toNormalize);
void bezier_CheckPointToEnd(Bezier *b);
void bezier_DrawDot(SDL_Surface *s, Bezier *b, uint32_t color);
void bezier_DrawLine(SDL_Surface *s, Bezier *b1, Bezier *b2, uint32_t color);
void bezier_Free(Bezier *b);

#endif

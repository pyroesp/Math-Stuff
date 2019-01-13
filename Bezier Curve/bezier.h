#ifndef _BEZIER_H
#define _BEZIER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "draw.h"

#define DOT_SIZE (5)

// Direction to where the point needs to go
enum Direction {NONE, DOWN, UP, RIGHT, LEFT};

// Bezier structure
typedef struct Bezier{
	Point p; // point
	float m,b; // y = mx+b
	enum Direction dir; // direction
	uint8_t locked; // locked == 1 ? stop moving : keep updating
	struct Bezier *parent[2]; // parent Bezier structure
}Bezier;

// Create Bezier point
Bezier *bezier_Create(uint8_t locked, Point p, Bezier *mom, Bezier *dad);
// Initialize Bezier structure, use this in main
void bezier_Initialize(Bezier **b, Point *p, uint8_t bezier_level);
// Copy point to Bezier structure
void bezier_SetPoint(Bezier *b, Point p);
// Calc m & b from y = mx + b
void bezier_GetLinearEquation(Bezier *b);
// Set in which direction the point will move
void bezier_GetDirection(Bezier *b);
// Update points
void bezier_UpdatePoint(Bezier *b, float toNormalize);
// Check if point reached the end and lock
void bezier_CheckPointToEnd(Bezier *b);
// Draw all Bezier points
void bezier_DrawDot(SDL_Surface *s, Bezier *b, uint32_t color);
// Connect Bezier points
void bezier_DrawLine(SDL_Surface *s, Bezier *b1, Bezier *b2, uint32_t color);
// Free Bezier structure
void bezier_Free(Bezier *b);

#endif

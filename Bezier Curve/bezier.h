#ifndef _BEZIER_H
#define _BEZIER_H

#include <stdint.h>

#define DOT_SIZE (5)

// Point structure
typedef struct{
	float x;
	float y;
}Point;

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
// The first points on screen define the "level" of the bezier, see triangle number
uint32_t bezier_TriangleNumber(uint32_t bezier_level);
// Copy point to Bezier structure
void bezier_SetPoint(Point *dest, Point src);
// Calc m & b from y = mx + b
void bezier_GetLinearEquation(Bezier *b);
// Set in which direction the point will move
void bezier_GetDirection(Bezier *b);
// Update points
void bezier_UpdatePoint(Bezier *b, float toNormalize);
// Check if point reached the end and lock
void bezier_CheckPointToEnd(Bezier *b);
// Draw all Bezier points
void bezier_DrawDot(void *surface, Bezier *b, uint32_t color);
// Connect Bezier points
void bezier_DrawLine(void *surface, Bezier *b1, Bezier *b2, uint32_t color);
// Free Bezier structure
void bezier_Free(Bezier *b);

#endif

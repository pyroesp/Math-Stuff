#include "bezier.h"

Bezier *bezier_Create(uint8_t locked, Point p, Bezier *mom, Bezier *dad){
	Bezier *b;
	b = (Bezier*)malloc(sizeof(Bezier));
	if(!b)
		return NULL;
	b->m = 0;
	b->b = 0;
	b->locked = locked;
	b->dir = NONE;
	bezier_SetPoint(b, p);
	b->parent[0] = mom;
	b->parent[1] = dad;
	bezier_GetLinearEquation(b);
	bezier_GetDirection(b);
	return b;
}

void bezier_SetPoint(Bezier *b, Point p){
	if (!b)
		return;
	b->p.x = p.x;
	b->p.y = p.y;
}

void bezier_GetLinearEquation(Bezier *b){
	if (b->parent[0] == NULL || b->parent[1] == NULL)
		return;
	if (b->locked)
		return;
	// y = mx + b
	b->m = (b->parent[1]->p.y - b->parent[0]->p.y) / (b->parent[1]->p.x - b->parent[0]->p.x);
	b->b = b->parent[1]->p.y - (b->parent[1]->p.x * b->m);
}

void bezier_GetDirection(Bezier *b){
	float width, height;
	if (b->parent[0] == NULL || b->parent[1] == NULL)
		return;
	if (b->locked)
		return;

	width = fabs(b->parent[1]->p.x - b->parent[0]->p.x);
	height = fabs(b->parent[1]->p.y - b->parent[0]->p.y);

	if (width >= height){
		if (b->parent[1]->p.x >= b->parent[0]->p.x)
			b->dir = RIGHT;
		else
			b->dir = LEFT;
	}else{
		if (b->parent[1]->p.y >= b->parent[0]->p.y)
			b->dir = DOWN;
		else
			b->dir = UP;
	}
}

void bezier_UpdatePoint(Bezier *b, float toNormalize){
	if (b->parent[0] == NULL || b->parent[1] == NULL)
		return;
	if (b->locked)
		return;

	//      ___________________
	// D = V(x2-x1)² + (y2-y1)²
	float total_dist, dist;
	total_dist = sqrt(pow((b->parent[1]->p.x - b->parent[0]->p.x),2) + 
					pow((b->parent[1]->p.y - b->parent[0]->p.y), 2));
	dist = total_dist/toNormalize;

	switch (b->dir){
		// x = (y - b)/m
		case DOWN:
			b->p.y += dist;
			b->p.x = (b->p.y - b->b) / b->m;
			break;
		case UP:
			b->p.y -= dist;
			b->p.x = (b->p.y - b->b) / b->m;
			break;
		// y = mx + b
		case RIGHT:
			b->p.x += dist;
			b->p.y = b->p.x * b->m + b->b;
			break;
		case LEFT:
			b->p.x -= dist;
			b->p.y = b->p.x * b->m + b->b;
			break;
		case NONE:
		default:
			break;
	}
}

void bezier_CheckPointToEnd(Bezier *b){
	float dist;
	if (!b || !b->parent[1])
		return;
	if (b->locked)
		return;
	//      ___________________
	// D = V(x2-x1)² + (y2-y1)²
	dist = sqrt(pow((b->parent[1]->p.x - b->p.x),2)  + pow((b->parent[1]->p.y - b->p.y), 2));
	if (dist < 0.5)
		b->locked = 1;
}

void bezier_DrawDot(SDL_Surface *s, Bezier *b, uint32_t color){
	if (!s || !b)
		return;
	draw_Dot(s, DOT_SIZE, b->p, color);
}

void bezier_DrawLine(SDL_Surface *s, Bezier *b1, Bezier *b2, uint32_t color){
	if (!s || !b1 || !b2)
		return;
	draw_Line(s, b1->p, b2->p, color);
}

void bezier_Free(Bezier *b){
	if (!b)
		return;
	free(b);
	b = NULL;
}

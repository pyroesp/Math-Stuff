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

void bezier_Initialize(Bezier **b, Point *p, uint8_t bezier_level){
	int i, j;
	int parent_offset = 0;
	int bezier_offset = 0;
	for (i = 0; i < bezier_level; i++){
		if (i > 1)
			parent_offset += bezier_level - (i - 2);
		for (j = 0; j < bezier_level - i; j++){
			b[j + bezier_offset] = bezier_Create(i == 0, p[j], i == 0 ? NULL : b[j + parent_offset], i == 0 ? NULL : b[j + parent_offset + 1]);
		}
		bezier_offset += bezier_level - i;
	}
	/* Example of what happens in this function :
	bezier_level == 4

	@ i = 0 ; bezier_offset = 0; parent_offset = 0;
	A. b[0] = Bezier *bezier_Create(0, p[0], NULL, NULL); // j = 0
	B. b[1] = Bezier *bezier_Create(0, p[1], NULL, NULL); // j = 1
	C. b[2] = Bezier *bezier_Create(0, p[2], NULL, NULL); // j = 2
	D. b[3] = Bezier *bezier_Create(0, p[3], NULL, NULL); // j = 3

	@ i = 1 ; bezier_offset = 4; parent_offset = 0;
	E. b[0 + 4] = Bezier *bezier_Create(1, p[0], b[0 + 0], b[0 + 0 + 1]); // j = 0 | (A, B)
	F. b[1 + 4] = Bezier *bezier_Create(1, p[1], b[1 + 0], b[1 + 0 + 1]); // j = 1 | (B, C)
	G. b[2 + 4] = Bezier *bezier_Create(1, p[2], b[2 + 0], b[2 + 0 + 1]); // j = 2 | (C, D)

	@ i = 2 ; bezier_offset = 7; parent_offset = 4;
	H. b[0 + 7] = Bezier *bezier_Create(1, p[0], b[0 + 4], b[0 + 4 + 1]); // j = 0 | (E, F)
	I. b[1 + 7] = Bezier *bezier_Create(1, p[1], b[1 + 4], b[1 + 4 + 1]); // j = 1 | (F, G)

	@ i = 3 ; bezier_offset = 9; parent_offset = 7;
	J. b[0 + 9] = Bezier *bezier_Create(1, p[0], b[0 + 7], b[0 + 7 + 1]); // j = 0 | (H, I)
	*/
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

	// check width and height
	width = fabs(b->parent[1]->p.x - b->parent[0]->p.x);
	height = fabs(b->parent[1]->p.y - b->parent[0]->p.y);

	// whatever is bigger determines the direction the point has to travel
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

	// All points need to get to the destination at the same time
	// that's why you normalize each step to last Bezier point (the one that follows the curve)

	//      ___________________
	// D = V(x2-x1)² + (y2-y1)²
	float total_dist, dist;
	total_dist = sqrt(pow((b->parent[1]->p.x - b->p.x),2) +
					pow((b->parent[1]->p.y - b->p.y), 2));
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
	if (dist <= 0.99f)
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

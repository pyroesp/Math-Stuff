#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

/// Bezier Curve
// https://giphy.com/gifs/curves-U9TTNMuIXwepa

typedef struct{
	float x;
	float y;
}Point;

typedef struct Bezier_Curve{
	struct Bezier_Curve *next;
	Point *p;
	uint32_t size;
}Bezier_Curve;

Bezier_Curve* create_BezierCurvePoints(Point *p, uint32_t size);
void print_BezierCurve(Bezier_Curve *bc);
void update_BezierCurve(Bezier_Curve *bc);
void draw_BezierCurvePoints(SDL_Surface *s, uint32_t pixel, uint32_t dot_size, Bezier_Curve *bc);
void draw_BezierCurveLines(SDL_Surface *s, uint32_t pixel, Bezier_Curve *bc);

void draw_pixel(SDL_Surface *s, Point p, uint32_t pixel);
void draw_line(SDL_Surface *s, Point start, Point end, uint32_t pixel);
void draw_dot(SDL_Surface *s, int size, Point p, uint32_t pixel);


int main(int argc, char *argv[]){
	// Do SDL stuff
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	SDL_Window *w = SDL_CreateWindow("DameGame", 256, 256, 210, 210, SDL_WINDOW_SHOWN);
	if (!w)
		return -1;
	SDL_Surface *ws = SDL_GetWindowSurface(w);
	SDL_FillRect(ws, &ws->clip_rect, 0xFF000000);
	SDL_UpdateWindowSurface(w);

	#define point_size 4
	Point point[point_size] = {
		{
			.x = 10,
			.y = 30
		},
		{
			.x = 20,
			.y = 130
		},
		{
			.x = 150,
			.y = 150
		},
		{
			.x = 180,
			.y = 15
		}
	};

	Bezier_Curve *bc;
	bc = create_BezierCurvePoints(point, point_size);
	print_BezierCurve(bc);
	printf("--------------------------------------\n");

	SDL_Event e;
	int exit = 0;
	while (!exit){
		SDL_PollEvent(&e);
		switch(e.type){
			case SDL_KEYUP:
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE:
						exit = 1;
						break;
				}
			case SDL_QUIT:
				exit = 1;
				break;
		}

		SDL_FillRect(ws, NULL, 0xFF000000);

		//printf("draw points\n");
		draw_BezierCurvePoints(ws, 0xFFFFFFFF, 3, bc);
		//SDL_UpdateWindowSurface(w);
		//printf("draw lines\n");
		draw_BezierCurveLines(ws, 0xFFFF0000, bc);
		//SDL_UpdateWindowSurface(w);
		//printf("update Bezier\n");
		update_BezierCurve(bc);

		SDL_UpdateWindowSurface(w);

		SDL_Delay(250);
	}

	SDL_FreeSurface(ws);
	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}


Bezier_Curve* create_BezierCurvePoints(Point *p, uint32_t size){
	Bezier_Curve *bc = NULL;
	if (size > 0){
		bc = (Bezier_Curve*)malloc(sizeof(Bezier_Curve));
		bc->next = NULL;
		bc->p = NULL;
		bc->next = create_BezierCurvePoints(p, size-1);
		bc->p = (Point*)malloc(sizeof(Point) * size);
		memcpy(bc->p, p, sizeof(Point) * size);
		bc->size = size;
	}else{
		return NULL;
	}

	return bc;
}

void print_BezierCurve(Bezier_Curve *bc){
	Bezier_Curve *t = bc;
	int i;
	if (t){
		printf("next = 0x%X\n", (unsigned int)t->next);
		printf("size = %d\n", t->size);
		for (i = 0; i < t->size; i++){
			printf("p[%d] = (%0.2f, %0.2f)\n", i, t->p[i].x, t->p[i].y);
		}
		print_BezierCurve(bc->next);
	}
}

void update_BezierCurve(Bezier_Curve *bc){
	int i;
	float max_len, temp, len, angle;
	Bezier_Curve *current, *next;
	Point *p1, *p2, v;
	len = 0;
	/// Check the length between the points on each level
	/// of the Bezier Curve
	/// Normalize each point movement to the longest length
	current = bc;
	next = bc->next;
	while (next->size > 1){
		for (i = 0; i < next->size - 1; i++){
			p1 = &next->p[i+1];
			p2 = &next->p[i];
			temp = sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));
			if (temp > len)
				max_len = temp;
		}
		current = next;
		next = next->next;
	}

	current = bc;
	next = bc->next;
	while (next->size > 1){
		for (i = 0; i < next->size; i++){
			/// get 2 dots in current
			p1 = &current->p[i+1];
			p2 = &current->p[i];
			/// calc length
			len = sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));
			/// calc angle
			// m = tan(theta) = (y2 - y1)/(x2 - x1)
			angle = atan((p1->y - p2->y)/(p1->x - p2->x));
			/// calc vector
			v.x = len/max_len * cos(angle);
			v.y = len/max_len * sin(angle);

			next->p[i].x += v.x;
			next->p[i].y += v.y;
		}

		current = next;
		next = next->next;
	}
}

void draw_BezierCurvePoints(SDL_Surface *s, uint32_t pixel, uint32_t dot_size, Bezier_Curve *bc){
	int i;
	Bezier_Curve *n;
	n = bc;
	while(1){
		for (i = 0; i < n->size; i++){
			draw_dot(s, dot_size, n->p[i], pixel);
		}
		if (n->size == 1)
			break;
		n = n->next;
	}
}

void draw_BezierCurveLines(SDL_Surface *s, uint32_t pixel, Bezier_Curve *bc){
	int i;
	Bezier_Curve *n;
	n = bc;
	while(n->size > 1){
		for (i = 0; i < n->size - 1; i++){
			draw_line(s, n->p[i+1], n->p[i], pixel);
		}
		n = n->next;
	}
}

void draw_pixel(SDL_Surface *s, Point p, uint32_t pixel){
	uint32_t *tp = (uint32_t*)(((uint8_t *)s->pixels) + (int)p.y * s->pitch + (int)p.x * sizeof(*tp));
	*tp = pixel;
}

void draw_line(SDL_Surface *s, Point start, Point end, uint32_t pixel){
	SDL_Rect r;
	Point p;
	float m, b, offset;

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
		// y = mx + b
		m = (end.y - start.y)/(end.x - start.x); // (30 - 130) / ( 10 - 20 ) = -100/-10 = 10
		b = start.y - start.x * m;

		if (start.x > end.x)
			offset = -0.1;
		else if (start.x < end.x)
			offset = 0.1;
		for (p.x = start.x; ceil(p.x) != floor(end.x); p.x += offset){
			p.y = p.x * m + b;
			draw_pixel(s, p, pixel);
		}
	}
}

void draw_dot(SDL_Surface *s, int size, Point p, uint32_t pixel){
	if (size%2 == 0)
		size++;

	SDL_Rect r;
	r.h = size;
	r.w = size;
	r.x = p.x - size/2;
	r.y = p.y - size/2;

	SDL_FillRect(s, &r, pixel);
}

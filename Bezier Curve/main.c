#include <stdio.h>

#include "bezier.h"

/// Bezier Curve
// https://giphy.com/gifs/curves-U9TTNMuIXwepa

void print_Bezier(Bezier *b, uint8_t index){
	printf("Bezier %d: 0x%X\n", index, b);
	enum Direction dir;
	uint8_t locked;
	struct Bezier *parent[2];
	printf("\tPoint [%f, %f]\n", b->p.x, b->p.y);
	printf("\tm = %f ; b = %f\n", b->m, b->b);
	printf("\tDir = %d\n", b->dir);
	printf("\tlocked = %d\n", b->locked);
	printf("\tparent = [0x%X, 0x%X]\n", b->parent[0], b->parent[1]);
}

int main(int argc, char *argv[]){
	int i;
	// Do SDL stuff
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	SDL_Window *w = SDL_CreateWindow("DameGame", 256, 256, 200, 200, SDL_WINDOW_SHOWN);
	if (!w)
		return -1;
	SDL_Surface *ws = SDL_GetWindowSurface(w);
	SDL_FillRect(ws, &ws->clip_rect, 0xFF000000);
	SDL_UpdateWindowSurface(w);

	Point p[3] = {
		{.x = 10, .y = 50},
		{.x = 100, .y = 150},
		{.x = 190, .y = 50}
	};

	uint8_t bezier_level = 3;
	uint8_t bezier_size = 6; // == 3!
	Bezier *b[bezier_size];
	b[0] = bezier_Create(1, p[0], NULL, NULL);
	b[1] = bezier_Create(1, p[1], NULL, NULL);
	b[2] = bezier_Create(1, p[2], NULL, NULL);

	b[3] = bezier_Create(0, b[0]->p, b[0], b[1]);
	b[4] = bezier_Create(0, b[1]->p, b[1], b[2]);

	b[5] = bezier_Create(0, b[3]->p, b[3], b[4]);

	float dist, temp;
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
				break;
			case SDL_QUIT:
				exit = 1;
				break;
			default:
				break;
		}

		// Clear screen
		SDL_FillRect(ws, NULL, 0xFF000000);

		// Draw Bezier lines
		for (i = 0; i < 2; i++){
			bezier_DrawLine(ws, b[i], b[i + 1], 0xFFFF0000);
		}
		// Draw Bezier dot
		for (i = 0; i < bezier_size; i++){
			print_Bezier(b[i], i);
			bezier_DrawDot(ws, b[i], 0xFFFFFFFF);
		}

		// Get longest distance to use to normalize the next step
		dist = 0;
		temp = 0;
		for (i = 0; i < bezier_size; i++){
			if (!b[i]->locked && b[i]->parent[0] && b[i]->parent[1]){
				temp = sqrt(pow((b[i]->parent[1]->p.x - b[i]->parent[0]->p.x), 2) +
							pow((b[i]->parent[1]->p.y - b[i]->parent[0]->p.y), 2));
				if (dist < temp)
					dist = temp;
			}
		}

		// Update Bezier
		for (i = 0; i < bezier_size; i++){
			bezier_UpdatePoint(b[i], dist);
			bezier_CheckPointToEnd(b[i]);
		}
		// Get linear equation and direction after update
		for (i = 0; i < bezier_size; i++){
			bezier_GetLinearEquation(b[i]);
			bezier_GetDirection(b[i]);
		}

		// Update window
		SDL_UpdateWindowSurface(w);

		SDL_Delay(100);
	}

	for (i = 0; i < bezier_size; i++)
		bezier_Free(b[i]);

	SDL_FreeSurface(ws);
	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}

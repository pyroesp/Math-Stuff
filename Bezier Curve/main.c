#include <stdio.h>

#include "bezier.h"

#define SCREEN_WIDTH (200)
#define SCREEN_HEIGHT (200)
#define BG_COLOR (0xFF000000)

/// Bezier Curve
// https://media.giphy.com/media/9Pi3Ad2EyzF3JKVcSa/giphy.gif

void print_Bezier(Bezier *b, uint8_t index){
	char dir[5][8] = {"NONE", "DOWN", "UP", "RIGHT", "LEFT"};
	printf("Bezier %d: 0x%X\n", index, (uint32_t)b);
	printf("\tPoint [%f, %f]\n", b->p.x, b->p.y);
	printf("\tm = %f ; b = %f\n", b->m, b->b);
	printf("\tDir = %s\n", dir[b->dir]);
	printf("\tlocked = %d\n", b->locked);
	printf("\tparent = [0x%X, 0x%X]\n", (uint32_t)b->parent[0], (uint32_t)b->parent[1]);
	if (b->parent[0] && b->parent[1])
		printf("\tParent points [%f, %f] - [%f, %f]\n", b->parent[0]->p.x, b->parent[0]->p.y, b->parent[1]->p.x, b->parent[1]->p.y);
}

int triangle_number(int x){
	int retval = 1;
	for (; x > 1; x--)
		retval += x;
	return retval;
}

int main(int argc, char *argv[]){
	// Do SDL stuff
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	SDL_Window *w = SDL_CreateWindow("Bezier Curve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!w)
		return -1;
	SDL_Surface *ws = SDL_GetWindowSurface(w);
	SDL_FillRect(ws, &ws->clip_rect, BG_COLOR);
	SDL_UpdateWindowSurface(w);

	// Initialize Bezier stuff
#define BEZ_SIZE (4)
	uint8_t bezier_level = BEZ_SIZE;
	Point p[BEZ_SIZE] = {
		{.x = 10, .y = 100},
		{.x = 70, .y = 50},
		{.x = 130, .y = 150},
		{.x = 190, .y = 100}
	};
	int curve_size = 1;
	Point *curve;
	curve = (Point*)malloc(sizeof(Point));
	uint8_t bezier_size = triangle_number(bezier_level);
	Bezier *b[bezier_size];
	bezier_Initialize(b, p, bezier_level);
	memcpy(curve, &b[bezier_level - 1]->p, sizeof(Point));

	int i, j, offset;
	int exit = 0;
	float dist, temp;
	SDL_Event e;

	// Wait for window to open
	SDL_Delay(1000);

	// Get events
	while (!exit){
		SDL_PollEvent(&e);
		switch(e.type){
			case SDL_KEYUP:
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE: // ESC key
						exit = 1;
						break;
				}
				break;
			case SDL_QUIT: // Close window
				exit = 1;
				break;
			default:
				break;
		}

		// Clear screen
		SDL_FillRect(ws, NULL, BG_COLOR);

		// Draw Bezier lines
		if (!b[bezier_size-1]->locked){
			offset = 0;
			for (i = 0; i < bezier_level - 1; i++){
				for (j = 0; j < bezier_level - 1 - i; j++){
					bezier_DrawLine(ws, b[j + offset], b[j + offset + 1], 0xFFFFFFFF);
				}
				offset += bezier_level - i;
			}
		}
		// Draw Bezier dot
		for (i = 0; i < bezier_size; i++){
			bezier_DrawDot(ws, b[i], 0xFF00AAFF);
		}

		// Get distance from end point to last bezier point to use to normalize the next step
		dist = sqrt(pow((b[bezier_size - 1]->parent[1]->p.x - b[bezier_size - 1]->p.x), 2) +
					pow((b[bezier_size - 1]->parent[1]->p.y - b[bezier_size - 1]->p.y), 2));

		// Bezier curve update stuff and co
		for (i = 0; i < bezier_size; i++){
			bezier_UpdatePoint(b[i], dist); // update point position
			bezier_CheckPointToEnd(b[i]); // lock if point has reached end point
			bezier_GetDirection(b[i]); // get direction to move
			bezier_GetLinearEquation(b[i]); // get linear equation variables m and b from y = mx+b
		}

		// Add points to curve array
		if (!b[bezier_size - 1]->locked){
			curve_size++;
			curve = (Point*)realloc(curve, sizeof(Point) * curve_size);
			memcpy(&curve[curve_size-1], &b[bezier_size - 1]->p, sizeof(Point));
		}

		// Draw points of curve
		for (i = 0; i < curve_size; i++)
			draw_Pixel(ws, curve[i], 0xFFFF0000);

		// Update window
		SDL_UpdateWindowSurface(w);

		// Delay so it's not over before the window opens
		SDL_Delay(100);
	}

	// free stuff before exit
	for (i = 0; i < bezier_size; i++)
		bezier_Free(b[i]);
	free(curve);

	SDL_FreeSurface(ws);
	SDL_DestroyWindow(w);
	// quit
	SDL_Quit();
	return 0;
}

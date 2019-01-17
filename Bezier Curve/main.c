#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "bezier.h"
#include "draw.h"

#define SCREEN_WIDTH (400)
#define SCREEN_HEIGHT (400)
#define BG_COLOR (0xFF000000)

#define CURSOR_MINIMUM_SIZE (8) // minimum cursor size
#define CURSOR_HYST_HIGH (2) // use : current_pos >= max_size / hyst_high => trigger to increase array size
#define CURSOR_HYST_LOW (8)  // use : current_pos <= max_size / hyst_low  => trigger to decrease array size

#define CURVE_MINIMUM_SIZE (128)
#define CURVE_HYST_HIGH (2)
#define CURVE_HYST_LOW (8)

#define BEZIER_MINIMUM_SIZE (16)
#define BEZIER_HYST_HIGH (2)
#define BEZIER_HYST_LOW (8)

/*
Bezier, Cursor & Curve arrays
	Arrays are dynamic, malloc'ed to a minimum value.
	Size is increased every time the array is half full.
	Size is decreased every time the array is less than 1/8 full.
*/

/// Bezier Curve

// Used for debug struct
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

int main(int argc, char *argv[]){
	// Do SDL stuff
	SDL_Event e;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;
	SDL_Window *w = SDL_CreateWindow("Bezier Curve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!w)
		return -1;
	SDL_Surface *ws = SDL_GetWindowSurface(w);
	SDL_FillRect(ws, &ws->clip_rect, BG_COLOR);
	SDL_UpdateWindowSurface(w);

	// Setup cursor array
	int hide_cursor_dots = 0;
	int cursor_max_size = CURSOR_MINIMUM_SIZE;
	int cursor_current_pos = 0;
	Point *cursor = NULL;
	Point *temp_cursor = NULL;
	cursor = (Point*)calloc(cursor_max_size, sizeof(Point));

	// Setup curve array
	int curve_max_size = CURVE_MINIMUM_SIZE;
	int curve_current_pos = 0;
	Point *curve;
	curve = (Point*)calloc(curve_max_size, sizeof(Point));

	// Setup Bezier array
	int bezier_update_points = 0;
	int bezier_max_size = BEZIER_MINIMUM_SIZE;
	int bezier_level = 0;
	int bezier_size = 0;
	Bezier **b = NULL;
	Bezier **temp_b = NULL;
	b = (Bezier**)malloc(sizeof(Bezier*) * bezier_max_size);

	int i, j, offset;
	int new_event;
	int exit = 0;
	float dist;

	// Wait for window to open
	SDL_Delay(1000);

	// Get events
	while (!exit){
		// Get the return value of PollEvent to check if there's a new event
		new_event = SDL_PollEvent(&e);
		if (new_event){
			switch(e.type){
				case SDL_MOUSEBUTTONUP:
					bezier_update_points = 1;
					hide_cursor_dots = 0;
					// Button released
					if (e.button.button == SDL_BUTTON_LEFT){ // button left released
						// Add cursor position to cursor Point array
						if (cursor_current_pos >= (cursor_max_size / CURSOR_HYST_HIGH)){
							// Increase size of array
							cursor_max_size *= CURSOR_HYST_HIGH;
							cursor = (Point*)realloc(cursor, sizeof(Point) * (cursor_max_size + 1));
						}
						cursor[cursor_current_pos].x = e.button.x;
						cursor[cursor_current_pos].y = e.button.y;
						cursor_current_pos++;
					}else if (e.button.button == SDL_BUTTON_RIGHT){ // button right released
						// Remove position from cursor Point
						if (cursor_current_pos <= (cursor_max_size / CURSOR_HYST_LOW) && cursor_max_size > CURSOR_MINIMUM_SIZE){
							// Decrease size of array
							cursor_max_size /= CURSOR_HYST_LOW;
							if (cursor_max_size < CURSOR_MINIMUM_SIZE)
								cursor_max_size = CURSOR_MINIMUM_SIZE;
							temp_cursor = (Point*)malloc(sizeof(Point) * cursor_max_size);
							memcpy(temp_cursor, cursor, sizeof(Point) * cursor_max_size);
							free(cursor);
							cursor = temp_cursor;
							temp_cursor = NULL;
						}

						if (cursor_current_pos > 0)
							cursor_current_pos--;
					}
					break;
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
		}

		// Clear screen
		SDL_FillRect(ws, NULL, BG_COLOR);

		// Draw cursor dots
		for (i = 0; i < cursor_current_pos && !hide_cursor_dots; i++)
			draw_Dot((void*)ws, DOT_SIZE, cursor[i], 0xFF00AAFF);

		// Only execute Bezier if there are more than 2 points
		if (cursor_current_pos > 2){
			// Check if point update available
			if (bezier_update_points){
				bezier_level = cursor_current_pos;
				bezier_size = bezier_TriangleNumber(bezier_level);
				// malloc / realloc Bezier structure
				if (bezier_size >= bezier_max_size / BEZIER_HYST_HIGH){
					bezier_max_size *= 2;
					b = (Bezier**)realloc(b, sizeof(Bezier*) * bezier_max_size);
				}else if (bezier_size <= bezier_max_size / BEZIER_HYST_LOW && bezier_max_size > BEZIER_MINIMUM_SIZE){
					bezier_max_size /= BEZIER_HYST_HIGH;
					if (bezier_max_size < BEZIER_MINIMUM_SIZE)
						bezier_max_size = BEZIER_MINIMUM_SIZE;
					temp_b = (Bezier**)malloc(sizeof(Bezier*) * bezier_max_size);
					memcpy(temp_b, b, sizeof(Bezier*) * bezier_max_size);
					free(b);
					b = temp_b;
					temp_b = NULL;
				}
				// Initialize Bezier structure with cursor positions
				bezier_Initialize(b, cursor, bezier_level);
				// Clear curve points
				curve_current_pos = 0;
				bezier_update_points = 0;
			}


			// Get distance from end point to last bezier point to normalize the next step
			dist = sqrt(pow((b[bezier_size - 1]->parent[1]->p.x - b[bezier_size - 1]->p.x), 2) +
						pow((b[bezier_size - 1]->parent[1]->p.y - b[bezier_size - 1]->p.y), 2));

			// Bezier curve update stuff and co
			for (i = 0; i < bezier_size; i++){
				bezier_UpdatePoint(b[i], dist); // update point position
				bezier_CheckPointToEnd(b[i]); // lock if point has reached end point
				bezier_GetDirection(b[i]); // get direction to move
				bezier_GetLinearEquation(b[i]); // get linear equation variables m and b from y = mx+b
			}


			if (!b[bezier_size - 1]->locked){
				// Draw Bezier lines
				offset = 0;
				for (i = 0; i < bezier_level - 1; i++){
					for (j = 0; j < bezier_level - 1 - i; j++){
						bezier_DrawLine((void*)ws, b[j + offset], b[j + offset + 1], 0xFFFFFFFF);
					}
					offset += bezier_level - i;
				}

				// Draw Bezier dots
				for (i = 0; i < bezier_size; i++){
					bezier_DrawDot((void*)ws, b[i], 0xFF00AAFF);
				}

				// Add points from last Bezier point to curve array
				if (curve_current_pos >= (curve_max_size / CURVE_HYST_HIGH)){
					curve_max_size *= CURVE_HYST_HIGH;
					curve = (Point*)realloc(curve, sizeof(Point) * curve_max_size);
				}
				bezier_SetPoint(&curve[curve_current_pos], b[bezier_size - 1]->p);
				curve_current_pos++;
			}else{
				hide_cursor_dots = 1;
			}

			// Draw points of curve
			for (i = 0; i < curve_current_pos; i++)
				draw_Pixel((void*)ws, curve[i], 0xFFFF0000);
		}

		// Update window
		SDL_UpdateWindowSurface(w);

		// Small delay so you can see the updates
		SDL_Delay(25);
	}

	// free stuff before exit
	for (i = 0; i < bezier_size; i++)
		bezier_Free(b[i]);
	free(curve);
	free(cursor);

	SDL_FreeSurface(ws);
	SDL_DestroyWindow(w);
	// quit
	SDL_Quit();
	return 0;
}


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "renderer.h"
#include "shader.h"
#include "tetris.h"
#include "window.h"

#define FRAMERATE 144

int
main() {
	srand(time(NULL));
	struct Tetris game = {.score = 0, .level = 0};
	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			game.playfield[x][y] = rand() % BLOCKCOLOR_MAX;
		}
	}
	struct Window win   = window_Init(800, 600, "win");
	struct Renderer ren = renderer_Init(win.win_x, win.win_y);

	double start_time = glfwGetTime();
	while(window_Active(win)) {
		glfwPollEvents();
		double acc_time = glfwGetTime() - start_time;
		if(acc_time > 1.0f / FRAMERATE) {
			renderer_DrawQuadBoundaries(ren, 0, 0, win.win_x, win.win_y, color_Uniform(140, 140, 140, 255, 255));

			int x_padding  = 100;
			int x_offset   = 2;
			int y_padding  = 40;
			int y_offset   = 2;
			int block_size = 20;

			int border_padding = 10;
			renderer_DrawQuad(
			    ren,
			    x_padding - border_padding,
			    y_padding - border_padding,
			    border_padding + block_size * PLAYFIELD_X + x_offset * PLAYFIELD_X + (border_padding - x_offset),
			    border_padding + block_size * PLAYFIELD_Y_MIN + y_offset * PLAYFIELD_Y_MIN +
			        (border_padding - y_offset),
			    color_Uniform(0, 0, 0, 255, 255));

			for(int x = 0; x < PLAYFIELD_X; ++x) {
				for(int y = PLAYFIELD_Y_MIN; y < PLAYFIELD_Y; ++y) {
					int x_start_pos = x_padding + (block_size * x) + (x * x_offset);
					int y_start_pos = y_padding + (block_size * (y - 20)) + ((y - 20) * y_offset);
					Color_t to_draw;
					switch(game.playfield[x][y]) {
					case BLOCKCOLOR_EMPTY:
						to_draw = color_Uniform(82, 82, 82, 255, 255);
						break;
					case BLOCKCOLOR_ORANGE:
						to_draw = color_Uniform(255, 122, 33, 255, 255);
						break;
					case BLOCKCOLOR_BLUE:
						to_draw = color_Uniform(0, 0, 255, 255, 255);
						break;
					default:
						break;
					}
					renderer_DrawQuad(ren, x_start_pos, y_start_pos, block_size, block_size, to_draw);
				}
			}
			window_Update(win);
			start_time = glfwGetTime();
		}
	}

	glfwTerminate();
	return 0;
}
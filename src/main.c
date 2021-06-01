
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "renderer.h"
#include "shader.h"
#include "tetris.h"
#include "window.h"
#include "painter.h"

#define FRAME_LIMITING 1
#define FRAMERATE 144

int
main() {
	srand((unsigned int)time(NULL));
	struct Tetris game = tetris_Init();
	struct Window win   = window_Init(800, 600, "tetris");
	struct Renderer ren = renderer_Init((float)win.win_x, (float)win.win_y);

	double start_time = glfwGetTime();
    double game_start_time = glfwGetTime();
	while(window_Active(win)) {
		glfwPollEvents();
        double game_update_acc_time = glfwGetTime() - game_start_time;
        if(game_update_acc_time > 1.0f) {
            game_start_time = glfwGetTime();
            for(int x = 0; x < PLAYFIELD_X; ++x) {
                for(int y = 0; y < PLAYFIELD_Y; ++y) {
                    game.playfield[x][y] = rand() % BLOCKCOLOR_MAX;
                }
            }
        }

		double acc_time = glfwGetTime() - start_time;
#if FRAME_LIMITING
		if(acc_time > 1.0f / FRAMERATE) {
            start_time = glfwGetTime();
#endif
			renderer_DrawQuadBoundaries(ren, 0, 0, (float)win.win_x, (float)win.win_y, color_Uniform(140, 140, 140, 255, 255));
            paint_TetrisPlayfield(ren, game);
			window_Update(win);
            
#if FRAME_LIMITING
		}
#endif
	}

	glfwTerminate();
	return 0;
}
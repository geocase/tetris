
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "window.h"
#include "renderer.h"
#include "shader.h"
#include "tetris.h"


#define FRAMERATE 144

int
main() {
	struct Tetris game = {.score = 0, .level = 0};
	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			game.playfield[x][y] = BLOCKCOLOR_EMPTY;
		}
	}
    struct Window win = window_Init(800, 600, "win");
	struct Renderer ren = renderer_Init(win.win_x, win.win_y);
    double start_time = glfwGetTime();
	while(window_Active(win)) {
		glfwPollEvents();
        double acc_time = glfwGetTime() - start_time;
        if(acc_time > 1.0f / FRAMERATE) {
            renderer_DrawQuad(
                ren,
                100,
                100,
                540,
                380,
                (Color_t){
                    (sinf(glfwGetTime()) + 1.0f) * (255.0f / 2.0f) / 255.0f,
                    (sinf(glfwGetTime() * 2) + 1.0f) * (255.0f / 2.0f) / 255.0f,
                    (sinf(glfwGetTime() / 2) + 1.0f) * (255.0f / 2.0f) / 255.0f,
                    1.0f});
        
            window_Update(win);
            start_time = glfwGetTime();
        }
	}

	glfwTerminate();
	return 0;
}
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "painter.h"
#include "renderer.h"
#include "shader.h"
#include "tetris.h"
#include "window.h"

#include <GLFW/glfw3.h>


#define FRAME_LIMITING 1
#define FRAMERATE 144
#define GAME_UPDATE_RATE 60

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS)
		printf("%s\n", glfwGetKeyName(key, scancode));
}


int
main() {
	srand((unsigned int)time(NULL));
	struct Tetris game  = tetris_Init(glfwGetTime());
	struct Window win   = window_Init(800, 600, "tetris");
	struct Renderer ren = renderer_Init((float)win.win_x, (float)win.win_y);

	double start_time      = glfwGetTime();
	double game_start_time = glfwGetTime();

	glfwSetKeyCallback(win.window, key_callback);

	while(window_Active(win)) {
		glfwPollEvents();
		double game_update_acc_time = glfwGetTime() - game_start_time;
		if(game_update_acc_time > 1.0f / GAME_UPDATE_RATE) {
			game_start_time = glfwGetTime();
			tetris_Update(&game, 1.0f / GAME_UPDATE_RATE);
		}

		double acc_time = glfwGetTime() - start_time;
#if FRAME_LIMITING
		if(acc_time > 1.0f / FRAMERATE) {
			start_time = glfwGetTime();
#endif

			renderer_DrawQuadBoundaries(
			    ren, 0, 0, (float)win.win_x, (float)win.win_y, color_Uniform(140, 140, 140, 255, 255));
			
			renderer_DrawLine(ren, 1.0f, 0, 0, ((sinf(glfwGetTime()) + 1.0f) / 2.0f) * win.win_x, ((sinf(glfwGetTime() * 2) + 1.0f) / 2.0f) * win.win_y, color_Uniform(255, 0, 0, 1, 1));


			paint_TetrisPlayfield(ren, game);
			window_Update(win);

#if FRAME_LIMITING
		}
#endif
	}

	glfwTerminate();
	return 0;
}
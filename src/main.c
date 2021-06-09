#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "painter.h"
#include "renderer.h"
#include "shader.h"
#include "tetris.h"
#include "window.h"

#include "audio.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <GLFW/glfw3.h>
#include <sndfile.h>

#define ASSET_DIR "assets/"

#define FRAME_LIMITING 1
#define FRAMERATE 144
#define GAME_UPDATE_RATE 60
struct Tetris game;

void
key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	game.rotate_key.just_pressed    = (action == GLFW_PRESS && key == GLFW_KEY_SPACE);
	game.left_key.just_pressed      = (action == GLFW_PRESS && key == GLFW_KEY_LEFT);
	game.right_key.just_pressed     = (action == GLFW_PRESS && key == GLFW_KEY_RIGHT);
	game.hard_drop_key.just_pressed = (action == GLFW_PRESS && key == GLFW_KEY_UP);
	game.reset_key.just_pressed     = (action == GLFW_PRESS && key == GLFW_KEY_R);
	game.pause_key.just_pressed     = (action == GLFW_PRESS && key == GLFW_KEY_P);
	if(action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
		game.fast_drop_key.is_pressed = true;
	}
	if(action == GLFW_RELEASE && key == GLFW_KEY_DOWN) {
		game.fast_drop_key.is_pressed = false;
	}
}

#ifdef __linux__
int
main() {
#elif _WIN32
#include <windows.h>
int
wmain() {
#ifdef NDEBUG
	FreeConsole();
#endif
#endif

	

	srand((unsigned int)time(NULL));
	game                = tetris_Init((float)glfwGetTime());
	struct Window win   = window_Init(800, 600, "tetris");
	struct Renderer ren = renderer_Init((float)win.win_x, (float)win.win_y);

	struct AudioPlayer ap = audioplayer_Init();
	audioplayer_LoadSample(&ap, ASSET_DIR "beep_0.wav", 0);
	audioplayer_LoadSample(&ap, ASSET_DIR "beep_1.wav", 1);
	audioplayer_LoadSample(&ap, ASSET_DIR "music_0.wav", 2);

	double start_time      = glfwGetTime();
	double game_start_time = glfwGetTime();

	glfwSetKeyCallback(win.window, key_callback);

	unsigned int bg = audioplayer_PlaySoundWithVolume(ap, 2, .001f, false, true);

	while(window_Active(win)) {
		glfwPollEvents();
		double game_update_acc_time = glfwGetTime() - game_start_time;
		if(game_update_acc_time > 1.0f / GAME_UPDATE_RATE) {
			game_start_time = glfwGetTime();
			tetris_Update(&game, ap, 1.0f / GAME_UPDATE_RATE);
		}

		double acc_time = glfwGetTime() - start_time;
#if FRAME_LIMITING
		if(acc_time > 1.0f / FRAMERATE) {
			start_time = glfwGetTime();
#endif

			renderer_DrawQuadBoundaries(
			    ren, 0, 0, (float)win.win_x, (float)win.win_y, color_Normal(140, 140, 140, 255, 255));

			paint_TetrisPlayfield(ren, game);
			window_Update(win);

#if FRAME_LIMITING
		}
#endif
	}

	glfwTerminate();
	return 0;
}
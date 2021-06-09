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

#define FRAME_LIMITING 1
#define FRAMERATE 144
#define GAME_UPDATE_RATE 60

static void
list_audio_devices(const ALCchar* devices) {
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;
	printf("devices:\n");
	while(device && *device != '\0' && next && *next != '\0') {
		printf("%s\n", devices);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

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
	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCdevice* device = alcOpenDevice(NULL);
	if(!device) {
		exit(-1);
	}
	ALCcontext* context = alcCreateContext(device, NULL);
	if(!alcMakeContextCurrent(context)) {
		exit(-1);
	}

	ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	struct AudioPlayer ap = audioplayer_Init();
	audioplayer_LoadSample(&ap, "air_raid.wav", 1);
	audioplayer_PlaySound(ap, 0, false, true);
	unsigned int bg = audioplayer_PlaySound(ap, 1, false, true);

	Sleep(2000);
	audioplayer_StopSound(ap, bg);

	srand((unsigned int)time(NULL));
	game                = tetris_Init((float)glfwGetTime());
	struct Window win   = window_Init(800, 600, "tetris");
	struct Renderer ren = renderer_Init((float)win.win_x, (float)win.win_y);

	double start_time      = glfwGetTime();
	double game_start_time = glfwGetTime();

	glfwSetKeyCallback(win.window, key_callback);

	while(window_Active(win)) {
		// alSource3f(source2, AL_POSITION, sinf(glfwGetTime()) * 20, 0, 0);

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
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "renderer.h"
#include "shader.h"
#include "tetris.h"

#include <math.h>

int
main() {
	struct Tetris game = {.score = 0, .level = 0};
	for(int x = 0; x < PLAYFIELD_X; ++x) {
		for(int y = 0; y < PLAYFIELD_Y; ++y) {
			game.playfield[x][y] = BLOCKCOLOR_EMPTY;
		}
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "win", NULL, NULL);
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	glViewport(0, 0, 640, 480);

	struct Renderer ren = renderer_Init(640.0f, 480.0f);

	float k = 0;
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
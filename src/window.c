#include "window.h"
#include <stdlib.h>
struct Window
window_Init(int window_size_x, int window_size_y, const char* window_title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

	struct Window win = {
	    .window = glfwCreateWindow(window_size_x, window_size_y, window_title, NULL, NULL),
	    .win_x  = window_size_x,
	    .win_y  = window_size_y};
	glfwMakeContextCurrent(win.window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		exit(-1);
	}

	glViewport(0, 0, win.win_x, win.win_y);
	return win;
}

void
window_Update(struct Window window_to_update) {
	// glfwSwapBuffers(window_to_update.window); // Double buffering enabled only? 
	// Should probably set a flag for double buffering later on.
	glFlush();
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

bool
window_Active(struct Window window_to_check) {
	return !glfwWindowShouldClose(window_to_check.window);
}
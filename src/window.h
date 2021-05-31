#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>


#include <GLFW/glfw3.h>
#include <stdbool.h>

struct Window {
	GLFWwindow* window;
	int win_x, win_y;
};

struct Window window_Init(int window_size_x, int window_size_y, const char* window_title);
void window_Update(struct Window window_to_update);
bool window_Active(struct Window window_to_check);
#endif
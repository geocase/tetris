#ifndef RENDERER_H
#define RENDERER_H
#include "shader.h"

typedef struct {
	float r, g, b, a;
} Color_t;

Color_t color_Normal(float r, float b, float g, float a, float divisor);

struct Renderer {
	float ren_x;
	float ren_y;

	float view_matrix[4][4];
	struct Shader flat_color;
	struct Shader line_color;
	struct Shader texture_color;

	unsigned int quad_vao;
	unsigned int line_vao;
	unsigned int texture_vao;

	unsigned int texture_index;
};

struct Renderer renderer_Init(float win_x, float win_y);

void renderer_LoadTexture(struct Renderer* to_load_to);
void renderer_DrawQuadBoundaries(struct Renderer to_render, float x0, float y0, float x1, float y1, Color_t color);
void renderer_DrawQuad(struct Renderer to_render, float x, float y, float sx, float sy, Color_t color);

void renderer_DrawTextureBoundaries(struct Renderer to_render, float x0, float y0, float x1, float y1, unsigned int texture);

void renderer_DrawLine(struct Renderer to_render, float line_width, float x0, float y0, float x1, float y1, Color_t color);
#endif

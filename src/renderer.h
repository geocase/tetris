#ifndef RENDERER_H
#define RENDERER_H
#include "shader.h"

typedef struct {
    float r, g, b, a;
} Color_t;

struct Renderer {
    float view_matrix[4][4];
    struct Shader flat_color;

    unsigned int vao;
};
struct Renderer renderer_Init(float win_x, float win_y);
void renderer_DrawQuad(struct Renderer to_render, float x0, float y0, float x1, float y1, Color_t color);
#endif

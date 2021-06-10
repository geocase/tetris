#ifndef RENDERER_H
#define RENDERER_H
#include "shader.h"

typedef struct {
	float r, g, b, a;
} Color_t;

Color_t color_Normal(float r, float b, float g, float a, float divisor);

typedef struct {
	int size_x;
	int size_y;
	unsigned int index;
} Texture_t;

typedef struct {
	unsigned int texture_index;
	int size_x;
	int size_y;
	int bearing_x;
	int bearing_y;
	unsigned int advance;
} CharacterGlyph_t;

struct Renderer {
	float ren_x;
	float ren_y;

	float view_matrix[4][4];
	struct Shader flat_color;
	struct Shader line_color;
	struct Shader texture_color;
	struct Shader character_color;

	unsigned int quad_vao;
	unsigned int line_vao;
	unsigned int texture_vao;

	Texture_t textures[2];

	CharacterGlyph_t char_glyphs[128];
	unsigned int glyph_vao;
	unsigned int glyph_vbo;
};

struct Renderer renderer_Init(float win_x, float win_y);

Texture_t renderer_LoadTexture(struct Renderer* to_load_to, const char* path);

void renderer_DrawQuadBoundaries(struct Renderer to_render, float x0, float y0, float x1, float y1, Color_t color);
void renderer_DrawQuad(struct Renderer to_render, float x, float y, float sx, float sy, Color_t color);

void renderer_DrawTextureBoundariesWithRepeat(
    struct Renderer to_render,
    float x0,
    float y0,
    float x1,
    float y1,
    float repeat_x,
    float repeat_y,
    unsigned int texture);

void renderer_DrawTextureWithRepeat(
    struct Renderer to_render,
    float x,
    float y,
    float sx,
    float sy,
    float repeat_x,
    float repeat_y,
    unsigned int texture);
void renderer_DrawTexture(struct Renderer to_render, float x, float y, float sx, float sy, unsigned int texture);

void
renderer_DrawLine(struct Renderer to_render, float line_width, float x0, float y0, float x1, float y1, Color_t color);

void renderer_DrawText(struct Renderer to_render, float x, float y, float scale, const char* text_to_draw);
#endif

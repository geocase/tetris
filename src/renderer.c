#include <glad/glad.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "renderer.h"

const char* vs = "#version 330 core\n"
                 "layout (location = 0) in vec2 a_pos;\n"
                 "uniform mat4 perspective;\n"
                 "uniform mat4 transform;\n"
                 "void main() {\n"
                 "    gl_Position = vec4(a_pos, 0.0, 1.0) * transform * perspective;\n"
                 "}";

const char* fs = "#version 330 core\n"
                 "out vec4 FragColor;\n"
                 "uniform vec4 in_color;\n"
                 "void main() {\n"
                 "  FragColor = in_color;\n"
                 "}";

const char* tex_vs = "#version 330 core\n"
                     "layout (location = 0) in vec2 a_pos;\n"
                     "layout (location = 1) in vec2 a_tex_coord;\n"
                     "uniform mat4 perspective;\n"
                     "uniform mat4 transform;\n"
                     "uniform vec2 repeat;\n"
                     "out vec2 tex_coord;\n"
                     "void main() {\n"
                     "    gl_Position = vec4(a_pos, 0.0, 1.0) * transform * perspective;\n"
                     "    tex_coord = a_tex_coord * repeat;\n"
                     "}";

const char* tex_fs = "#version 330 core\n"
                     "out vec4 FragColor;\n"
                     "in vec2 tex_coord;\n"
                     "uniform sampler2D in_texture;\n"
                     "void main() {\n"
                     "  FragColor = texture(in_texture, tex_coord) * vec4(1.0, 1.0, 1.0, 1.0);\n"
                     "}";

const char* char_vs = "#version 330 core\n"
                      "layout (location = 0) in vec4 vertex;\n"
                      "out vec2 tex_coords;\n"
                      "uniform mat4 perspective;\n"
                      "void main() {\n"
                      "    gl_Position = vec4(vertex.xy, 0.0, 1.0) * perspective;\n"
                      "    tex_coords = vertex.zw;\n"
                      "}";

const char* char_fs = "#version 330 core\n"
                      "in vec2 tex_coords;\n"
                      "out vec4 color;\n"
                      "uniform sampler2D text;\n"
                      "uniform vec3 text_color;\n"
                      "void main() {\n"
                      "	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);\n"
                      "	color = vec4(text_color, 1.0) * sampled;\n"
                      "}";

#define ASSET_DIR "assets/"

Color_t
color_Normal(float r, float b, float g, float a, float divisor) {
	return (Color_t){r / divisor, b / divisor, g / divisor, a / divisor};
}

static void
renderer_GlyphInit(struct Renderer* to_init_to) {
	FT_Library ft;
	if(FT_Init_FreeType(&ft)) {
		exit(-1);
	}
	FT_Face face;
	if(FT_New_Face(ft, ASSET_DIR "PressStart2P-Regular.ttf", 0, &face)) {
		exit(-1);
	}

	FT_Set_Pixel_Sizes(face, 0, 36);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for(unsigned int c = 0; c < 128; c++) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		glGenTextures(1, &(to_init_to->char_glyphs[c].texture_index));
		glBindTexture(GL_TEXTURE_2D, to_init_to->char_glyphs[c].texture_index);
		glTexImage2D(
		    GL_TEXTURE_2D,
		    0,
		    GL_RED,
		    face->glyph->bitmap.width,
		    face->glyph->bitmap.rows,
		    0,
		    GL_RED,
		    GL_UNSIGNED_BYTE,
		    face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		to_init_to->char_glyphs[c].size_x    = face->glyph->bitmap.width;
		to_init_to->char_glyphs[c].size_y    = face->glyph->bitmap.rows;
		to_init_to->char_glyphs[c].bearing_x = face->glyph->bitmap_left;
		to_init_to->char_glyphs[c].bearing_y = face->glyph->bitmap_top;
		to_init_to->char_glyphs[c].advance   = face->glyph->advance.x;
	}

	glGenVertexArrays(1, &(to_init_to->glyph_vao));
	glGenBuffers(1, &(to_init_to->glyph_vbo));
	glBindVertexArray(to_init_to->glyph_vao);
	glBindBuffer(GL_ARRAY_BUFFER, to_init_to->glyph_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

struct Renderer
renderer_Init(float win_x, float win_y) {
	struct Shader flat;
	shader_LoadFragmentShaderFromCharString(&flat, fs);
	shader_LoadVertexShaderFromCharString(&flat, vs);
	shader_GenerateProgram(&flat);

	struct Shader line;
	shader_LoadFragmentShaderFromCharString(&line, fs);
	shader_LoadVertexShaderFromCharString(&line, vs);
	shader_GenerateProgram(&line);

	struct Shader texture;
	shader_LoadFragmentShaderFromCharString(&texture, tex_fs);
	shader_LoadVertexShaderFromCharString(&texture, tex_vs);
	shader_GenerateProgram(&texture);

	struct Shader text;
	shader_LoadFragmentShaderFromCharString(&text, char_fs);
	shader_LoadVertexShaderFromCharString(&text, char_vs);
	shader_GenerateProgram(&text);

	// orthographic 2D matrix generation
	float near = -1.0f, far = 1.0f;
	float right = win_x, left = 0.0f;
	float bottom = win_y, top = 0.0f;
	struct Renderer r = {
	    .view_matrix =
	        {{2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left)},
	         {0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom)},
	         {0.0f, 0.0f, -2 / (far - near), -(far + near) / (far - near)},
	         {0.0f, 0.0f, 0.0f, 1.0f}},
	    .flat_color      = flat,
	    .line_color      = flat,
	    .texture_color   = texture,
	    .character_color = text,
	    .ren_x           = win_x,
	    .ren_y           = win_y};

	renderer_GlyphInit(&r);

	// glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float verts[] = {0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f};

	glGenVertexArrays(1, &(r.quad_vao));
	glBindVertexArray(r.quad_vao);

	unsigned int quad_vbo;
	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	float line_verts[] = {0.0f, 0.0f, 1.0f, 1.0f};

	glGenVertexArrays(1, &(r.line_vao));
	glBindVertexArray(r.line_vao);

	unsigned int line_vbo;
	glGenBuffers(1, &line_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_verts), line_verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	float texture_verts[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	                         0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

	glGenVertexArrays(1, &(r.texture_vao));
	glBindVertexArray(r.texture_vao);

	unsigned int texture_vbo;
	glGenBuffers(1, &texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_verts), texture_verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	r.textures[0] = renderer_LoadTexture(&r, ASSET_DIR "64x64.png");
	r.textures[1] = renderer_LoadTexture(&r, ASSET_DIR "game_over.png");

	return r;
}

Texture_t
renderer_LoadTexture(struct Renderer* to_load_to, const char* path) {
	Texture_t tex;

	glGenTextures(1, &(tex.index));
	glBindTexture(GL_TEXTURE_2D, tex.index);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// load texture
	int nr_channels;
	unsigned char* image = stbi_load(path, &(tex.size_x), &(tex.size_y), &nr_channels, 0);
	if(nr_channels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.size_x, tex.size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.size_x, tex.size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	return tex;
}

void
renderer_DrawQuadBoundaries(struct Renderer to_render, float x0, float y0, float x1, float y1, Color_t color) {
	float transform[4][4] = {{x1 - x0, 0, 0, x0}, {0, y1 - y0, 0, y0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	shader_Use(to_render.flat_color);

	int vertex_color = glGetUniformLocation(to_render.flat_color.program, "in_color");
	int view         = glGetUniformLocation(to_render.flat_color.program, "perspective");
	int tform        = glGetUniformLocation(to_render.flat_color.program, "transform");

	glUniform4f(vertex_color, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(view, 1, GL_FALSE, (float*)to_render.view_matrix);
	glUniformMatrix4fv(tform, 1, GL_FALSE, (float*)transform);
	glBindVertexArray(to_render.quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void
renderer_DrawQuad(struct Renderer to_render, float x, float y, float sx, float sy, Color_t color) {
	renderer_DrawQuadBoundaries(to_render, x, y, sx + x, sy + y, color);
}

void
renderer_DrawTextureBoundariesWithRepeat(
    struct Renderer to_render,
    float x0,
    float y0,
    float x1,
    float y1,
    float repeat_x,
    float repeat_y,
    unsigned int texture) {
	float transform[4][4] = {{x1 - x0, 0, 0, x0}, {0, y1 - y0, 0, y0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	shader_Use(to_render.texture_color);

	int view   = glGetUniformLocation(to_render.texture_color.program, "perspective");
	int tform  = glGetUniformLocation(to_render.texture_color.program, "transform");
	int repeat = glGetUniformLocation(to_render.texture_color.program, "repeat");
	glUniformMatrix4fv(view, 1, GL_FALSE, (float*)to_render.view_matrix);
	glUniformMatrix4fv(tform, 1, GL_FALSE, (float*)transform);
	glUniform2f(repeat, repeat_x, repeat_y);
	glBindTexture(GL_TEXTURE_2D, to_render.textures[texture].index);
	glBindVertexArray(to_render.texture_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void
renderer_DrawTextureWithRepeat(
    struct Renderer to_render,
    float x,
    float y,
    float sx,
    float sy,
    float repeat_x,
    float repeat_y,
    unsigned int texture) {
	renderer_DrawTextureBoundariesWithRepeat(to_render, x, y, sx + x, sy + y, repeat_x, repeat_y, texture);
}

void
renderer_DrawTexture(struct Renderer to_render, float x, float y, float sx, float sy, unsigned int texture) {
	renderer_DrawTextureBoundariesWithRepeat(to_render, x, y, sx + x, sy + y, 1.0f, 1.0f, texture);
}

void
renderer_DrawLine(struct Renderer to_render, float line_width, float x0, float y0, float x1, float y1, Color_t color) {
	float transform[4][4] = {{x1 - x0, 0, 0, x0}, {0, y1 - y0, 0, y0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	int vertex_color      = glGetUniformLocation(to_render.flat_color.program, "in_color");
	int view              = glGetUniformLocation(to_render.flat_color.program, "perspective");
	int tform             = glGetUniformLocation(to_render.flat_color.program, "transform");

	glUniform4f(vertex_color, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(view, 1, GL_FALSE, (float*)to_render.view_matrix);
	glUniformMatrix4fv(tform, 1, GL_FALSE, (float*)transform);

	glBindVertexArray(to_render.line_vao);
	glLineWidth(line_width);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1.0f);

	glBindVertexArray(0);
}

void
renderer_DrawText(struct Renderer to_render, float x, float y, float scale, const char* text_to_draw) {
	shader_Use(to_render.character_color);
	glUniform3f(glGetUniformLocation(to_render.character_color.program, "text_color"), 0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(
	    glGetUniformLocation(to_render.character_color.program, "perspective"),
	    1,
	    GL_FALSE,
	    (float*)to_render.view_matrix);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(to_render.glyph_vao);

	for(int c = 0; c < strlen(text_to_draw); ++c) {
		CharacterGlyph_t ch = to_render.char_glyphs[text_to_draw[c]];

		float xpos = x + ch.bearing_x * scale;
		float ypos = y - ch.bearing_y * scale;

		float w = ch.size_x * scale;
		float h = ch.size_y * scale;

		float vertices[6][4] = {
		    {xpos, ypos + h, 0.0f, 1.0f},
		    {xpos, ypos, 0.0f, 0.0f},
		    {xpos + w, ypos, 1.0f, 0.0f},

		    {xpos, ypos + h, 0.0f, 1.0f},
		    {xpos + w, ypos, 1.0f, 0.0f},
		    {xpos + w, ypos + h, 1.0f, 1.0f}};

		glBindTexture(GL_TEXTURE_2D, ch.texture_index);
		glBindBuffer(GL_ARRAY_BUFFER, to_render.glyph_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
	}
}
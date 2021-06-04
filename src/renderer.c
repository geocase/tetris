#include <glad/glad.h>
#include <stdlib.h>

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
				 "out vec2 tex_coord;\n"
                 "void main() {\n"
                 "    gl_Position = vec4(a_pos, 0.0, 1.0) * transform * perspective;\n"
				 "    tex_coord = a_tex_coord;\n"
                 "}";

const char* tex_fs = "#version 330 core\n"
                 "out vec4 FragColor;\n"
                 "in vec2 tex_coord;\n"
				 "uniform sampler2D in_texture;\n"
                 "void main() {\n"
                 "  FragColor = texture(in_texture, tex_coord);\n"
                 "}";

Color_t
color_Normal(float r, float b, float g, float a, float divisor) {
	return (Color_t){r / divisor, b / divisor, g / divisor, a / divisor};
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
	    .flat_color = flat,
	    .line_color = flat,
		.texture_color = texture,
	    .ren_x      = win_x,
	    .ren_y      = win_y};

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

	float texture_verts[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

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



	renderer_LoadTexture(&r);

	return r;
}

void renderer_LoadTexture(struct Renderer* to_load_to) {
	// gen 256 x 256 random image
	unsigned char image[256 * 256 * 3];
	for(int x = 0; x < 256 * 256 * 3; x++) {
		image[x] = rand() % 255;
	}

	glGenTextures(1, &(to_load_to->texture_index));
	glBindTexture(GL_TEXTURE_2D, to_load_to->texture_index);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

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
renderer_DrawTextureBoundaries(struct Renderer to_render, float x0, float y0, float x1, float y1, unsigned int texture) {
	float transform[4][4] = {{x1 - x0, 0, 0, x0}, {0, y1 - y0, 0, y0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	shader_Use(to_render.texture_color);

	int view         = glGetUniformLocation(to_render.texture_color.program, "perspective");
	int tform        = glGetUniformLocation(to_render.texture_color.program, "transform");
	// int tex = glGetUniformLocation(to_render.texture_color.program, "in_texture");
	glUniformMatrix4fv(view, 1, GL_FALSE, (float*)to_render.view_matrix);
	glUniformMatrix4fv(tform, 1, GL_FALSE, (float*)transform);
	glBindTexture(GL_TEXTURE_2D, to_render.texture_index);
	glBindVertexArray(to_render.texture_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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

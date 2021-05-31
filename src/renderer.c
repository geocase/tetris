#include <glad/glad.h>

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

struct Renderer renderer_Init(float win_x, float win_y) {
    struct Shader flat;
    shader_LoadFragmentShaderFromCharString(&flat, fs);
    shader_LoadVertexShaderFromCharString(&flat, vs);
    shader_GenerateProgram(&flat);

    // orthographic 2D matrix generation
    float near = -1.0f, far = 1.0f;
    float right = win_x, left = 0.0f;
    float bottom = win_y, top = 0.0f;
    struct Renderer r = {
        .view_matrix = {
            {2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left)},
            {0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom)},
            {0.0f, 0.0f, -2 / (far - near), -(far + near) / (far - near)},
            {0.0f, 0.0f, 0.0f, 1.0f}},
        .flat_color = flat
    };

    float verts[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &(r.vao));
    glBindVertexArray(r.vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return r;
}

void renderer_DrawQuad(struct Renderer to_render, float x0, float y0, float x1, float y1, Color_t color) {
    float transform[4][4] = {
        {x1 - x0, 0, 0, x0},
        {0, y1 - y0, 0, y0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };


    // single color for now
    shader_Use(to_render.flat_color);

    int vertex_color = glGetUniformLocation(to_render.flat_color.program, "in_color");
    int view = glGetUniformLocation(to_render.flat_color.program, "perspective");
    int tform = glGetUniformLocation(to_render.flat_color.program, "transform");

    glUniform4f(vertex_color, color.r, color.g, color.b, color.a);  
    glUniformMatrix4fv(view, 1, GL_FALSE, (float*)to_render.view_matrix); 
    glUniformMatrix4fv(tform, 1, GL_FALSE, (float*)transform); 
    glBindVertexArray(to_render.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

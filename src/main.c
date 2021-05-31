#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "shader.h"
#include "tetris.h"


int main() {
    struct Tetris game = {
        .score = 0,
        .level = 0
    };
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

    // float vertices[] = {
    //     100.0f, 100.0f,
    //     0.0f, 400.0f,
    //     100.0f, 479.0f
    // };

   
    
    // unsigned int vao;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);

    // unsigned int vbo;
    // glGenBuffers(1, &vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    struct Renderer ren = renderer_Init(640.0f, 480.0f);

    float k = 0;
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // int vertex_color = glGetUniformLocation(ren.flat_color.program, "in_color");
        // int view = glGetUniformLocation(ren.flat_color.program, "perspective");
        // shader_Use(ren.flat_color);
        // glUniform4f(vertex_color, 1.0f, 1.0f, 0.0f, 1.0f);  
        // glUniformMatrix4fv(view, 1, GL_FALSE, (float *)ren.view_matrix);      
        // glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        renderer_DrawQuad(ren, 100, 100, 540, 380, (Color_t){1.0f, 0.0f, 0.0f, 1.0f});
        renderer_DrawQuad(ren, 150, 100, 540, 380, (Color_t){1.0f, 1.0f, 0.0f, 1.0f});


        glfwSwapBuffers(window);
        glfwPollEvents();
        k += 0.0001f;
    }

    glfwTerminate();

    return 0;
}
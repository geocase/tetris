#include <glad/glad.h>
#include <stdio.h>

#include "shader.h"

bool shader_LoadFragmentShaderFromCharString(struct Shader* to_load_to, const char* to_load_from) {
    to_load_to->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(to_load_to->fragment, 1, &to_load_from, NULL);
    glCompileShader(to_load_to->fragment);

    int success;
    char info[512];
    glGetShaderiv(to_load_to->fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(to_load_to->fragment, 512, NULL, info);
        printf("Fragment Shader Error:\n%s", info);
    }
    return (bool)success;
}

bool shader_LoadVertexShaderFromCharString(struct Shader* to_load_to, const char* to_load_from) {
    to_load_to->vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(to_load_to->vertex, 1, &to_load_from, NULL);
    glCompileShader(to_load_to->vertex);

    int success;
    char info[512];
    glGetShaderiv(to_load_to->vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(to_load_to->vertex, 512, NULL, info);
        printf("Vertex Shader Error:\n%s", info);
    }
    return (bool)success;
}

bool shader_GenerateProgram(struct Shader* to_generate_to) {
    to_generate_to->program = glCreateProgram();
    glAttachShader(to_generate_to->program, to_generate_to->vertex);
    glAttachShader(to_generate_to->program, to_generate_to->fragment);
    glLinkProgram(to_generate_to->program);

    int success;
    char info[512];
    glGetProgramiv(to_generate_to->program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(to_generate_to->program, 512, NULL, info);
        printf("Linking error:\n%s", info);
    }
    return (bool)success;
}

void shader_Use(struct Shader to_use) {
    glUseProgram(to_use.program);
}
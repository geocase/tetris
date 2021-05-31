#ifndef SHADER_H
#define SHADER_H
#include <stdbool.h>
struct Shader {
	unsigned int fragment;
	unsigned int vertex;
	unsigned int program;
};
bool shader_LoadFragmentShaderFromCharString(struct Shader* to_load_to, const char* to_load_from);
bool shader_LoadVertexShaderFromCharString(struct Shader* to_load_to, const char* to_load_from);
bool shader_GenerateProgram(struct Shader* to_generate_to);
void shader_Use(struct Shader to_use);
#endif
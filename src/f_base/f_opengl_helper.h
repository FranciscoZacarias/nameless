#ifndef F_OPENGL_HELPER_H
#define F_OPENGL_HELPER_H

typedef GLuint OGL_Shader;

global GLuint vao, vbo, instance_vbo;
global OGL_Shader program;

internal OGL_Shader ogl_make_shader(const char* shader_path, GLenum kind);
internal OGL_Shader ogl_make_program(Arena *arena, GLuint *shaders, u32 count);

#endif // F_OPENGL_HELPER_H
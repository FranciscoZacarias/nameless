#ifndef F_OPENGL_HELPER_H
#define F_OPENGL_HELPER_H

typedef GLuint OGL_Shader;

global GLuint Vao, Vbo, Instanced_Vbo;
global OGL_Shader ShaderProgram;

internal OGL_Shader ogl_make_shader(String shader_path, GLenum kind);
internal OGL_Shader ogl_make_program(GLuint *shaders, u32 count);

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat);

#endif // F_OPENGL_HELPER_H
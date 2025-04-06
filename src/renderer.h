#ifndef RENDERER_H
#define RENDERER_H

#define VERTEX_SHADER_PATH   "D:/work/nameless/src/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/fragment_shader.glsl"

///////////////////////
//~ OpenGL structures

global GLuint Vao, Vbo, Instanced_Vbo;
global OGL_Shader ShaderProgram;

///////////////////////
//~ Renderer Primitives 

// Instanced Triangle
#define RENDERER_MAX_TRIANGLES Kilobytes(8)*3
typedef struct Triangle_Instance {
  Transformf32 transform;
  Vec4f32 color;
} Triangle_Instance;

global GLuint VAO_Triangle;
global GLuint VBO_Triangle;
global GLuint VBO_TriangleInstance;

///////////////////////
//~ Renderer

// Renderer Struct
typedef struct Renderer_State {
  Arena* arena;

  Triangle_Instance*  triangles_data;
  u32                 triangles_count;
  u32                 triangles_max;
} Renderer_State;

// Renderer globals
global Renderer_State Renderer;
global Vec2f32 Translations[100];

// Renderer core functions
internal void renderer_init();
internal void renderer_begin_frame();
internal void renderer_end_frame(Mat4f32 view, Mat4f32 projection);

// Renderer primitives
internal void renderer_push_triangle(Transformf32 transform, Vec4f32 color);

#endif // RENDERER_H
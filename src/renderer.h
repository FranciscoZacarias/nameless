#ifndef RENDERER_H
#define RENDERER_H

#define TRIANGLE_VERTEX_SHADER_PATH   "D:/work/nameless/src/shaders/triangle_vertex_shader.glsl"
#define TRIANGLE_FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/triangle_fragment_shader.glsl"

#define LINE_VERTEX_SHADER_PATH   "D:/work/nameless/src/shaders/line_vertex_shader.glsl"
#define LINE_FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/line_fragment_shader.glsl"

///////////////////////
//~ Renderer Primitives 

// Instanced Lines
#define RENDERER_MAX_LINES Kilobytes(8)*2
typedef struct Line_Instance {
  Vec3f32 start;
  Vec3f32 end;
  Vec4f32 color;
} Line_Instance;

global GLuint VAO_Line;
global GLuint VBO_Line;
global GLuint VBO_LineInstance;
global OGL_Shader LineProgram;

// Instanced Triangle
#define RENDERER_MAX_TRIANGLES Kilobytes(16)*3
typedef struct Triangle_Instance {
  Transformf32 transform;
  Vec4f32 color;
} Triangle_Instance;

global GLuint VAO_Triangle;
global GLuint VBO_Triangle;
global GLuint VBO_TriangleInstance;
global OGL_Shader TriangleProgram;

///////////////////////
//~ Renderer

// Renderer Struct
typedef struct Renderer_State {
  Arena* arena;

  Line_Instance* lines_data;
  u32            lines_count;
  u32            lines_max;

  Triangle_Instance* triangles_data;
  u32                triangles_count;
  u32                triangles_max;
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
internal void renderer_push_line(Vec3f32 start, Vec3f32 end, Vec4f32 color);

#endif // RENDERER_H
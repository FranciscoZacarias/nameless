#ifndef RENDERER_H
#define RENDERER_H

#define RAW_VERTEX_SHADER_PATH       "D:/work/nameless/src/shaders/raw_vertex_shader.glsl"
#define INSTANCED_VERTEX_SHADER_PATH "D:/work/nameless/src/shaders/instanced_vertex_shader.glsl"

#define FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/fragment_shader.glsl"

///////////////////////
//~ Renderer Primitives 

// Instanced Lines
#define RENDERER_MAX_LINES Megabytes(1)*2
typedef struct Line_Instance {
  Vec3f32 start;
  Vec3f32 end;
  Vec4f32 color;
} Line_Instance;

global GLuint VAO_Line;
global GLuint VBO_Line;
global GLuint VBO_LineInstance;

// Instanced Triangle
#define RENDERER_MAX_TRIANGLES Megabytes(2)*3
typedef struct Triangle_Instance {
  Transformf32 transform;
  Vec4f32 color;
  GLuint texture_id; // 0 means no texture
} Triangle_Instance;

global GLuint VAO_Triangle;
global GLuint VBO_Triangle;
global GLuint VBO_TriangleInstance;

// Instanced Quads
#define RENDERER_MAX_QUADS Megabytes(2)*4
typedef struct Quad_Instance {
    Transformf32 transform;
    Vec4f32 color;
} Quad_Instance;

global GLuint VAO_Quad;
global GLuint VBO_Quad;
global GLuint VBO_QuadInstance;

// OGL Programs
global OGL_Shader RawProgram;
global OGL_Shader InstancedProgram;

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

  Quad_Instance* quads_data;
  u32            quads_count;
  u32            quads_max;

} Renderer_State;

global Renderer_State Renderer;

// Textures
typedef struct Texture {
  GLuint id;
  String path;
} Texture;

global Texture textures[64];
global u32 texture_count = 0;

// Renderer core functions
internal void renderer_init();
internal void renderer_begin_frame();
internal void renderer_end_frame(Mat4f32 view, Mat4f32 projection);

// Renderer primitives
internal void renderer_push_line(Vec3f32 start, Vec3f32 end, Vec4f32 color);
internal void renderer_push_triangle(Transformf32 transform, Vec4f32 color);
internal void renderer_push_triangle_texture(Transformf32 transform, Vec4f32 color, GLuint texture_id);internal void renderer_push_quad(Transformf32 transform, Vec4f32 color);
internal void renderer_push_arrow(Vec3f32 start, Vec3f32 end, Vec4f32 color);
internal void renderer_push_box(Vec3f32 min, Vec3f32 max, Vec4f32 color);
internal void renderer_push_grid(Vec3f32 center, Vec3f32 normal, f32 spacing, s32 count, Vec4f32 color);

#endif // RENDERER_H
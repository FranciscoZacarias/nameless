#ifndef RENDERER_H
#define RENDERER_H

#define VERTEX_SHADER_PATH   "D:/work/nameless/src/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/fragment_shader.glsl"

#define RENDERER_MAX_TRIANGLES Kilobytes(4)*3

typedef struct Renderer_Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
} Renderer_Vertex;
#define renderer_vertex(p,c,u) (Renderer_Vertex){p,c,u}

// Instanced triangle 
typedef struct Triangle_Instance {
  Renderer_Vertex p0;
  Renderer_Vertex p1;
  Renderer_Vertex p2;
} Triangle_Instance;

global GLuint VAO_Triangle;
global GLuint VBO_Triangle;
global GLuint VBO_TriangleInstance;

// Renderer Struct
typedef struct Renderer_State {
  Arena* arena;

  //Renderer_Vertex* points_vertices;
  //u32              points_count;

  //Renderer_Vertex* lines_vertices;
  //u32              lines_count;

  Triangle_Instance* triangles_data;
  u32                triangles_count;
  u32                triangles_max;

  //Renderer_Vertex* quads_vertices;
  //u32              quads_count;

} Renderer_State;

// Renderer globals
global Renderer_State Renderer;
global Vec2f32 Translations[100];

// Renderer core functions
internal void renderer_init();
internal void renderer_begin_frame();
internal void renderer_end_frame();

// Renderer primitives
//internal void renderer_push_point   (Vec3f32 p0, Vec4f32 p0_color);
//internal void renderer_push_line    (Vec3f32 p0, Vec4f32 p0_color, Vec3f32 p1, Vec4f32 p1_color);
internal void renderer_push_triangle(Vec3f32 p0, Vec4f32 p0_color, Vec3f32 p1, Vec4f32 p1_color, Vec3f32 p2, Vec4f32 p2_color);
//internal void renderer_push_quad    (Vec3f32 p0, Vec4f32 p0_color, Vec3f32 p1, Vec4f32 p1_color, Vec3f32 p2, Vec4f32 p2_color, Vec3f32 p3, Vec4f32 p3_color);

#endif // RENDERER_H
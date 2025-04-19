#ifndef RENDERER_H
#define RENDERER_H

#define RAW_VERTEX_SHADER_PATH       "D:/work/nameless/src/shaders/raw_vertex_shader.glsl"
#define INSTANCED_VERTEX_SHADER_PATH "D:/work/nameless/src/shaders/instanced_vertex_shader.glsl"

#define FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/fragment_shader.glsl"

#define FONT_INCONSOLATA "D:/work/nameless/fonts/Inconsolata.otf"
#define FONT_SPACEMONO "D:/work/nameless/fonts/SpaceMono-Regular.ttf"

///////////////////////
//~ Renderer Primitives 

typedef struct Vertex {
  Vec3f32 position;
  Vec2f32 uv;
} Vertex;

// Font
typedef struct Glyph {
  Vec2f32 uv_min; // Bottom-left UV in atlas
  Vec2f32 uv_max; // Top-right UV in atlas
  Vec2f32 size;
  Vec2f32 offset;
  f32 advance;
} Glyph;

typedef struct Font {
  Glyph glyphs[126];
  u32 texture_id;
  f32 height;
} Font;

// Instanced Lines
#define RENDERER_MAX_LINES Megabytes(1)*2
typedef struct Line_Instance {
  Vec3f32 start;
  Vec3f32 end;
  Vec4f32 color;
  u8 padding[8];
} Line_Instance;

global GLuint Vao_Line;
global GLuint Vbo_LineInstance;

// Instanced Triangle
typedef enum Instanced_Type {
  Instanced_Triangle,
  Instanced_Quad,
} Instanced_Type;

#define RENDERER_MAX_INSTANCED_DATA Megabytes(12)
typedef struct Instanced_Data {
  Transformf32 transform;
  Vec4f32 color;
  u32 texture_id;
  b32 is_screen_space;
} Instanced_Data;

global GLuint Vbo_InstancedData; // Shared for Instanced_Data
global GLuint Vao_Triangle, Vbo_Triangle;
global GLuint Vao_Quad, Vbo_Quad, Ebo_Quad;

// OGL Programs
global OGL_Shader RawProgram;
global OGL_Shader InstancedProgram;

///////////////////////
//~ Renderer

// Renderer Struct
typedef struct Renderer_State {
  Arena* arena;

  // Line primitives
  Line_Instance* lines_data;
  u32            lines_count;
  u32            lines_max;

  // Instanced Data
  Instanced_Data* instanced_data;
  u32             instanced_max;
  u32             instanced_count;
  u32             triangle_count;
  u32             quad_count;

  // Textures
  GLuint* textures;
  u32 texture_count;
  u32 texture_max;

  // Font
  Font font;
  
} Renderer_State;

global Renderer_State Renderer;

// Renderer core functions
internal void renderer_init();
internal void renderer_begin_frame();
internal void renderer_end_frame(Mat4f32 view, Mat4f32 projection);

// Renderer load functions
internal u32 renderer_load_texture(String8 path); /* Returns the index into Renderer.textures[] */
internal u32 renderer_load_font(String8 path, f32 font_height);

// Renderer text
internal void renderer_push_text_screenspace(Vec2f32 position, f32 scale, Vec4f32 color, String8 text);
internal void renderer_push_text_worldspace(Vec3f32 position, f32 scale, Vec4f32 color, String8 text);

// Renderer primitives
internal void renderer_push_line(Vec3f32 start, Vec3f32 end, Vec4f32 color);
internal void renderer_push_arrow(Vec3f32 start, Vec3f32 end, Vec4f32 color);
internal void renderer_push_box(Vec3f32 min, Vec3f32 max, Vec4f32 color);
internal void renderer_push_grid(Vec3f32 center, Vec3f32 normal, f32 spacing, s32 count, Vec4f32 color);

internal void renderer_push_triangle(Transformf32 transform, Vec4f32 color);
internal void renderer_push_triangle_texture(Transformf32 transform, u32 texture_id);
internal void renderer_push_quad(Transformf32 transform, Vec4f32 color);
internal void renderer_push_quad_texture(Transformf32 transform, u32 texture_id);

#endif // RENDERER_H
#ifndef RENDERER_H
#define RENDERER_H

#define VERTEX_SHADER_PATH   "D:/work/nameless/src/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_PATH "D:/work/nameless/src/shaders/fragment_shader.glsl"

typedef struct Renderer_Vertex {
  Vec3f32 position;
  Vec4f32 color;
  Vec2f32 uv;
} Renderer_Vertex;

typedef struct Renderer {
  u32 temp;
} Renderer;

internal void renderer_init(Arena* arena);
internal void renderer_begin_frame();
internal void renderer_end_frame();

#endif // RENDERER_H
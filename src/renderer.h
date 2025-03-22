#ifndef RENDERER_H
#define RENDERER_H

#define VERTEX_SHADER_PATH   "./src/shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER_PATH "./src/shaders/fragment_shader.glsl"

internal void renderer_init(Arena* arena);
internal void renderer_begin_frame();
internal void renderer_end_frame();

#endif // RENDERER_H
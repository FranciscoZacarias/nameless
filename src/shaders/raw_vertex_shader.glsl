#version 460 core

layout(location = 0) in vec3 start;
layout(location = 1) in vec3 end;
layout(location = 2) in vec4 color;

out vec4 frag_color;

// Textures not rendered in this program, but we are sharing the same fragment shader
flat out uint texture_id;
out vec2 frag_texcoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
  vec3 position = gl_VertexID == 0 ? start : end;
  gl_Position   = projection * view * vec4(position, 1.0);
  frag_color    = color;
  texture_id    = 0;
}

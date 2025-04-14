#version 460 core

layout(location = 0) in vec3 base_position;
layout(location = 1) in vec3 instance_translation;
layout(location = 2) in vec4 instance_rotation; // Quaternion
layout(location = 3) in vec3 instance_scale;
layout(location = 4) in vec4 instance_color;
layout(location = 5) in vec2 base_texcoord; // Unused

uniform mat4 projection;
uniform mat4 view;

out vec4 frag_color;
out vec2 frag_texcoord; // Unused in fragment shader

vec3 rotate(vec3 v, vec4 q) {
  vec3 t = 2.0 * cross(q.xyz, v);
  return v + q.w * t + cross(q.xyz, t);
}

void main() {
  vec3 scaled_pos  = base_position * instance_scale;
  vec3 rotated_pos = rotate(scaled_pos, instance_rotation);
  vec3 final_pos   = rotated_pos + instance_translation;

  gl_Position      = projection * view * vec4(final_pos, 1.0);
  frag_color       = instance_color; // Pass the instance color
}

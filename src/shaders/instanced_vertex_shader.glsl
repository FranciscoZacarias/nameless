#version 460 core

layout(location = 0) in vec3 base_position;
layout(location = 1) in vec2 base_texcoord;
layout(location = 2) in vec3 instance_translation;
layout(location = 3) in vec4 instance_rotation;
layout(location = 4) in vec3 instance_scale;
layout(location = 5) in vec4 instance_color;
layout(location = 6) in uint instance_texture_id;
layout(location = 7) in vec2 instance_uv_min;
layout(location = 8) in vec2 instance_uv_max;
layout(location = 9) in int instance_is_screen_space;

uniform mat4 projection;
uniform mat4 view;
uniform float screen_width;
uniform float screen_height;

out vec4 frag_color;
out vec2 frag_texcoord;
flat out uint texture_id;

vec3 rotate(vec3 v, vec4 q) {
  vec3 t = 2.0 * cross(q.xyz, v);
  return v + q.w * t + cross(q.xyz, t);
}

void main() {
  vec3 scaled_pos  = base_position * instance_scale;
  vec3 rotated_pos = rotate(scaled_pos, instance_rotation);
  vec3 final_pos   = rotated_pos + instance_translation;

  if (instance_is_screen_space == 1) {
    float screen_x = final_pos.x;
    float screen_y = final_pos.y;
    float ndc_x = 2.0 * (screen_x / screen_width) - 1.0;
    float ndc_y = 1.0 - 2.0 * (screen_y / screen_height);
    gl_Position = vec4(ndc_x, ndc_y, -1.0, 1.0);
  } else {
    gl_Position = projection * view * vec4(final_pos, 1.0);
  }

  frag_color    = instance_color;
  texture_id    = instance_texture_id;
  frag_texcoord = mix(instance_uv_min, instance_uv_max, base_texcoord);
}
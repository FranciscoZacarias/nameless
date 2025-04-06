#version 330 core
layout(location = 0) in vec3 in_start;
layout(location = 1) in vec3 in_end;
layout(location = 2) in vec4 in_color;

out vec4 v_color;

uniform mat4 projection;
uniform mat4 view;

void main() {
  vec3 position = gl_VertexID == 0 ? in_start : in_end;
  gl_Position = projection * view * vec4(position, 1.0);
  v_color = in_color;
}
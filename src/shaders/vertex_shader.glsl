#version 460 core

layout(location = 0) in vec3 aPos; // Unused (dummy binding to VBO_Triangle)

// REGION: Triangle Instances
layout(location = 1) in vec3 instP0Pos;
layout(location = 2) in vec4 instP0Color;
layout(location = 3) in vec2 instP0UV;

layout(location = 4) in vec3 instP1Pos;
layout(location = 5) in vec4 instP1Color;
layout(location = 6) in vec2 instP1UV;

layout(location = 7) in vec3 instP2Pos;
layout(location = 8) in vec4 instP2Color;
layout(location = 9) in vec2 instP2UV;
// ENDREGION: Triangle Instances

out vec4 vertexColor; // Pass color to fragment shader

void main() {
  vec3 pos;
  if (gl_VertexID % 3 == 0) { // Vertex 0 of the triangle
    pos = instP0Pos;
    vertexColor = instP0Color;
  } else if (gl_VertexID % 3 == 1) { // Vertex 1 of the triangle
    pos = instP1Pos;
    vertexColor = instP1Color;
  } else if (gl_VertexID % 3 == 2) { // Vertex 2 of the triangle
    pos = instP2Pos;
    vertexColor = instP2Color;
  }
  gl_Position = vec4(pos, 1.0);
}
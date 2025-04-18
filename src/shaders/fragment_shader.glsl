#version 460 core

in vec4 frag_color;
in uint texture_id;

out vec4 FragColor;

void main() {
  FragColor = frag_color;
}

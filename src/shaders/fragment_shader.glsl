#version 330 core
in vec4 frag_color;
in vec2 frag_texcoord;
out vec4 FragColor;

uniform sampler2D texture0;
uniform bool use_texture;

void main() {
  if (use_texture) {
    vec4 tex_color = texture(texture0, frag_texcoord);
    FragColor = tex_color * frag_color;
  } else {
    FragColor = frag_color;
  }
}
#version 460 core

in vec4 frag_color;
in vec2 frag_texcoord;

flat in uint texture_id;

uniform sampler2D textures[32];
uniform uint texture_count;

out vec4 FragColor;

void main() {
  if (texture_id == 0 || texture_id > texture_count) {
    FragColor = frag_color; // Solid color or invalid texture
  } else {
    // NOTE(fz): We subtract -1 from texture_id because we are using index 1 as the initial texture.
    vec4 tex = texture(textures[texture_id - 1], frag_texcoord);
    FragColor = frag_color * tex;
  }
}

internal void renderer_init(Arena* arena) {
  Arena_Temp scratch = scratch_begin(0, 0);
  
  glEnable(GL_DEPTH_TEST);
  
  String vertex_shader_source = file_load(scratch.arena, StringLiteral(VERTEX_SHADER_PATH));
  OGL_Shader vertex_shader    = ogl_make_shader(vertex_shader_source.str, GL_VERTEX_SHADER);

  String fragment_shader_source = file_load(scratch.arena, StringLiteral(FRAGMENT_SHADER_PATH));
  OGL_Shader fragment_shader    = ogl_make_shader(fragment_shader_source.str, GL_FRAGMENT_SHADER);

  GLuint shaders[] = { vertex_shader, fragment_shader };
  program = ogl_make_program(arena, shaders, 2);

  scratch_end(&scratch);
}

internal void renderer_begin_frame() {

}

internal void renderer_end_frame() {

}

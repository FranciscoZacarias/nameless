internal void renderer_init(Arena* arena) {
  Arena_Temp scratch = scratch_begin(0, 0);
  
  glEnable(GL_DEPTH_TEST);

  const GLubyte* glversion = glGetString(GL_VERSION);
  printf("Opengl version: %s\n", glversion);
  
  OGL_Shader vertex_shader   = ogl_make_shader(StringLiteral(VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
  OGL_Shader fragment_shader = ogl_make_shader(StringLiteral(FRAGMENT_SHADER_PATH), GL_FRAGMENT_SHADER);

  GLuint shaders[] = { vertex_shader, fragment_shader };
  ShaderProgram = ogl_make_program(shaders, 2);

  scratch_end(&scratch);
}

internal void renderer_begin_frame() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

internal void renderer_end_frame() {
  glUseProgram(ShaderProgram);
  glBindVertexArray(Vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
  glBindVertexArray(0);
}

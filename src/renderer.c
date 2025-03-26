internal void renderer_init() {
  AssertNoReentry();

  Arena_Temp scratch = scratch_begin(0, 0);
  
  glEnable(GL_DEPTH_TEST);

  const GLubyte* glversion = glGetString(GL_VERSION);
  printf("Opengl version: %s\n", glversion);

  // Renderer Setup
  MemoryZeroStruct(&Renderer);
  Renderer.arena = arena_init();

  Assert(RENDERER_MAX_TRIANGLES % 3 == 0);
  Renderer.triangles_max   = RENDERER_MAX_TRIANGLES / 3;
  Renderer.triangles_data  = ArenaPush(Renderer.arena, Triangle_Instance, RENDERER_MAX_TRIANGLES);
  Renderer.triangles_count = 0;

  
  // Opengl Shaders and Program
  OGL_Shader vertex_shader   = ogl_make_shader(StringLiteral(VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
  OGL_Shader fragment_shader = ogl_make_shader(StringLiteral(FRAGMENT_SHADER_PATH), GL_FRAGMENT_SHADER);
  GLuint shaders[] = { vertex_shader, fragment_shader };
  ShaderProgram    = ogl_make_program(shaders, 2);

  // Triangle setup: no base vertex data, just buffer creation
  glGenVertexArrays(1, &VAO_Triangle);
  glGenBuffers(1,      &VBO_Triangle);
  glGenBuffers(1,      &VBO_TriangleInstance);

  glBindVertexArray(VAO_Triangle);
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);
    {
      // No glBufferData call—leave it empty
      glEnableVertexAttribArray(0); // Placeholder for base positions (unused)
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Dummy binding
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleInstance);
    {
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p0.position));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p0.color));
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p0.uv));
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p1.position));
      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p1.color));
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p1.uv));
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p2.position));
      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p2.color));
      glEnableVertexAttribArray(9);
      glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, p2.uv));
      glVertexAttribDivisor(1, 1);
      glVertexAttribDivisor(2, 1);
      glVertexAttribDivisor(3, 1);
      glVertexAttribDivisor(4, 1);
      glVertexAttribDivisor(5, 1);
      glVertexAttribDivisor(6, 1);
      glVertexAttribDivisor(7, 1);
      glVertexAttribDivisor(8, 1);
      glVertexAttribDivisor(9, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  } 
  glBindVertexArray(0);

  scratch_end(&scratch);
}

internal void renderer_begin_frame() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Renderer.triangles_count = 0;
}

internal void renderer_end_frame() {
  glUseProgram(ShaderProgram);


  if (Renderer.triangles_count > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleInstance);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_Instance) * Renderer.triangles_count, Renderer.triangles_data, GL_STATIC_DRAW);
    glBindVertexArray(VAO_Triangle);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, Renderer.triangles_count);
  }

  glBindVertexArray(0);
}

internal void renderer_push_triangle(Vec3f32 p0, Vec4f32 p0_color, Vec3f32 p1, Vec4f32 p1_color, Vec3f32 p2, Vec4f32 p2_color) {
  if (Renderer.triangles_count < Renderer.triangles_max) {
    Triangle_Instance* triangle_instance = &Renderer.triangles_data[Renderer.triangles_count];
    triangle_instance->p0 = renderer_vertex(p0, p0_color, vec2f32(0.0f,0.0f));
    triangle_instance->p1 = renderer_vertex(p1, p1_color, vec2f32(0.0f,0.0f));;
    triangle_instance->p2 = renderer_vertex(p2, p2_color, vec2f32(0.0f,0.0f));;
    Renderer.triangles_count += 1;
  }
}

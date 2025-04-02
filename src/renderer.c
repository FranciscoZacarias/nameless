internal void renderer_init() {
  AssertNoReentry();

  const GLubyte* glversion = glGetString(GL_VERSION);
  printf("Opengl version: %s\n", glversion);

  glEnable(GL_DEPTH_TEST);

  // Opengl Shaders and Program
  OGL_Shader vertex_shader   = ogl_make_shader(StringLiteral(VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
  OGL_Shader fragment_shader = ogl_make_shader(StringLiteral(FRAGMENT_SHADER_PATH), GL_FRAGMENT_SHADER);
  GLuint shaders[] = { vertex_shader, fragment_shader };
  ShaderProgram    = ogl_make_program(shaders, 2);

  MemoryZeroStruct(&Renderer);
  Renderer.arena = arena_init();

  // Triangle setup:
  Assert(RENDERER_MAX_TRIANGLES % 3 == 0);
  Renderer.triangles_max   = RENDERER_MAX_TRIANGLES / 3;
  Renderer.triangles_data  = ArenaPush(Renderer.arena, Triangle_Instance, RENDERER_MAX_TRIANGLES);
  Renderer.triangles_count = 0;

  Vec3f32 base_triangle[] = {
    vec3f32( 0.0f,  0.577f, 0.0f), // Top
    vec3f32(-0.5f, -0.289f, 0.0f), // Bottom-left
    vec3f32( 0.5f, -0.289f, 0.0f), // Bottom-right
  };

  glGenVertexArrays(1, &VAO_Triangle);
  glGenBuffers(1, &VBO_Triangle);
  glGenBuffers(1, &VBO_TriangleInstance);
  glBindVertexArray(VAO_Triangle);
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);
    {
      glBufferData(GL_ARRAY_BUFFER, sizeof(base_triangle), base_triangle, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f32), (void*)0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleInstance);
    {
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, transform.translation));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, transform.rotation));
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, transform.scale));
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Instance), (void*)OffsetOfMember(Triangle_Instance, color));
      glVertexAttribDivisor(1, 1);
      glVertexAttribDivisor(2, 1);
      glVertexAttribDivisor(3, 1);
      glVertexAttribDivisor(4, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glBindVertexArray(0);
}

internal void renderer_begin_frame() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Renderer.triangles_count = 0;
}

internal void renderer_end_frame() {
  glUseProgram(ShaderProgram);
  {
    // Draw triangles
    if (Renderer.triangles_count > 0) {
      glBindVertexArray(VAO_Triangle);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleInstance);
      glBufferData(GL_ARRAY_BUFFER, Renderer.triangles_count * sizeof(Triangle_Instance), Renderer.triangles_data, GL_STREAM_DRAW);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 3, Renderer.triangles_count);
      glBindVertexArray(0);
    }
  }
  glBindVertexArray(0);

  // TODO(fz): We don't want to do immediate mode rendering for this renderer.
  Renderer.triangles_count = 0;

  SwapBuffers(DeviceContextHandle);
}

internal void renderer_push_triangle(Transformf32 transform, Vec4f32 color) {
  if (Renderer.triangles_count < Renderer.triangles_max) {
    Triangle_Instance* triangle_instance = &Renderer.triangles_data[Renderer.triangles_count];
    triangle_instance->transform = transform;
    triangle_instance->color     = color;
    Renderer.triangles_count += 1;
  }
}
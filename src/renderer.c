internal void renderer_init() {
  AssertNoReentry();

  MemoryZeroStruct(&Renderer);
  Renderer.arena = arena_init();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  OGL_Shader fragment_shader = ogl_make_shader(StringLiteral(FRAGMENT_SHADER_PATH), GL_FRAGMENT_SHADER);

  OGL_Shader raw_vertex_shader = ogl_make_shader(StringLiteral(RAW_VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
  OGL_Shader raw_shaders[] = { raw_vertex_shader, fragment_shader };
  RawProgram               = ogl_make_program(raw_shaders, 2);
  ogl_delete_shader(raw_vertex_shader);

  OGL_Shader instanced_vertex_shader = ogl_make_shader(StringLiteral(INSTANCED_VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
  GLuint instanced_shaders[] = { instanced_vertex_shader, fragment_shader };
  InstancedProgram           = ogl_make_program(instanced_shaders, 2);
  ogl_delete_shader(instanced_vertex_shader);
  ogl_delete_shader(fragment_shader);

  // Lines Setup
  {
    Assert(RENDERER_MAX_LINES % 2 == 0);
    Renderer.lines_max   = RENDERER_MAX_LINES;
    Renderer.lines_data  = ArenaPush(Renderer.arena, Line_Instance, RENDERER_MAX_LINES);
    Renderer.lines_count = 0;

    glGenVertexArrays(1, &VAO_Line);
    glGenBuffers(1, &VBO_LineInstance);
    glBindVertexArray(VAO_Line);
    {
      glBindBuffer(GL_ARRAY_BUFFER, VBO_LineInstance);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, start));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, end));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, color));
      glVertexAttribDivisor(0, 1);
      glVertexAttribDivisor(1, 1);
      glVertexAttribDivisor(2, 1);
    }
    glBindVertexArray(0);
  }

  // Triangles Setup
  {
    Assert(RENDERER_MAX_TRIANGLES % 3 == 0);
    Renderer.triangles_max = RENDERER_MAX_TRIANGLES / 3;
    Renderer.triangles_data = ArenaPush(Renderer.arena, Triangle_Instance, RENDERER_MAX_TRIANGLES);
    Renderer.triangles_count = 0;

    Vec3f32 base_triangle_positions[] = {
      vec3f32( 0.0f,  0.577f, 0.0f), // Top
      vec3f32(-0.5f, -0.289f, 0.0f), // Bottom-left
      vec3f32( 0.5f, -0.289f, 0.0f), // Bottom-right
    };
    Vec2f32 base_triangle_texcoords[] = {
      vec2f32(0.5f, 1.0f), // Top (center of texture)
      vec2f32(0.0f, 0.0f), // Bottom-left
      vec2f32(1.0f, 0.0f), // Bottom-right
    };

    glGenVertexArrays(1, &VAO_Triangle);
    glGenBuffers(1, &VBO_Triangle);
    glGenBuffers(1, &VBO_TriangleInstance);
    GLuint VBO_TriangleTexCoords;
    glGenBuffers(1, &VBO_TriangleTexCoords);

    glBindVertexArray(VAO_Triangle);
    {
      // Positions
      glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);
      glBufferData(GL_ARRAY_BUFFER, sizeof(base_triangle_positions), base_triangle_positions, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f32), (void*)0);

      // Texture coordinates
      glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleTexCoords);
      glBufferData(GL_ARRAY_BUFFER, sizeof(base_triangle_texcoords), base_triangle_texcoords, GL_STATIC_DRAW);
      glEnableVertexAttribArray(5); // New attribute for tex coords
      glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f32), (void*)0);

      // Instance data
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

  // Quads Setup
  {
    Renderer.quads_max   = RENDERER_MAX_QUADS;
    Renderer.quads_data  = ArenaPush(Renderer.arena, Quad_Instance, RENDERER_MAX_QUADS);
    Renderer.quads_count = 0;

    // Define a unit quad (centered at origin, 1x1 size)
    Vec3f32 base_quad[] = {
        vec3f32(-0.5f, -0.5f, 0.0f), // Bottom-left
        vec3f32( 0.5f, -0.5f, 0.0f), // Bottom-right
        vec3f32( 0.5f,  0.5f, 0.0f), // Top-right
        vec3f32(-0.5f,  0.5f, 0.0f), // Top-left
    };

    // Define indices for two triangles
    u32 indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    glGenVertexArrays(1, &VAO_Quad);
    glGenBuffers(1, &VBO_Quad);
    glGenBuffers(1, &VBO_QuadInstance);
    GLuint EBO_Quad;
    glGenBuffers(1, &EBO_Quad);

    glBindVertexArray(VAO_Quad);
    {
      // Base quad vertices
      glBindBuffer(GL_ARRAY_BUFFER, VBO_Quad);
      glBufferData(GL_ARRAY_BUFFER, sizeof(base_quad), base_quad, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f32), (void*)0);

      // Element buffer for indices
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Quad);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

      // Instance data (transform and color)
      glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadInstance);
      {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Quad_Instance), (void*)OffsetOfMember(Quad_Instance, transform.translation));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Quad_Instance), (void*)OffsetOfMember(Quad_Instance, transform.rotation));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Quad_Instance), (void*)OffsetOfMember(Quad_Instance, transform.scale));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Quad_Instance), (void*)OffsetOfMember(Quad_Instance, color));
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
      }
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
  }
}

internal void renderer_begin_frame() {
  glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

internal void renderer_end_frame(Mat4f32 view, Mat4f32 projection) {

  // Lines
  glUseProgram(RawProgram);
  {
    renderer_set_uniform_mat4fv(RawProgram, "view", view);
    renderer_set_uniform_mat4fv(RawProgram, "projection", projection);
    if (Renderer.lines_count > 0) {
      glBindVertexArray(VAO_Line);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_LineInstance);
      glBufferData(GL_ARRAY_BUFFER, Renderer.lines_count * sizeof(Line_Instance), Renderer.lines_data, GL_STREAM_DRAW);
      glDrawArraysInstanced(GL_LINES, 0, 2, Renderer.lines_count);
      glBindVertexArray(0);
    }
  }
  glBindVertexArray(0);
  Renderer.lines_count = 0;
  
  // Triangles
  glUseProgram(InstancedProgram);
  {
    renderer_set_uniform_mat4fv(InstancedProgram, "view", view);
    renderer_set_uniform_mat4fv(InstancedProgram, "projection", projection);
    if (Renderer.triangles_count > 0) {
      glBindVertexArray(VAO_Triangle);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_TriangleInstance);
      glBufferData(GL_ARRAY_BUFFER, Renderer.triangles_count * sizeof(Triangle_Instance), Renderer.triangles_data, GL_STREAM_DRAW);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 3, Renderer.triangles_count);
    }
  }
  glBindVertexArray(0);
  Renderer.triangles_count = 0;

  // Quads
  glUseProgram(InstancedProgram);
  {
    renderer_set_uniform_mat4fv(InstancedProgram, "view", view);
    renderer_set_uniform_mat4fv(InstancedProgram, "projection", projection);
    if (Renderer.quads_count > 0) {
      glBindVertexArray(VAO_Quad);
      glBindBuffer(GL_ARRAY_BUFFER, VBO_QuadInstance);
      glBufferData(GL_ARRAY_BUFFER, Renderer.quads_count * sizeof(Quad_Instance), Renderer.quads_data, GL_STREAM_DRAW);
      glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, Renderer.quads_count);
    }
  }
  glBindVertexArray(0);
  Renderer.quads_count = 0;

  SwapBuffers(_DeviceContextHandle);
}

internal void renderer_push_line(Vec3f32 start, Vec3f32 end, Vec4f32 color) {
  if (Renderer.lines_count < Renderer.lines_max) {
    Line_Instance* line   = &Renderer.lines_data[Renderer.lines_count];
    line->start           = start;
    line->end             = end;
    line->color           = color;
    Renderer.lines_count += 1;
  } else {
    printf("Too many Lines! Consider increasing the buffer.\n");
  }
}

internal void renderer_push_triangle(Transformf32 transform, Vec4f32 color) {
  if (Renderer.triangles_count < Renderer.triangles_max) {
    Triangle_Instance* triangle_instance = &Renderer.triangles_data[Renderer.triangles_count];
    triangle_instance->transform         = transform;
    triangle_instance->color             = color;
    Renderer.triangles_count            += 1;
  } else {
    printf("Too many Triangles! Consider increasing the buffer.\n");
  }
}

internal void renderer_push_quad(Transformf32 transform, Vec4f32 color) {
  if (Renderer.quads_count < Renderer.quads_max) {
    Quad_Instance* quad_instance = &Renderer.quads_data[Renderer.quads_count];
    quad_instance->transform = transform;
    quad_instance->color = color;
    Renderer.quads_count += 1;
  } else {
    printf("Too many Quads! Consider increasing the buffer.\n");
  }
}

internal void renderer_push_arrow(Vec3f32 start, Vec3f32 end, Vec4f32 color) {
  renderer_push_line(start, end, color);

  Vec3f32 dir = vec3f32_sub(end, start);
  f32 len = vec3f32_length(dir);
  if (len < 0.0001f) return;

  dir = vec3f32_normalize(dir);

  Vec3f32 up = fabsf(dir.y) < 0.99f ? vec3f32(0,1,0) : vec3f32(1,0,0);

  Vec3f32 right   = vec3f32_normalize(vec3f32_cross(up, dir));
  Vec3f32 forward = vec3f32_cross(dir, right);

  f32 head_length = 0.2f;
  right   = vec3f32_scale(right, head_length);
  forward = vec3f32_scale(forward, head_length);
  Vec3f32 tip  = end;
  Vec3f32 base = vec3f32_sub(tip, vec3f32_scale(dir, head_length));

  // Two arrowhead lines
  renderer_push_line(tip, vec3f32_add(base, right),   color);
  renderer_push_line(tip, vec3f32_sub(base, right),   color);
  renderer_push_line(tip, vec3f32_add(base, forward), color);
  renderer_push_line(tip, vec3f32_sub(base, forward), color);
}

internal void renderer_push_box(Vec3f32 min, Vec3f32 max, Vec4f32 color) {
  Vec3f32 corners[8] = {
    { min.x, min.y, min.z },
    { max.x, min.y, min.z },
    { max.x, max.y, min.z },
    { min.x, max.y, min.z },
    { min.x, min.y, max.z },
    { max.x, min.y, max.z },
    { max.x, max.y, max.z },
    { min.x, max.y, max.z },
  };

  u32 edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // top
    {0, 4}, {1, 5}, {2, 6}, {3, 7}, // verticals
  };

  for (u32 i = 0; i < 12; ++i) {
    renderer_push_line(corners[edges[i][0]], corners[edges[i][1]], color);
  }
}

internal void renderer_push_grid(Vec3f32 center, Vec3f32 normal, f32 spacing, s32 count, Vec4f32 color) {
  if (count <= 0) return;

  Vec3f32 n = vec3f32_normalize(normal);

  Vec3f32 temp = fabsf(n.y) < 0.99f ? vec3f32(0,1,0) : vec3f32(1,0,0);
  Vec3f32 u = vec3f32_normalize(vec3f32_cross(temp, n));
  Vec3f32 v = vec3f32_cross(n, u);

  s32 half = count / 2;

  for (s32 i = -half; i <= half; i++) {
    f32 offset = i * spacing;
    Vec3f32 offsetU = vec3f32_scale(u, offset);
    Vec3f32 offsetV = vec3f32_scale(v, offset);

    Vec3f32 p0 = vec3f32_add(center, vec3f32_add(vec3f32_scale(v, -half * spacing), offsetU));
    Vec3f32 p1 = vec3f32_add(center, vec3f32_add(vec3f32_scale(v,  half * spacing), offsetU));
    renderer_push_line(p0, p1, color);

    Vec3f32 q0 = vec3f32_add(center, vec3f32_add(vec3f32_scale(u, -half * spacing), offsetV));
    Vec3f32 q1 = vec3f32_add(center, vec3f32_add(vec3f32_scale(u,  half * spacing), offsetV));
    renderer_push_line(q0, q1, color);
  }
}

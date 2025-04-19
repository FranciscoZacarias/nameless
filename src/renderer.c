internal void renderer_init() {
  AssertNoReentry();

  MemoryZeroStruct(&Renderer);
  Renderer.arena = arena_init_sized(Gigabytes(1), ARENA_COMMIT_SIZE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Shader programs setup
  {
    OGL_Shader raw_vertex_shader       = ogl_make_shader(StringLiteral(RAW_VERTEX_SHADER_PATH),       GL_VERTEX_SHADER);
    OGL_Shader instanced_vertex_shader = ogl_make_shader(StringLiteral(INSTANCED_VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
    OGL_Shader fragment_shader         = ogl_make_shader(StringLiteral(FRAGMENT_SHADER_PATH),         GL_FRAGMENT_SHADER);

    OGL_Shader raw_shaders[] = { raw_vertex_shader, fragment_shader };
    RawProgram               = ogl_make_program(raw_shaders, 2);

    GLuint instanced_shaders[] = { instanced_vertex_shader, fragment_shader };
    InstancedProgram           = ogl_make_program(instanced_shaders, 2);

    ogl_delete_shader(raw_vertex_shader);
    ogl_delete_shader(instanced_vertex_shader);
    ogl_delete_shader(fragment_shader);
  }

  // Lines Setup
  {
    Renderer.lines_max   = RENDERER_MAX_LINES;
    Renderer.lines_data  = ArenaPush(Renderer.arena, Line_Instance, RENDERER_MAX_LINES);
    Renderer.lines_count = 0;

    glGenVertexArrays(1, &Vao_Line);
    glGenBuffers(1, &Vbo_LineInstance);
    glBindVertexArray(Vao_Line);
    {
      glBindBuffer(GL_ARRAY_BUFFER, Vbo_LineInstance);
      glBufferData(GL_ARRAY_BUFFER, sizeof(Line_Instance) * Renderer.lines_max, NULL, GL_STREAM_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, start));
      glVertexAttribDivisor(0, 1);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, end));
      glVertexAttribDivisor(1, 1);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Line_Instance), (void*)OffsetOfMember(Line_Instance, color));
      glVertexAttribDivisor(2, 1);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // Instanced Primitives Setup
  {
    Renderer.instanced_max   = RENDERER_MAX_INSTANCED_DATA;
    Renderer.instanced_data  = ArenaPush(Renderer.arena, Instanced_Data, RENDERER_MAX_INSTANCED_DATA);
    Renderer.instanced_count = 0;
    Renderer.triangle_count  = 0;
    Renderer.quad_count      = 0;

    // Shared instance buffer
    glGenBuffers(1, &Vbo_InstancedData);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo_InstancedData);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Instanced_Data) * Renderer.instanced_max, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Triangle Primitives
    {
        Vertex unit_triangle[] = {
          { vec3f32( 0.0f,  0.577f, 0.0f), vec2f32(0.5f, 1.0f) }, // Top
          { vec3f32(-0.5f, -0.289f, 0.0f), vec2f32(0.0f, 0.0f) }, // Bottom-left
          { vec3f32( 0.5f, -0.289f, 0.0f), vec2f32(1.0f, 0.0f) }, // Bottom-right
        };

      glGenVertexArrays(1, &Vao_Triangle);
      glGenBuffers(1, &Vbo_Triangle);
      glBindVertexArray(Vao_Triangle);

      // Unit triangle vertices (base_position, location 0)
      glBindBuffer(GL_ARRAY_BUFFER, Vbo_Triangle);
      glBufferData(GL_ARRAY_BUFFER, sizeof(unit_triangle), unit_triangle, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f32), (void*)0);

      glBindBuffer(GL_ARRAY_BUFFER, Vbo_InstancedData);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.translation));
      glVertexAttribDivisor(1, 1);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.rotation));
      glVertexAttribDivisor(2, 1);
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.scale));
      glVertexAttribDivisor(3, 1);
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, color));
      glVertexAttribDivisor(4, 1);
      glEnableVertexAttribArray(5);
      glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, texture_id));
      glVertexAttribDivisor(5, 1);

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Quad Primitives
    {
      Vertex unit_quad[] = {
        { vec3f32(-0.5f, -0.5f, 0.0f), vec2f32(0.0f, 0.0f) }, // Bottom-left
        { vec3f32( 0.5f, -0.5f, 0.0f), vec2f32(1.0f, 0.0f) }, // Bottom-right
        { vec3f32( 0.5f,  0.5f, 0.0f), vec2f32(1.0f, 1.0f) }, // Top-right
        { vec3f32(-0.5f,  0.5f, 0.0f), vec2f32(0.0f, 1.0f) }, // Top-left
      };
      u32 unit_quad_indices[] = { 0, 1, 2, 0, 2, 3 };

      glGenVertexArrays(1, &Vao_Quad);
      glGenBuffers(1, &Vbo_Quad);
      glGenBuffers(1, &Ebo_Quad);
      glBindVertexArray(Vao_Quad);

      // Unit quad vertices (base_position, location 0)
      glBindBuffer(GL_ARRAY_BUFFER, Vbo_Quad);
      glBufferData(GL_ARRAY_BUFFER, sizeof(unit_quad), unit_quad, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OffsetOfMember(Vertex, position));
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OffsetOfMember(Vertex, uv));

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo_Quad);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unit_quad_indices), unit_quad_indices, GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, Vbo_InstancedData);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.translation));
      glVertexAttribDivisor(1, 1);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.rotation));
      glVertexAttribDivisor(2, 1);
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.scale));
      glVertexAttribDivisor(3, 1);
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, color));
      glVertexAttribDivisor(4, 1);
      glEnableVertexAttribArray(5);
      glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, texture_id));
      glVertexAttribDivisor(5, 1);

      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Texture setup
    {
      s32 max_gpu_textures;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_gpu_textures);
      Renderer.texture_max   = max_gpu_textures;
      Renderer.textures      = ArenaPush(Renderer.arena, GLuint, Renderer.texture_max);
      Renderer.texture_count = 0;
    }
  }

  print_arena(Renderer.arena, "Arena");
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
    glBindVertexArray(Vao_Line);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo_LineInstance);
    glBufferData(GL_ARRAY_BUFFER, Renderer.lines_count * sizeof(Line_Instance), Renderer.lines_data, GL_STREAM_DRAW);
    glDrawArraysInstanced(GL_LINES, 0, 2, Renderer.lines_count);
    glBindVertexArray(0);
  }

  // Instanced Data
  glUseProgram(InstancedProgram);
  {
    local_persist GLint texture_units[32] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    renderer_set_uniform_mat4fv(InstancedProgram, "view", view);
    renderer_set_uniform_mat4fv(InstancedProgram, "projection", projection);
    for (u32 i = 0; i < Renderer.texture_count; i += 1) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, Renderer.textures[i]);
    }
    glUniform1iv(glGetUniformLocation(InstancedProgram, "textures"), Renderer.texture_count, texture_units);
    renderer_set_uniform_u32(InstancedProgram, "texture_count", Renderer.texture_count);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo_InstancedData);
    glBufferData(GL_ARRAY_BUFFER, Renderer.instanced_count * sizeof(Instanced_Data), Renderer.instanced_data, GL_STREAM_DRAW);
    if (Renderer.triangle_count > 0) {
      glBindVertexArray(Vao_Triangle);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 3, Renderer.triangle_count);
    }
    if (Renderer.quad_count > 0) {
      glBindVertexArray(Vao_Quad);
      glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, Renderer.quad_count);
    }
    glBindVertexArray(0);
  }

  // Reset counters
  Renderer.lines_count     = 0;
  Renderer.instanced_count = 0;
  Renderer.triangle_count  = 0;
  Renderer.quad_count      = 0;

  SwapBuffers(_DeviceContextHandle);
}

u32 renderer_load_texture(String path) {
  u32 result = 0;
  if (Renderer.texture_count >= Renderer.texture_max) {
    printf("Texture limit reached\n");
    return result;
  }

  s32 width, height, channels;
  u8* data = stbi_load(path.str, &width, &height, &channels, 4);
  if (!data) {
    printf("Failed to load texture: %s\n", path.str);
    return result;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  stbi_image_free(data);
  result = Renderer.texture_count;
  Renderer.textures[result] = texture;
  Renderer.texture_count++;
  
  return result + 1;
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
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform      = transform;
    data->color          = color;
    data->texture_id     = 0;
    Renderer.triangle_count  += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_triangle_texture(Transformf32 transform, u32 texture_id) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform      = transform;
    data->color          = Color_White;
    data->texture_id     = texture_id;
    Renderer.triangle_count  += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_quad(Transformf32 transform, Vec4f32 color) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform      = transform;
    data->color          = color;
    data->texture_id     = 0;
    Renderer.quad_count      += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_quad_texture(Transformf32 transform, u32 texture_id) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform      = transform;
    data->color          = Color_White;
    data->texture_id     = texture_id;
    Renderer.quad_count      += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
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
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
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

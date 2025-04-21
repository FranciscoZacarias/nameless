internal void renderer_init() {
  AssertNoReentry();

  MemoryZeroStruct(&Renderer);
  Renderer.arena = arena_init_sized(Gigabytes(1), ARENA_COMMIT_SIZE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Shader programs setup
  {
    OGL_Shader raw_vertex_shader       = opengl_make_shader(Str8(RAW_VERTEX_SHADER_PATH),       GL_VERTEX_SHADER);
    OGL_Shader instanced_vertex_shader = opengl_make_shader(Str8(INSTANCED_VERTEX_SHADER_PATH), GL_VERTEX_SHADER);
    OGL_Shader fragment_shader         = opengl_make_shader(Str8(FRAGMENT_SHADER_PATH),         GL_FRAGMENT_SHADER);

    OGL_Shader raw_shaders[] = { raw_vertex_shader, fragment_shader };
    RawProgram               = opengl_make_program(raw_shaders, 2);

    GLuint instanced_shaders[] = { instanced_vertex_shader, fragment_shader };
    InstancedProgram           = opengl_make_program(instanced_shaders, 2);

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
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OffsetOfMember(Vertex, uv));

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo_Quad);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unit_quad_indices), unit_quad_indices, GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, Vbo_InstancedData);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.translation));
      glVertexAttribDivisor(2, 1);
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.rotation));
      glVertexAttribDivisor(3, 1);
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, transform.scale));
      glVertexAttribDivisor(4, 1);
      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, color));
      glVertexAttribDivisor(5, 1);
      glEnableVertexAttribArray(6);
      glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, texture_id));
      glVertexAttribDivisor(6, 1);
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, uv_min));
      glVertexAttribDivisor(7, 1);
      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, uv_max));
      glVertexAttribDivisor(8, 1);
      glEnableVertexAttribArray(9);
      glVertexAttribIPointer(9, 1, GL_INT, sizeof(Instanced_Data), (void*)OffsetOfMember(Instanced_Data, is_screen_space));
      glVertexAttribDivisor(9, 1);

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

  // Default renderer font 
  {
    f32 font_height = 32.0f;
    u32 font_texture_id = renderer_load_font(Str8(FONT_INCONSOLATA), font_height);
    if (font_texture_id == 0) {
      printf("Failed to initialize font rendering\n");
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
    opengl_set_uniform_mat4fv(RawProgram, "view", view);
    opengl_set_uniform_mat4fv(RawProgram, "projection", projection);
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
    opengl_set_uniform_mat4fv(InstancedProgram, "view", view);
    opengl_set_uniform_mat4fv(InstancedProgram, "projection", projection);
    glUniform1f(glGetUniformLocation(InstancedProgram, "screen_width"), (f32)WindowWidth);
    glUniform1f(glGetUniformLocation(InstancedProgram, "screen_height"), (f32)WindowHeight);
    for (u32 i = 0; i < Renderer.texture_count; i += 1) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, Renderer.textures[i]);
    }
    glUniform1iv(glGetUniformLocation(InstancedProgram, "textures"), Renderer.texture_count, texture_units);
    opengl_set_uniform_u32(InstancedProgram, "texture_count", Renderer.texture_count);
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

internal u32 renderer_load_texture(String8 path) {
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

internal u32 renderer_load_font(String8 path, f32 font_height) {
  Arena_Temp scratch = scratch_begin(0, 0);
  u32 result = 0;

  if (Renderer.texture_count >= Renderer.texture_max) return 0;

  File_Data file_data = file_load(scratch.arena, path);
  if (!file_data.data.str || file_data.data.size == 0) return 0;

  stbtt_fontinfo font_info;
  if (!stbtt_InitFont(&font_info, (u8*)file_data.data.str, 0)) return 0;

  s32 atlas_width = 512, atlas_height = 512;
  u8* atlas_bitmap = ArenaPush(scratch.arena, u8, atlas_width * atlas_height);

  stbtt_packedchar char_data[95];
  stbtt_pack_context pack;
  stbtt_PackBegin(&pack, atlas_bitmap, atlas_width, atlas_height, atlas_width, 1, NULL);
  stbtt_PackSetOversampling(&pack, 1, 1);
  stbtt_PackFontRange(&pack, (u8*)file_data.data.str, 0, font_height, 32, 95, char_data);
  stbtt_PackEnd(&pack);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, atlas_bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  u32 texture_index = Renderer.texture_count++;
  Renderer.textures[texture_index] = texture;
  result = texture_index + 1;

  f32 max_height = 0.0f;
  f32 min_y = 0.0f;

  for (s32 i = 0; i < 95; i++) {
    Glyph* glyph = &Renderer.font.glyphs[i];
    stbtt_packedchar* ch = &char_data[i];

    glyph->uv_min  = vec2f32((f32)ch->x0 / atlas_width, (f32)ch->y0 / atlas_height);
    glyph->uv_max  = vec2f32((f32)ch->x1 / atlas_width, (f32)ch->y1 / atlas_height);
    glyph->size    = vec2f32(ch->x1 - ch->x0, ch->y1 - ch->y0);
    glyph->offset  = vec2f32(ch->xoff, ch->yoff);
    glyph->advance = ch->xadvance;

    f32 y_top    = ch->yoff;
    f32 y_bottom = ch->yoff + (ch->y1 - ch->y0);

    if (y_top < min_y) min_y = y_top;
    if (y_bottom > max_height) max_height = y_bottom;
  }

  scratch_end(&scratch);

  Renderer.font.line_height = max_height - min_y;
  Renderer.font.height      = font_height;
  Renderer.font.texture_id  = result;

  return result;
}

internal f32 renderer_push_textf_screenspace(Vec2f32 screen_position, Vec4f32 color, f32 scale, const char8* text, ...) {
  Arena_Temp scratch = scratch_begin(0, 0);
  f32 result = 0;

  va_list args;
  va_start(args, text);
  s32 required_size = vsnprintf(NULL, 0, (const char8*)text, args) + 1;
  va_end(args);
  
  char8* buffer = ArenaPush(scratch.arena, char8, required_size);
  if (buffer) {
    va_start(args, text);
    vsnprintf((char8*)buffer, required_size, (const char8*)text, args);
    va_end(args);
  
    String8 formatted_text = { .str = buffer, .size = required_size - 1};
    result = renderer_push_text_screenspace(screen_position, color, scale, formatted_text);
  }

  scratch_end(&scratch);
  return result;
}

internal f32 renderer_push_text_screenspace(Vec2f32 screen_position, Vec4f32 color, f32 scale, String8 text) {
  f32 x_start  = screen_position.x;
  f32 y_cursor = screen_position.y;

  Font* font      = &Renderer.font;
  f32 line_height = font->line_height * scale;
  f32 max_y       = y_cursor;

  for (u64 i = 0; i < text.size; ++i) {
    char8 c = text.str[i];

    if (c == '\n') {
      y_cursor         += line_height;
      screen_position.x = x_start;
      continue;
    }

    if (c < 32 || c > 126) continue;

    Glyph* glyph = &font->glyphs[c - 32];
    Vec2f32 pos  = vec2f32(screen_position.x + glyph->offset.x * scale, y_cursor + glyph->offset.y * scale);
    Vec2f32 size = vec2f32(glyph->size.x * scale, glyph->size.y * scale);

    if (Renderer.instanced_count >= Renderer.instanced_max) {
      printf("Instanced data buffer overflow at char %llu\n", i);
      break;
    }

    Instanced_Data* data        = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform.translation = vec3f32(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, 0.0f);
    data->transform.rotation    = quatf32_identity();
    data->transform.scale       = vec3f32(size.x, size.y, 1.0f);
    data->color                 = color;
    data->texture_id            = font->texture_id;
    data->uv_min                = glyph->uv_min;
    data->uv_max                = glyph->uv_max;
    data->is_screen_space       = true;

    Renderer.quad_count        += 1;
    Renderer.instanced_count   += 1;

    screen_position.x += glyph->advance * scale;
    max_y = Max(max_y, y_cursor + line_height);
  }

  return max_y - screen_position.y;
}

internal f32 renderer_push_text_worldspace(Vec3f32 position, Vec4f32 color, f32 scale, String8 text) {
  f32 x_offset = 0.0f;
  f32 y_cursor = 0.0f;

  Font* font      = &Renderer.font;
  f32 line_height = font->line_height * scale;
  f32 max_y       = y_cursor;

  for (u64 i = 0; i < text.size; ++i) {
    char8 c = text.str[i];

    if (c == '\n') {
      y_cursor -= line_height;
      x_offset = 0.0f;
      continue;
    }

    if (c < 32 || c > 126) continue;

    Glyph* glyph      = &font->glyphs[c - 32];
    Vec3f32 glyph_pos = vec3f32(position.x + x_offset + glyph->offset.x * scale, 
                                position.y + y_cursor + glyph->offset.y * -scale, 
                                position.z);
    Vec2f32 size = vec2f32(glyph->size.x * scale, glyph->size.y * -scale);

    if (Renderer.instanced_count >= Renderer.instanced_max) {
      printf("Instanced data buffer overflow at char %llu\n", i);
      break;
    }

    Instanced_Data* data = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform.translation = vec3f32(glyph_pos.x + size.x * 0.5f, 
                                          glyph_pos.y + size.y * 0.5f, 
                                          glyph_pos.z);

    data->transform.rotation    = quatf32_identity();
    data->transform.scale       = vec3f32(size.x, size.y, 1.0f);
    data->color                 = color;
    data->texture_id            = font->texture_id;
    data->uv_min                = glyph->uv_min;
    data->uv_max                = glyph->uv_max;
    data->is_screen_space       = false;

    Renderer.quad_count += 1;
    Renderer.instanced_count += 1;

    x_offset += glyph->advance * scale;
    max_y = Max(max_y, y_cursor + line_height);
  }

  return max_y;
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
    Instanced_Data* data  = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform       = transform;
    data->color           = color;
    data->texture_id      = 0;
    data->is_screen_space = false;
    Renderer.triangle_count  += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_triangle_texture(Transformf32 transform, u32 texture_id) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data  = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform       = transform;
    data->color           = Color_White;
    data->texture_id      = texture_id;
    data->is_screen_space = false;
    Renderer.triangle_count  += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_quad(Transformf32 transform, Vec4f32 color) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data  = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform       = transform;
    data->color           = color;
    data->texture_id      = 0;
    data->is_screen_space = false;
    Renderer.quad_count      += 1;
    Renderer.instanced_count += 1;
  } else {
    printf("Maximum instanced data reached.\n");
  }
}

internal void renderer_push_quad_texture(Transformf32 transform, u32 texture_id) {
  if (Renderer.instanced_count < Renderer.instanced_max) {
    Instanced_Data* data  = &Renderer.instanced_data[Renderer.instanced_count];
    data->transform       = transform;
    data->color           = Color_White;
    data->texture_id      = texture_id;
    data->is_screen_space = false;
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

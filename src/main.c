#include "main.h"

global Vec2f32 Translations[100];

void application_init() {
  win32_init();
  thread_context_init_and_attach(&main_thread_context);

  Arena_Temp scratch = scratch_begin(0, 0);

  renderer_init(scratch.arena);

  s32 index = 0;
  f32 offset = 0.1f;
  for (s32 y = -10; y < 10; y += 2) {
    for (s32 x = -10; x < 10; x += 2) {
      Vec2f32 translation;
      translation.x = (f32)x / 10.0f + offset;
      translation.y = (f32)y / 10.0f + offset;
      Translations[index++] = translation;
    }
  }

  glGenBuffers(1, &Instanced_Vbo);
  glBindBuffer(GL_ARRAY_BUFFER, Instanced_Vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2f32) * 100, Translations, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  f32 quadVertices[] = {
    // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f
  };

  glGenVertexArrays(1, &Vao);
  glGenBuffers(1, &Vbo);
  glBindVertexArray(Vao);
  glBindBuffer(GL_ARRAY_BUFFER, Vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(2 * sizeof(f32)));

  // also set instance data
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, Instanced_Vbo); // this attribute comes from a different vertex buffer
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

  scratch_end(&scratch);
}

void application_tick() {
  renderer_begin_frame();


  renderer_end_frame();
  SwapBuffers(DeviceContextHandle);
}
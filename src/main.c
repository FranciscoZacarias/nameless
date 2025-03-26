#include "main.h"

/*
  TODO:
  
  HIGH PRIORITY:
  1. Set the renderer so that it's not immediate mode rendering.
  2. Add Points, Lines and Quads as rendering primitives

  ELSE:
  - Add Texture rendering
  - Add Text rendering
*/

void application_init() {
  win32_init();
  thread_context_init_and_attach(&main_thread_context);
  renderer_init();
}

void application_tick() {
  renderer_begin_frame();

  renderer_push_triangle(
    vec3f32(-0.5f, -0.5f, 0.0f), Color_Red,
    vec3f32( 0.5f, -0.5f, 0.0f), Color_Green,
    vec3f32( 0.0f,  0.5f, 0.0f), Color_Blue
  );

  renderer_end_frame();
  SwapBuffers(DeviceContextHandle);
}
#define F_ATTACH_CONSOLE 1
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
  camera_init();
}

void application_tick() {
  renderer_begin_frame();
  camera_update(DeltaTime);

  f32 angle = sin(ElapsedTime);
  Vec3f32 axis = vec3f32(0.0f, 0.0f, 1.0f);
  Quatf32 rotation = quaternion_from_axis_angle(axis, angle);

  f32 offset = 0.5f;
  for(f32 z = 0; z < 1; z += 0.1f) {
    for(f32 y = 0; y < 1; y += 0.1f) {
      for(f32 x = 0; x < 1; x += 0.1f) {
        Transformf32 t = transformf32(vec3f32(x - offset, y - offset, z - offset), rotation, vec3f32(0.1f, 0.1f, 0.1f));
        renderer_push_triangle(t, vec4f32(x, y, z, 1.0f));
      }
    }
  }

  Mat4f32 view = camera_get_view_matrix();
  Mat4f32 projection = matrix4_perspective(GlobalCamera.fov, (f32)WINDOW_WIDTH, (f32)WINDOW_HEIGHT, 0.1f, 100.0f);

  renderer_end_frame(view, projection);
}


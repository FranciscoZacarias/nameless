#include "main.h"

/*
  TODO:
  
  HIGH PRIORITY:
  1. Set the renderer so that it's not immediate mode rendering.
  2. Add Points and Quads as rendering primitives

  ELSE:
  - Add Texture rendering
  - Add Text rendering
*/

void application_init() {
  renderer_init();
  camera_init(&GlobalCamera);
}

void application_tick() {
  input_update();
  camera_update(&GlobalCamera, DeltaTime);

  renderer_begin_frame();

  renderer_push_line(vec3f32(-100.0f, 0.0f, 0.0f), vec3f32(100.0f, 0.0f, 0.0f), Color_Red);
  renderer_push_line(vec3f32(0.0f, -100.0f, 0.0f), vec3f32(0.0f, 100.0f, 0.0f), Color_Green);
  renderer_push_line(vec3f32(0.0f, 0.0f, -100.0f), vec3f32(0.0f, 0.0f, 100.0f), Color_Blue);

  f32 angle = sin(ElapsedTime);
  Vec3f32 axis = vec3f32(0.0f, 0.0f, 1.0f);
  Quatf32 rotation = quatf32_from_axis_angle(axis, angle);

  for(f32 z = -1; z < 1; z += 0.2f) {
    for(f32 y = -1; y < 1; y += 0.2f) {
      for(f32 x = -1; x < 1; x += 0.2f) {
        Transformf32 t = transformf32(vec3f32(x, y, z), rotation, vec3f32(0.1f, 0.1f, 0.1f));
        renderer_push_triangle(t, vec4f32(x, y, z, 1.0f));
      }
    }
  }

  Mat4f32 view = camera_get_view_matrix(&GlobalCamera);
  Mat4f32 projection = mat4f32_perspective(GlobalCamera.fov, (f32)FZ_WINDOW_WIDTH, (f32)FZ_WINDOW_HEIGHT, 0.1f, 100.0f);

  renderer_end_frame(view, projection);
}

internal void input_update() {
  if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
    application_stop();
  }
}
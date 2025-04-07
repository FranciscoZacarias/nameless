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

  renderer_push_grid(vec3f32(0.0f, 0.0f, 0.0f), vec3f32(0.0f, 1.0f, 0.0f), 1.0f, 64, Color_Gray);

  renderer_push_arrow(vec3f32(-6.0f, 0.0f, 0.0f), vec3f32(6.0f, 0.0f, 0.0f), Color_Red);
  renderer_push_arrow(vec3f32(0.0f, -6.0f, 0.0f), vec3f32(0.0f, 6.0f, 0.0f), Color_Green);
  renderer_push_arrow(vec3f32(0.0f, 0.0f, -6.0f), vec3f32(0.0f, 0.0f, 6.0f), Color_Blue);

  renderer_push_box(vec3f32(-1.1f, -0.1f, -1.1f), vec3f32(1.1f, 1.9f, 1.1f), Color_Yellow);

  Quatf32 x_rot = quatf32_from_axis_angle(AXIS_X, ElapsedTime);
  Quatf32 y_rot = quatf32_from_axis_angle(AXIS_Y, ElapsedTime);
  Quatf32 rotation = quatf32_multiply(x_rot, y_rot);

  for(f32 z = -1; z < 1; z += 0.2f) {
    for(f32 y = 0; y < 2; y += 0.2f) {
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
#include "main.h"

/*
  TODO:
  
  HIGH PRIORITY:
  1. Set the renderer so that it's not immediate mode rendering.
  2. Triangle primitive should not take a transform.

  ELSE:
  - Add Texture rendering
  - Add Text rendering
*/

void application_init() {
  renderer_init();
  camera_init(&GlobalCamera);
  GlobalCamera.position = vec3f32(0.0f, 5.0f, 10.0f);
  camera_look_at(&GlobalCamera, vec3f32(0.0f, 0.0f, 0.0f));
}

void application_tick() {
  input_update();
  camera_update(&GlobalCamera, DeltaTime);

  renderer_begin_frame();

  renderer_push_grid(vec3f32(0.0f, 0.0f, 0.0f), vec3f32(0.0f, 1.0f, 0.0f), 1.0f, 64, Color_Gray);

  renderer_push_arrow(vec3f32(-6.0f, 0.0f, 0.0f), vec3f32(6.0f, 0.0f, 0.0f), Color_Red);
  renderer_push_arrow(vec3f32(0.0f, -6.0f, 0.0f), vec3f32(0.0f, 6.0f, 0.0f), Color_Green);
  renderer_push_arrow(vec3f32(0.0f, 0.0f, -6.0f), vec3f32(0.0f, 0.0f, 6.0f), Color_Blue);

  renderer_push_box(vec3f32(-0.1f, 0.0f, -0.1f), vec3f32(6.1f, 6.1f, 6.1f), Color_Yellow);

  Mat4f32 view = camera_get_view_matrix(&GlobalCamera);
  Mat4f32 projection = mat4f32_perspective(GlobalCamera.fov, (f32)FZ_WINDOW_WIDTH, (f32)FZ_WINDOW_HEIGHT, 0.1f, 100.0f);

  renderer_end_frame(view, projection);
}

internal void input_update() {
  if (input_is_key_pressed(KeyboardKey_ESCAPE)) {
    application_stop();
  }
  if (input_is_key_pressed(KeyboardKey_K)) {
    print_arena(Renderer.arena, "Renderer Arena");
  }
}
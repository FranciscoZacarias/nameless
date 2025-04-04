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
  
  - Fix right click camera bug
  - Hide mouse when moving camera
*/

void application_init() {
  win32_init();
  thread_context_init_and_attach(&main_thread_context);
  renderer_init();
  camera_init(&GlobalCamera);
}

void application_tick() {
  renderer_begin_frame();
  camera_update(&GlobalCamera, DeltaTime);

  f32 angle = sin(ElapsedTime);
  Vec3f32 axis = vec3f32(0.0f, 0.0f, 1.0f);
  Quatf32 rotation = quaternion_from_axis_angle(axis, angle);
  Vec3f32 axis2 = vec3f32(1.0f, 0.0f, 0.0f);
  Quatf32 rotation2 = quaternion_from_axis_angle(axis2, angle);

  f32 offset = 0.1f;
  for(f32 y = -1; y < 1; y += 0.2f) {
    for(f32 x = -1; x < 1; x += 0.2f) {
      Transformf32 t = transformf32(vec3f32(x + offset, y + offset, 0.0f), rotation, vec3f32(0.1f, 0.1f, 0.1f));
      renderer_push_triangle(t, vec4f32(1.0f, 0.0f, 0.0f, 1.0f));
    }
  }
    
  Mat4f32 view = camera_get_view_matrix(&GlobalCamera);
  Mat4f32 projection = matrix4_perspective(Radians(45), (f64)WINDOW_WIDTH, (f64)WINDOW_HEIGHT, 0.1f, 100.0f);

  // Print to debug
  //printf("View\n");
  //print_mat4f32(&view);
  //printf("Projection\n");
  //print_mat4f32(&projection);
  
  renderer_end_frame(view, projection);
}
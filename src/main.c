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

u32 Tired  = 0;
u32 Angry  = 0;
u32 Cool   = 0;
u32 Scared = 0;

u32 font_inconsolata = 0;

void application_init() {
  renderer_init();
  camera_init(&GlobalCamera);
  GlobalCamera.position = vec3f32(0.0f, 5.0f, 10.0f);
  camera_look_at(&GlobalCamera, vec3f32(0.0f, 0.0f, 0.0f));

  Tired = renderer_load_texture(Str8("D:/work/nameless/assets/tired.png"));
  printf("Tired: %d\n", Tired);

  Angry = renderer_load_texture(Str8("D:/work/nameless/assets/angry.png"));
  printf("Angry: %d\n", Angry);

  Cool = renderer_load_texture(Str8("D:/work/nameless/assets/cool.png"));
  printf("Cool: %d\n", Cool);

  Scared = renderer_load_texture(Str8("D:/work/nameless/assets/scared.png"));
  printf("Scared: %d\n", Scared);
}

void application_tick() {
  input_update();
  camera_update(&GlobalCamera, DeltaTime);

  renderer_begin_frame();

  renderer_push_grid(vec3f32(0.0f, 0.0f, 0.0f), vec3f32(0.0f, 1.0f, 0.0f), 1.0f, 64, Color_Gray);
  renderer_push_arrow(vec3f32(-6.0f, 0.0f, 0.0f), vec3f32(6.0f, 0.0f, 0.0f), Color_Red);
  renderer_push_arrow(vec3f32(0.0f, -6.0f, 0.0f), vec3f32(0.0f, 6.0f, 0.0f), Color_Green);
  renderer_push_arrow(vec3f32(0.0f, 0.0f, -6.0f), vec3f32(0.0f, 0.0f, 6.0f), Color_Blue);

  f32 spacing = 1.5f;
  f32 start_x = -2.25f;
  Transformf32 transform = transformf32(vec3f32(0.0f, 0.5f, 0.0f), quatf32(0.0f, 0.0f, 0.0f, 1.0f), vec3f32(1.0f, 1.0f, 1.0f));

  transform.translation.x = start_x;
  renderer_push_quad_texture(transform, Tired);
  transform.translation.x = start_x + spacing;
  renderer_push_quad_texture(transform, Angry);
  transform.translation.x = start_x + 2.0f * spacing;
  renderer_push_quad_texture(transform, Cool);
  transform.translation.x = start_x + 3.0f * spacing;
  renderer_push_quad_texture(transform, Scared);

  transform.translation.y = 2.5f;
  transform.translation.x = start_x;
  renderer_push_quad(transform, Color_Red);
  transform.translation.x = start_x + spacing;
  renderer_push_quad(transform, Color_Green);
  transform.translation.x = start_x + 2.0f * spacing;
  renderer_push_quad(transform, Color_Blue);
  transform.translation.x = start_x + 3.0f * spacing;
  renderer_push_quad(transform, Color_Yellow);

  f32 ws_offset = renderer_push_text_worldspace(vec3f32(0.0f, 4.0f, 3.0f), Color_Black, 0.01f, Str8("We are not your kind!"));
  renderer_push_text_worldspace(vec3f32(0.0f, 4.0f - ws_offset, 3.0f), Color_Purple, 0.01f, Str8("We are not your kind!"));
  renderer_push_textf_screenspace(vec2f32(0.0f, Renderer.font.line_height), Color_Black, 1.0f, "FPS: %.0f", Fps);
  
  float cycle = fmodf(ElapsedTime, 2.0f * 2.0f);
  float t = cycle < 2.0f ? (cycle / 2.0f) : (1.0f - ((cycle - 2.0f) / 2.0f));
  float value = 0.5f * (1.0f - cosf(t * (float)PI));

  printf("Value: %.2f\n", value);
  
  f32 offset = renderer_push_text_screenspace(vec2f32(0.0f, Renderer.font.line_height*2), vec4f32(1.0f, 0.0f, 0.0f, value), 1.0f, Str8("This is a big string\nWith a string break\nAnd another one"));
  renderer_push_text_screenspace(vec2f32(value*256, Renderer.font.line_height*2 + offset), Color_Blue, 1.0f, Str8("And yet another one!"));

  Mat4f32 view       = camera_get_view_matrix(&GlobalCamera);
  Mat4f32 projection = mat4f32_perspective(GlobalCamera.fov, (f32)WindowWidth, (f32)WindowHeight, 0.1f, 100.0f);

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
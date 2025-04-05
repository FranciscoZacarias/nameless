
internal void camera_init() {
  AssertNoReentry();
    
  GlobalCamera.position    = vec3f32(0.0f, 0.0f, 5.0f);
  GlobalCamera.orientation = quatf32_identity();
  GlobalCamera.fov         = 90.0f;
  GlobalCamera.mode        = CameraMode_Select;
  camera_set_euler(0.0f, 0.0f, 0.0f);
}

internal void camera_update(f32 delta_time) {
  local_persist b32 was_right_mouse_button_down = 0;

  if (input_is_button_down(MouseButton_Right)) {
    if (!was_right_mouse_button_down) {
      GlobalInput.mouse_previous.screen_space_x = GlobalInput.mouse_current.screen_space_x;
      GlobalInput.mouse_previous.screen_space_y = GlobalInput.mouse_current.screen_space_y;
      was_right_mouse_button_down = 1;
      win32_lock_cursor(true);
      win32_hide_cursor(true);
    }

    GlobalCamera.mode = CameraMode_Fly;

    f32 camera_speed = (f32)(CAMERA_SPEED * delta_time);

    Vec3f32 forward = camera_get_forward();
    Vec3f32 right   = camera_get_right();
    Vec3f32 up      = camera_get_up();

    if (input_is_key_down(KeyboardKey_W)) GlobalCamera.position = vector3_add(GlobalCamera.position, vector3_scale(forward, camera_speed));
    if (input_is_key_down(KeyboardKey_S)) GlobalCamera.position = vector3_sub(GlobalCamera.position, vector3_scale(forward, camera_speed));
    if (input_is_key_down(KeyboardKey_D)) GlobalCamera.position = vector3_add(GlobalCamera.position, vector3_scale(right, camera_speed));
    if (input_is_key_down(KeyboardKey_A)) GlobalCamera.position = vector3_sub(GlobalCamera.position, vector3_scale(right, camera_speed));
    if (input_is_key_down(KeyboardKey_E)) GlobalCamera.position = vector3_add(GlobalCamera.position, vector3_scale(up, camera_speed));
    if (input_is_key_down(KeyboardKey_Q)) GlobalCamera.position = vector3_sub(GlobalCamera.position, vector3_scale(up, camera_speed));

    // Mouse look
    f32 sensitivity = 0.0015f * CAMERA_SENSITIVITY;
    f32 dx = GlobalInput.mouse_current.delta_x;
    f32 dy = GlobalInput.mouse_current.delta_y;

    f32 yaw   = -dx * sensitivity;
    f32 pitch = -dy * sensitivity;

    Quatf32 yaw_rotation   = quaternion_from_axis_angle((Vec3f32){0.0f, 1.0f, 0.0f}, yaw);
    Vec3f32 camera_right   = camera_get_right();
    Quatf32 pitch_rotation = quaternion_from_axis_angle(camera_right, pitch);

    GlobalCamera.orientation = quaternion_multiply(yaw_rotation, quaternion_multiply(pitch_rotation, GlobalCamera.orientation));
    GlobalCamera.orientation = quaternion_normalize(GlobalCamera.orientation);

    RECT rect;
    GetClientRect(WindowHandle, &rect);
    POINT center = {(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
    ClientToScreen(WindowHandle, &center);
    SetCursorPos(center.x, center.y);

    // Manually set current mouse position to center (so next frame delta is correct)
    GlobalInput.mouse_current.screen_space_x = (f32)((rect.right - rect.left) / 2);
    GlobalInput.mouse_current.screen_space_y = (f32)((rect.bottom - rect.top) / 2);
  } else {
    GlobalCamera.mode = CameraMode_Select;
    was_right_mouse_button_down = 0;
    win32_lock_cursor(false);
    win32_hide_cursor(false);
  }
}

internal Vec3f32 camera_get_forward() {
  Mat4f32 rot     = matrix_from_quaternion(GlobalCamera.orientation);
  Vec3f32 forward = {0.0f, 0.0f, -1.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, forward));
}

internal Vec3f32 camera_get_right() {
  Mat4f32 rot   = matrix_from_quaternion(GlobalCamera.orientation);
  Vec3f32 right = {1.0f, 0.0f, 0.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, right));
}

internal Vec3f32 camera_get_up() {
  Mat4f32 rot = matrix_from_quaternion(GlobalCamera.orientation);
  Vec3f32 up  = {0.0f, 1.0f, 0.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, up));
}

internal Mat4f32 camera_get_view_matrix() {
  Vec3f32 forward = camera_get_forward();
  Vec3f32 up      = camera_get_up();
  Vec3f32 target  = vector3_add(GlobalCamera.position, forward);
  Mat4f32 result  = matrix4_look_at(GlobalCamera.position, target, up);
  return result;
}

internal void camera_look_at(Vec3f32 target) {
  Vec3f32 direction   = vector3_normalize(vector3_sub(target, GlobalCamera.position));
  Vec3f32 forward     = {0.0f, 0.0f, -1.0f};
  GlobalCamera.orientation = quaternion_from_vector3_to_vector3(forward, direction);
}

internal void camera_set_euler(f32 pitch, f32 yaw, f32 roll) {
  GlobalCamera.orientation = quaternion_from_euler(pitch, yaw, roll);
}
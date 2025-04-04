
internal void camera_init(Camera* camera) {
  AssertNoReentry();
    
  camera->position    = vec3f32(0.0f, 0.0f, 5.0f);  // Closer, above origin
  camera->orientation = quatf32_identity();
  camera->fov         = 45.0f;
  camera->mode        = CameraMode_Select;
  camera_set_euler(camera, 0.0f, 0.0f, 0.0f);  // Look straight at origin
}

internal void camera_update(Camera* camera, f32 delta_time) {
  local_persist b32 was_right_mouse_button_down = 0;
  if (input_is_button_down(MouseButton_Right)) {
    if (!was_right_mouse_button_down) {
      InputState.mouse_previous.screen_space_x = InputState.mouse_current.screen_space_x;
      InputState.mouse_previous.screen_space_y = InputState.mouse_current.screen_space_y;
      was_right_mouse_button_down = 1;
    }
    camera->mode = CameraMode_Fly;

    f32 camera_speed = (f32)(CAMERA_SPEED * delta_time);
    Vec3f32 forward  = camera_get_forward(camera);
    Vec3f32 right    = camera_get_right(camera);
    Vec3f32 up       = camera_get_up(camera);
        
    if (input_is_key_down(KeyboardKey_W)) camera->position = vector3_add(camera->position, vector3_scale(forward, camera_speed));
    if (input_is_key_down(KeyboardKey_S)) camera->position = vector3_sub(camera->position, vector3_scale(forward, camera_speed));
    if (input_is_key_down(KeyboardKey_D)) camera->position = vector3_add(camera->position, vector3_scale(right, camera_speed));
    if (input_is_key_down(KeyboardKey_A)) camera->position = vector3_sub(camera->position, vector3_scale(right, camera_speed));
    if (input_is_key_down(KeyboardKey_E)) camera->position = vector3_add(camera->position, vector3_scale(up, camera_speed));
    if (input_is_key_down(KeyboardKey_Q)) camera->position = vector3_sub(camera->position, vector3_scale(up, camera_speed));

  } else {
    camera->mode = CameraMode_Select;
    was_right_mouse_button_down = 0;
  }
}

internal void camera_mouse_callback(Camera* camera, f64 x_pos, f64 y_pos) {
  if (camera->mode != CameraMode_Fly) return;
    
  local_persist f32 last_x = 0.0f;
  local_persist f32 last_y = 0.0f;
  local_persist b32 first_mouse = true;
    
  if (first_mouse) {
    last_x = (f32)x_pos;
    last_y = (f32)y_pos;
    first_mouse = false;
  }
    
  f32 x_offset = ((f32)x_pos - last_x) * CAMERA_SENSITIVITY;
  f32 y_offset = (last_y - (f32)y_pos) * CAMERA_SENSITIVITY * -1.0f; // TODO(Fz): Extract -1.0f (invert Y) into program args
    
  last_x = (f32)x_pos;
  last_y = (f32)y_pos;
    
  Quatf32 yaw   = quaternion_from_axis_angle(WORLD_UP, -Radians(x_offset));
  Vec3f32 right = camera_get_right(camera);
  Quatf32 pitch = quaternion_from_axis_angle(right, -Radians(y_offset));

  print_quatf32(&yaw, "yaw");
  print_vec3f32(&right, "right");
  print_quatf32(&pitch, "pitch");
    
  camera->orientation = quaternion_multiply(yaw, camera->orientation);
  camera->orientation = quaternion_multiply(pitch, camera->orientation);
  camera->orientation = quaternion_normalize(camera->orientation);

  print_quatf32(&camera->orientation, "orientation");
}

internal Vec3f32 camera_get_forward(Camera* camera) {
  Mat4f32 rot     = matrix_from_quaternion(camera->orientation);
  Vec3f32 forward = {0.0f, 0.0f, -1.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, forward));
}

internal Vec3f32 camera_get_right(Camera* camera) {
  Mat4f32 rot   = matrix_from_quaternion(camera->orientation);
  Vec3f32 right = {1.0f, 0.0f, 0.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, right));
}

internal Vec3f32 camera_get_up(Camera* camera) {
  Mat4f32 rot = matrix_from_quaternion(camera->orientation);
  Vec3f32 up  = {0.0f, 1.0f, 0.0f};
  return vector3_normalize(mat4f32_transform_vec3f32(rot, up));
}

internal Mat4f32 camera_get_view_matrix(Camera* camera) {
  Vec3f32 forward = camera_get_forward(camera);
  Vec3f32 up      = camera_get_up(camera);
  Vec3f32 target  = vector3_add(camera->position, forward);
  Mat4f32 result  = matrix4_look_at(camera->position, target, up);
  return result;
}

internal void camera_look_at(Camera* camera, Vec3f32 target) {
  Vec3f32 direction   = vector3_normalize(vector3_sub(target, camera->position));
  Vec3f32 forward     = {0.0f, 0.0f, -1.0f};
  camera->orientation = quaternion_from_vector3_to_vector3(forward, direction);
}

internal void camera_set_euler(Camera* camera, f32 pitch, f32 yaw, f32 roll) {
  camera->orientation = quaternion_from_euler(pitch, yaw, roll);
}
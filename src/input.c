void input_init() {
  AssertNoReentry();
  MemoryZeroStruct(&GlobalInput);

  // TODO(fz): Change macros with the state of the app.
  GlobalInput.mouse_current.screen_space_x = WINDOW_WIDTH/2;
  GlobalInput.mouse_current.screen_space_y = WINDOW_HEIGHT/2;
  
  GlobalInput.mouse_previous.screen_space_x = WINDOW_WIDTH/2;
  GlobalInput.mouse_previous.screen_space_y = WINDOW_HEIGHT/2;
}

void input_update() {
  MemoryCopy(&GlobalInput.keyboard_previous, &GlobalInput.keyboard_current, sizeof(Keyboard_State));
  MemoryCopy(&GlobalInput.mouse_previous,    &GlobalInput.mouse_current,    sizeof(Mouse_State));
}

//~ Keyboard functions
internal b32 input_is_key_up(Keyboard_Key key) {
  b32 result = GlobalInput.keyboard_current.keys[key] == false;
  return result;
}

internal b32 input_is_key_down(Keyboard_Key key) {
  b32 result = GlobalInput.keyboard_current.keys[key] == true;
  return result;
}

internal b32 input_was_key_up(Keyboard_Key key) {
  b32 result = GlobalInput.keyboard_previous.keys[key] == false;
  return result;
}

internal b32 input_was_key_down(Keyboard_Key key) {
  b32 result = GlobalInput.keyboard_previous.keys[key] == true;
  return result;
}

internal b32 input_is_key_pressed(Keyboard_Key key) {
  return input_is_key_down(key) && input_was_key_up(key);
}

internal void _input_process_keyboard_key(Keyboard_Key key, b8 is_pressed) {
  if (GlobalInput.keyboard_current.keys[key] != is_pressed) {
    GlobalInput.keyboard_current.keys[key] = is_pressed;
  }
}

//~ Mouse functions
internal b32 input_is_button_up(Mouse_Button button) {
  b32 result = GlobalInput.mouse_current.buttons[button] == false;
  return result;
}

internal b32 input_is_button_down(Mouse_Button button) {
  b32 result = GlobalInput.mouse_current.buttons[button] == true;
  return result;
}

internal b32 input_was_button_up(Mouse_Button button) {
  b32 result = GlobalInput.mouse_previous.buttons[button] == false;
  return result;
}

internal b32 input_was_button_down(Mouse_Button button) {
  b32 result = GlobalInput.mouse_previous.buttons[button] == true;
  return result;
}

internal b32 input_is_button_pressed(Mouse_Button button) {
  b32 result = input_is_button_down(button) && input_was_button_up(button);
  return result;
}

internal void _input_process_mouse_button(Mouse_Button button, b32 is_pressed) {
  if (GlobalInput.mouse_current.buttons[button] != (b8)is_pressed) {
    GlobalInput.mouse_current.buttons[button] = (b8)is_pressed;
  }
}

internal void _input_process_mouse_cursor(s32 x, s32 y) {
  // Copy current state to previous
  MemoryCopyStruct(&(GlobalInput.mouse_previous), &(GlobalInput.mouse_current));

  // Compute new deltas
  GlobalInput.mouse_current.delta_x = x - GlobalInput.mouse_previous.screen_space_x;
  GlobalInput.mouse_current.delta_y = y - GlobalInput.mouse_previous.screen_space_y;

  // Update current position
  GlobalInput.mouse_current.screen_space_x = x;
  GlobalInput.mouse_current.screen_space_y = y;
}
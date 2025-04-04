void input_init() {
  AssertNoReentry();
  MemoryZeroStruct(&InputState);

  // TODO(fz): Change macros with the state of the app.
  InputState.mouse_current.screen_space_x = WINDOW_WIDTH/2;
  InputState.mouse_current.screen_space_y = WINDOW_HEIGHT/2;
  
  InputState.mouse_previous.screen_space_x = WINDOW_WIDTH/2;
  InputState.mouse_previous.screen_space_y = WINDOW_HEIGHT/2;
}

void input_update() {
  MemoryCopy(&InputState.keyboard_previous, &InputState.keyboard_current, sizeof(Keyboard_State));
  MemoryCopy(&InputState.mouse_previous,    &InputState.mouse_current,    sizeof(Mouse_State));
}

//~ Keyboard functions
internal b32 input_is_key_up(Keyboard_Key key) {
  b32 result = InputState.keyboard_current.keys[key] == false;
  return result;
}

internal b32 input_is_key_down(Keyboard_Key key) {
  b32 result = InputState.keyboard_current.keys[key] == true;
  return result;
}

internal b32 input_was_key_up(Keyboard_Key key) {
  b32 result = InputState.keyboard_previous.keys[key] == false;
  return result;
}

internal b32 input_was_key_down(Keyboard_Key key) {
  b32 result = InputState.keyboard_previous.keys[key] == true;
  return result;
}

internal b32 input_is_key_pressed(Keyboard_Key key) {
  return input_is_key_down(key) && input_was_key_up(key);
}

internal void _input_process_keyboard_key(Keyboard_Key key, b8 is_pressed) {
  if (InputState.keyboard_current.keys[key] != is_pressed) {
    InputState.keyboard_current.keys[key] = is_pressed;
  }
#if F_ENABLE_INPUT_LOGGING
   if (is_pressed) {
      if (input_was_key_up(key)) {
        printf("Key Pressed: %d\n", key);
      } else {
        printf("Key Down: %d\n", key);
      }
    } else {
      if (input_was_key_down(key)) {
        printf("Key Up: %d\n", key);
      }
    }
#endif
}

//~ Mouse functions
internal b32 input_is_button_up(Mouse_Button button) {
  b32 result = InputState.mouse_current.buttons[button] == false;
  return result;
}

internal b32 input_is_button_down(Mouse_Button button) {
  b32 result = InputState.mouse_current.buttons[button] == true;
  return result;
}

internal b32 input_was_button_up(Mouse_Button button) {
  b32 result = InputState.mouse_previous.buttons[button] == false;
  return result;
}

internal b32 input_was_button_down(Mouse_Button button) {
  b32 result = InputState.mouse_previous.buttons[button] == true;
  return result;
}

internal b32 input_is_button_pressed(Mouse_Button button) {
  b32 result = input_is_button_down(button) && input_was_button_up(button);
  return result;
}

internal void _input_process_mouse_button(Mouse_Button button, b32 is_pressed) {
  if (InputState.mouse_current.buttons[button] != is_pressed) {
    InputState.mouse_current.buttons[button] = is_pressed;
  }
#if F_ENABLE_INPUT_LOGGING
  if (is_pressed) {
      if (input_was_button_up(button)) {
        printf("Mouse Button Pressed: %d\n", button);
      } else {
        printf("Mouse Button Down: %d\n", button);
      }
    } else {
      if (input_was_button_down(button)) {
        printf("Mouse Button Up: %d\n", button);
      }
    }
#endif
}

internal void _input_process_mouse_cursor(s32 x, s32 y) {
  InputState.mouse_current.screen_space_x = (f32)x;
  InputState.mouse_current.screen_space_y = (f32)y;
  camera_mouse_callback(&GlobalCamera, (f64)x, (f64)y);
#if F_ENABLE_INPUT_LOGGING
  printf("Mouse Moved: X=%d, Y=%d\n", x, y);
#endif
}
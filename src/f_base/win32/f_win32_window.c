LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_SETCURSOR:
      if (LOWORD(lParam) == HTCLIENT) {
        win32_set_cursor(CURSOR_ARROW);
        return TRUE;
      } break;

    case WM_SIZE: {
      win32_window_resize_callback(LOWORD(lParam), HIWORD(lParam));
      return 0;
    } break;

    // Keyboard keys
    case WM_KEYDOWN: {
      _input_process_keyboard_key((Keyboard_Key)wParam, TRUE);
      return 0;
    } break;
    case WM_KEYUP: {
      _input_process_keyboard_key((Keyboard_Key)wParam, FALSE);
      return 0;
    } break;

    // Mouse Cursor
    case WM_MOUSEMOVE: {
      if (IgnoreNextMouseMove) {
        IgnoreNextMouseMove = false;
        return 0;
      }
      s32 x = LOWORD(lParam);
      s32 y = HIWORD(lParam);
      _input_process_mouse_cursor((f32)x, (f32)y);
      return 0;
    } break;
    
    // Mouse Buttons
    case WM_LBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Left, TRUE);
      return 0;
    } break;
    case WM_LBUTTONUP: {
      _input_process_mouse_button(MouseButton_Left, FALSE);
      return 0;
    } break;
    case WM_RBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Right, TRUE);
      return 0;
    } break;
    case WM_RBUTTONUP: {
      _input_process_mouse_button(MouseButton_Right, FALSE);
      return 0;
    } break;
    case WM_MBUTTONDOWN: {
      _input_process_mouse_button(MouseButton_Middle, TRUE);
      return 0;
    } break;
    case WM_MBUTTONUP: {
      _input_process_mouse_button(MouseButton_Middle, FALSE);
      return 0;
    } break;

    case WM_DESTROY: {
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(_RenderingContextHandle);
      ReleaseDC(hWnd, _DeviceContextHandle);
      PostQuitMessage(0);
      return 0;
    } break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

// NOTE(fz): App entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
#if F_ENABLE_WINDOW
  _WindowHandle = win32_window_create(hInstance, WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!_WindowHandle) {
    return 1;
  }
#endif 

#if F_ENABLE_OPENGL
  if (!attach_opengl_context()) {
    return 1;
  }
#endif

  // TODO(fz): Should be togglable
#if F_ENABLE_CONSOLE
  attach_console_output();
#endif

  RECT rect;
  GetClientRect(_WindowHandle, &rect);
  win32_window_resize_callback(rect.right - rect.left, rect.bottom - rect.top);

  // Initialize timers
  win32_timer_init();
  win32_timer_start(&_Timer_ElapsedTime);
  win32_timer_start(&_Timer_DeltaTime);
  win32_timer_start(&_Timer_FrameTime);
  
  _input_init();
  application_init();

  MSG msg;
  while (true) {
    // Measure true frame time
    win32_timer_start(&_Timer_FrameTime);

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Measure gameplay delta time
    win32_timer_start(&_Timer_DeltaTime);
    application_tick();

    win32_timer_end(&_Timer_DeltaTime);
    win32_timer_end(&_Timer_FrameTime);
  }

  return (s32)msg.wParam;
}

internal void win32_window_resize_callback(s32 width, s32 height) {
  if (height == 0) { 
    height = 1;
  }
  if (_IsOpenGLContextAttached) {
    glViewport(0, 0, width, height);
  }
}

internal void win32_timer_init() {
  AssertNoReentry();
  QueryPerformanceFrequency(&_PerformanceFrequency);
}

internal void win32_timer_start(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->start);
}

internal void win32_timer_end(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->end);
  LONGLONG difference = timer->end.QuadPart - timer->start.QuadPart;
  timer->elapsed_seconds = (f32)difference / (f32)_PerformanceFrequency.QuadPart;
}


internal HWND win32_window_create(HINSTANCE hInstance, s32 width, s32 height) {
  HWND result = {0};

  WNDCLASS wc      = {0};
  wc.lpfnWndProc   = WndProc;
  wc.hInstance     = hInstance;
  wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
  wc.lpszClassName = "OpenGLWindow";
    
  RegisterClass(&wc);
    
#ifdef F_WINDOW_NAME
  LPCSTR app_name = F_WINDOW_NAME;
#else
  LPCSTR app_name = "f_program";
#endif

  result = CreateWindow("OpenGLWindow", app_name,
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                        NULL, NULL, hInstance, NULL);
    
  return result;
}

internal void win32_set_cursor(CursorType cursor) {
  HCURSOR hCursor = NULL;

  switch (cursor) {
    case CURSOR_ARROW: {
      hCursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case CURSOR_HAND: {
      hCursor = LoadCursor(NULL, IDC_HAND);
      break;
    }
    case CURSOR_CROSSHAIR: {
      hCursor = LoadCursor(NULL, IDC_CROSS);
      break;
    }
    case CURSOR_IBEAM: {
      hCursor = LoadCursor(NULL, IDC_IBEAM);
      break;
    }
    case CURSOR_WAIT: { 
      hCursor = LoadCursor(NULL, IDC_WAIT);
      break;
    }
    case CURSOR_SIZE_ALL: {
      hCursor = LoadCursor(NULL, IDC_SIZEALL);
      break;
    }
    default: {
      hCursor = LoadCursor(NULL, IDC_ARROW);
      break;
    }
  }

  if (hCursor) {
    SetCursor(hCursor);
  }
}

internal void win32_set_cursor_position(s32 x, s32 y) {
  SetCursorPos(x, y);
}

internal void win32_lock_cursor(b32 lock) {
  if (lock) {
    RECT rect;
    GetClientRect(_WindowHandle, &rect);
    POINT center = {(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
    ClientToScreen(_WindowHandle, &center);
    SetCursorPos(center.x, center.y);

    IsCursorLocked      = true;
    IgnoreNextMouseMove = true;

    // Reset deltas to avoid cursor jump
    _InputState.mouse_current.delta_x  = 0.0f;
    _InputState.mouse_current.delta_y  = 0.0f;
    _InputState.mouse_previous.delta_x = 0.0f;
    _InputState.mouse_previous.delta_y = 0.0f;
    MemoryCopyStruct(&_InputState.mouse_previous, &_InputState.mouse_current);
  } else {
    IsCursorLocked = false;
  }
}

internal void win32_hide_cursor(b32 hide) {
  // Win32 quirk. It has an internal counter required to show the cursor.
  // The while loops just make sure it exhausts the counter and applies immediately.
  while (ShowCursor(hide ? FALSE : TRUE) >= 0 &&  hide);
  while (ShowCursor(hide ? FALSE : TRUE) < 0  && !hide);
}

internal b32 attach_opengl_context() {
  b32 result = true;
  _DeviceContextHandle = GetDC(_WindowHandle);
  if (!_DeviceContextHandle) {
    ERROR_MESSAGE_AND_EXIT("Failed to get device context\n");
    return false;
  }

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 16, 0, 0,
    PFD_MAIN_PLANE, 0, 0, 0, 0
  };

  s32 pixelFormat = ChoosePixelFormat(_DeviceContextHandle, &pfd);
  if (!pixelFormat) {
    ERROR_MESSAGE_AND_EXIT("Failed to choose pixel format\n");
    return false;
  }
  if (!SetPixelFormat(_DeviceContextHandle, pixelFormat, &pfd)) {
    ERROR_MESSAGE_AND_EXIT("Failed to set pixel format\n");
    return false;
  }

  HGLRC tempRC = wglCreateContext(_DeviceContextHandle);
  if (!tempRC) {
    ERROR_MESSAGE_AND_EXIT("Failed to create temporary context\n");
    return false;
  }
  if (!wglMakeCurrent(_DeviceContextHandle, tempRC)) {
    ERROR_MESSAGE_AND_EXIT("Failed to make temporary context current\n");
    return false;
  }

  typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

  if (!wglCreateContextAttribsARB) {
    ERROR_MESSAGE_AND_EXIT("wglCreateContextAttribsARB not supported\n");
    wglDeleteContext(tempRC);
    return false;
  }

  s32 attribs[] = {
    0x2091, 4,      // WGL_CONTEXT_MAJOR_VERSION_ARB
    0x2092, 6,      // WGL_CONTEXT_MINOR_VERSION_ARB
    0x2094, 0x9126, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
    0 // End
  };

  _RenderingContextHandle = wglCreateContextAttribsARB(_DeviceContextHandle, NULL, attribs);
  if (!_RenderingContextHandle) {
    ERROR_MESSAGE_AND_EXIT("Failed to create OpenGL 4.6 context\n");
    wglDeleteContext(tempRC);
    return false;
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(tempRC);
  if (!wglMakeCurrent(_DeviceContextHandle, _RenderingContextHandle)) {
    ERROR_MESSAGE_AND_EXIT("Failed to make OpenGL 4.6 context current\n");
    return false;
  }

  if (!gladLoadGL()) {
    ERROR_MESSAGE_AND_EXIT("Failed to initialize GLAD\n");
    return false;
  }

  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  _IsOpenGLContextAttached = true;
  return result;
}

internal void attach_console_output() {
  AllocConsole();
  FILE* fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);
  _IsTerminalAttached = true;
}

internal f32 _get_elapsed_time(void) {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  LONGLONG ticks = current.QuadPart - _Timer_ElapsedTime.start.QuadPart;
  return (f32)ticks / (f32)_PerformanceFrequency.QuadPart;
}
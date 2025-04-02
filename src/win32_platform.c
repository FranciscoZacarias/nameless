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
      _input_process_mouse_cursor(LOWORD(lParam), HIWORD(lParam));
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
      wglDeleteContext(RenderingContextHandle);
      ReleaseDC(hWnd, DeviceContextHandle);
      PostQuitMessage(0);
      return 0;
    } break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

// NOTE(fz): App entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
  WindowHandle = win32_window_create(hInstance, WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!WindowHandle) {
    return 1;
  }

  if (!attach_opengl_context()) {
    return 1;
  }

  // TODO(fz): Should be togglable
#if F_ATTACH_CONSOLE
  attach_console_output();
#endif

  RECT rect;
  GetClientRect(WindowHandle, &rect);
  win32_window_resize_callback(rect.right - rect.left, rect.bottom - rect.top);

  // Initialize performance counter
  win32_timer_init();
  win32_timer_start(&Timer_ElapsedTime);
  win32_timer_start(&Timer_DeltaTime);
  win32_timer_start(&Timer_FrameTime);
  

  application_init();

  MSG msg;
  while (true) {
    // Measure true frame time
    win32_timer_end(&Timer_FrameTime);
    win32_timer_start(&Timer_FrameTime);

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Measure gameplay delta time
    win32_timer_end(&Timer_DeltaTime);
    win32_timer_start(&Timer_DeltaTime);
    application_tick();
  }

  return (s32)msg.wParam;
}

internal void win32_window_resize_callback(s32 width, s32 height) {
  if (height == 0) { 
    height = 1;
  }
  if (IsOpenGLContextAttached) {
    glViewport(0, 0, width, height);
  }
}

internal void win32_timer_init() {
  AssertNoReentry();
  QueryPerformanceFrequency(&GlobalPerfFrequency);
}

internal void win32_timer_start(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->start);
}

internal void win32_timer_end(PerformanceTimer* timer) {
  QueryPerformanceCounter(&timer->end);
  LONGLONG difference = timer->end.QuadPart - timer->start.QuadPart;
  timer->elapsed_seconds = (f32)difference / (f32)GlobalPerfFrequency.QuadPart;
}


internal HWND win32_window_create(HINSTANCE hInstance, s32 width, s32 height) {
  HWND result = {0};

  WNDCLASS wc      = {0};
  wc.lpfnWndProc   = WndProc;
  wc.hInstance     = hInstance;
  wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
  wc.lpszClassName = "OpenGLWindow";
    
  RegisterClass(&wc);
    
  result = CreateWindow("OpenGLWindow", APP_NAME,
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

internal b32 attach_opengl_context() {
  b32 result = true;
  DeviceContextHandle = GetDC(WindowHandle);
  if (!DeviceContextHandle) {
    printf("Failed to get device context\n");
    return false;
  }

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 16, 0, 0,
    PFD_MAIN_PLANE, 0, 0, 0, 0
  };

  s32 pixelFormat = ChoosePixelFormat(DeviceContextHandle, &pfd);
  if (!pixelFormat) {
    printf("Failed to choose pixel format\n");
    return false;
  }
  if (!SetPixelFormat(DeviceContextHandle, pixelFormat, &pfd)) {
    printf("Failed to set pixel format\n");
    return false;
  }

  HGLRC tempRC = wglCreateContext(DeviceContextHandle);
  if (!tempRC) {
    printf("Failed to create temporary context\n");
    return false;
  }
  if (!wglMakeCurrent(DeviceContextHandle, tempRC)) {
    printf("Failed to make temporary context current\n");
    return false;
  }

  typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

  if (!wglCreateContextAttribsARB) {
    printf("wglCreateContextAttribsARB not supported\n");
    wglDeleteContext(tempRC);
    return false;
  }

  s32 attribs[] = {
    0x2091, 4,      // WGL_CONTEXT_MAJOR_VERSION_ARB
    0x2092, 6,      // WGL_CONTEXT_MINOR_VERSION_ARB
    0x2094, 0x9126, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
    0 // End
  };

  RenderingContextHandle = wglCreateContextAttribsARB(DeviceContextHandle, NULL, attribs);
  if (!RenderingContextHandle) {
    printf("Failed to create OpenGL 4.6 context\n");
    wglDeleteContext(tempRC);
    return false;
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(tempRC);
  if (!wglMakeCurrent(DeviceContextHandle, RenderingContextHandle)) {
    printf("Failed to make OpenGL 4.6 context current\n");
    return false;
  }

  if (!gladLoadGL()) {
    printf("Failed to initialize GLAD\n");
    return false;
  }

  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  IsOpenGLContextAttached = true;
  return result;
}

internal void attach_console_output() {
  AllocConsole();
  FILE* fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);
  IsTerminalAttached = true;
}

internal f32 _get_elapsed_time(void) {
  LARGE_INTEGER current;
  QueryPerformanceCounter(&current);
  LONGLONG ticks = current.QuadPart - Timer_ElapsedTime.start.QuadPart;
  return (f32)ticks / (f32)GlobalPerfFrequency.QuadPart;
}
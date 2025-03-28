LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_SIZE:
    win32_window_resize_callback(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_KEYDOWN:
    win32_window_keyboard_callback(wParam);
    return 0;
  case WM_MOUSEMOVE:
    win32_window_mouse_move_callback(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    win32_window_mouse_buttons_callback(wParam, LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_DESTROY:
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(RenderingContextHandle);
    ReleaseDC(hWnd, DeviceContextHandle);
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

// NOTE(fz): App entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
  WindowHandle = win32_window_create(hInstance, 800, 600);
  if (!WindowHandle) {
    return 1;
  }

  if (!attach_opengl_context()) {
    return 1;
  }

  // TODO(fz): Should be togglable
  attach_console_output();

  RECT rect;
  GetClientRect(WindowHandle, &rect);
  win32_window_resize_callback(rect.right - rect.left, rect.bottom - rect.top);

  // Initialize performance counter
  win32_timer_init();
  win32_timer_start(&DeltaTime);
  win32_timer_start(&FrameTime);

  application_init();

  MSG msg;
  while (true) {
    // Measure true frame time
    win32_timer_end(&FrameTime);
    win32_timer_start(&FrameTime);

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // Measure gameplay delta time
    win32_timer_end(&DeltaTime);
    win32_timer_start(&DeltaTime);
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

internal void win32_window_keyboard_callback(WPARAM wParam) {
  // TODO(fz): This should have an application layer callback.
  switch (wParam) {
    case VK_ESCAPE:
      PostQuitMessage(0);
      break;
  }
}

internal void win32_window_mouse_buttons_callback(WPARAM wParam, s32 x, s32 y) {
  if (wParam & MK_LBUTTON) {
  }
  if (wParam & MK_RBUTTON) {
  }
}

internal void win32_window_mouse_move_callback(s32 x, s32 y) {
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
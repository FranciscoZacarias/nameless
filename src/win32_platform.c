LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_SIZE:
    window_resize_callback(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_KEYDOWN:
    window_keyboard_callback(wParam);
    return 0;
  case WM_MOUSEMOVE:
    window_mouse_move_callback(LOWORD(lParam), HIWORD(lParam));
    return 0;
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    window_mouse_buttons_callback(wParam, LOWORD(lParam), HIWORD(lParam));
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
    
  WindowHandle = window_create(hInstance, 800, 600);
  if (!WindowHandle) {
    return 1;
  }
    
  if (!attach_opengl_context()) {
    return 1;
  }
    
  // NOTE(fz): Optional
  attach_console_output();
    
  RECT rect;
  GetClientRect(WindowHandle, &rect);
  window_resize_callback(rect.right - rect.left, rect.bottom - rect.top);

  program_init();

  MSG msg;
  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
        
    program_tick();
  }

  return (s32)msg.wParam;
}

void window_resize_callback(s32 width, s32 height) {
  if (height == 0) { 
    height = 1;
  }
  if (IsOpenGLContextAttached) {
    glViewport(0, 0, width, height);
  }
}

HWND window_create(HINSTANCE hInstance, s32 width, s32 height) {
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

void window_keyboard_callback(WPARAM wParam) {
  switch (wParam) {
    case VK_ESCAPE:
      PostQuitMessage(0);
      break;
  }
}

void window_mouse_buttons_callback(WPARAM wParam, s32 x, s32 y) {
  if (wParam & MK_LBUTTON) {
  }
  if (wParam & MK_RBUTTON) {
  }
}

void window_mouse_move_callback(s32 x, s32 y) {
}

b32 attach_opengl_context() {
  b32 result = true;
  DeviceContextHandle = GetDC(WindowHandle);
    
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 16, 0, 0,
    PFD_MAIN_PLANE, 0, 0, 0, 0
  };
    
  s32 pixelFormat = ChoosePixelFormat(DeviceContextHandle, &pfd);
  SetPixelFormat(DeviceContextHandle, pixelFormat, &pfd);
    
  // Create temporary context to load modern OpenGL
  HGLRC tempRC = wglCreateContext(DeviceContextHandle);
  wglMakeCurrent(DeviceContextHandle, tempRC);

  // Load wgl extensions for modern context creation
  typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 
      (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

  if (wglCreateContextAttribsARB) {
    s32 attribs[] = {
      0x2091, 4,      // WGL_CONTEXT_MAJOR_VERSION_ARB
      0x2092, 6,      // WGL_CONTEXT_MINOR_VERSION_ARB
      0x2094, 0x9126, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
      0 // End
    };

    RenderingContextHandle = wglCreateContextAttribsARB(DeviceContextHandle, NULL, attribs);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
    wglMakeCurrent(DeviceContextHandle, RenderingContextHandle);
  }

  IsOpenGLContextAttached = true;
  return result;
}

void attach_console_output() {
  AllocConsole();
  FILE* fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);
  IsTerminalAttached = true;
}
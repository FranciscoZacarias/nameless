#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

// TODO(fz): I don't like these forward declares here.
extern void application_init();
extern void application_tick();

typedef enum Cursor_Type {
  CURSOR_ARROW,
  CURSOR_HAND,
  CURSOR_CROSSHAIR,
  CURSOR_IBEAM,
  CURSOR_WAIT,
  CURSOR_SIZE_ALL,

  CURSOR_COUNT
} CursorType;

typedef struct PerformanceTimer {
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  f32 elapsed_seconds;
} PerformanceTimer;

global LARGE_INTEGER GlobalPerfFrequency;
global PerformanceTimer Timer_DeltaTime   = {0}; 
global PerformanceTimer Timer_FrameTime   = {0};
global PerformanceTimer Timer_ElapsedTime = {0};

#define DeltaTime   Timer_DeltaTime.elapsed_seconds
#define FrameTime   Timer_FrameTime.elapsed_seconds
#define ElapsedTime _get_elapsed_time()

void win32_timer_init();
void win32_timer_start(PerformanceTimer* timer);
void win32_timer_end(PerformanceTimer* timer);

global HDC   DeviceContextHandle    = NULL;
global HGLRC RenderingContextHandle = NULL;
global HWND  WindowHandle           = NULL;

global b32 IsOpenGLContextAttached = false;
global b32 IsTerminalAttached      = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

internal HWND win32_window_create(HINSTANCE hInstance, s32 width, s32 height);
internal void win32_window_resize_callback(s32 width, s32 height);
internal void win32_window_keyboard_callback(WPARAM wParam);
internal void win32_window_mouse_buttons_callback(WPARAM wParam, s32 x, s32 y);
internal void win32_window_mouse_move_callback(s32 x, s32 y);

internal void win32_set_cursor(CursorType cursor);

internal b32  attach_opengl_context();
internal void attach_console_output();

internal f32 _get_elapsed_time();

#endif // WIN32_PLATFORM_H
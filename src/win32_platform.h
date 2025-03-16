#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H

extern void program_init();
extern void program_tick();

HDC   DeviceContextHandle    = NULL;
HGLRC RenderingContextHandle = NULL;
HWND  WindowHandle           = NULL;

b32   IsOpenGLContextAttached = false;
b32   IsTerminalAttached      = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND window_create(HINSTANCE hInstance, s32 width, s32 height);
void window_resize_callback(s32 width, s32 height);
void window_keyboard_callback(WPARAM wParam);
void window_mouse_buttons_callback(WPARAM wParam, s32 x, s32 y);
void window_mouse_move_callback(s32 x, s32 y);

b32  attach_opengl_context();
void attach_console_output();


#endif // WIN32_PLATFORM_H
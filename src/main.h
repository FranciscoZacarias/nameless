#ifndef MAIN_H
#define MAIN_H

#define F_ENABLE_WINDOW  1
#define F_WINDOW_NAME "nameless"

#define F_ENABLE_ASSERT  1
#define F_ENABLE_CONSOLE 1
#define F_ENABLE_OPENGL  1

#define CAMERA_SENSITIVITY 1.0f
#define CAMERA_SPEED       8.0f

//~ Opengl(Glad)
#include "glad/glad.h"
#include "glad/glad.c"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

//~ F Layer
#define F_ENABLE_OPENGL 1
#include "f_includes.h"

//~ *.h
#include "f_camera.h"
#include "renderer.h"

//~ *.c
#include "f_camera.c"
#include "renderer.c"

global Thread_Context main_thread_context;
global Camera GlobalCamera;

void application_init(); // Run once at start of program
void application_tick(); // Run every tick

internal void input_update();

#endif // MAIN_H
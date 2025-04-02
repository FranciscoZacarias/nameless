#ifndef MAIN_H
#define MAIN_H

#define APP_NAME "nameless"
#define ENABLE_ASSERT 1

//~ Opengl(Glad)
#include "glad/glad.h"
#include "glad/glad.c"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

//~ F Layer
#define F_ENABLE_OPENGL 1
#include "f_includes.h"

//~ *.h
#include "win32_platform.h"
#include "input.h"
#include "renderer.h"

//~ *.c
#include "win32_platform.c"
#include "input.c"
#include "renderer.c"

global Thread_Context main_thread_context;

void application_init(); /* Run once at start of program*/
void application_tick(); /* Run every tick */

#endif // MAIN_H
#ifndef MAIN_H
#define MAIN_H

//~ fz_std
#define FZ_WINDOW_NAME   "nameless"

#define FZ_ENABLE_ASSERT  1
#define FZ_ENABLE_WINDOW  1
#define FZ_ENABLE_CONSOLE 1
#define FZ_ENABLE_OPENGL  1

#define FZ_CAMERA_SENSITIVITY 1.0f
#define FZ_CAMERA_SPEED       8.0f

#include "fz_include.h"

//~ *.h
#include "fz_camera.h"
#include "renderer.h"

//~ *.c
#include "fz_camera.c"
#include "renderer.c"

global Camera GlobalCamera;

void application_init(); // Run once at start of program
void application_tick(); // Run every tick

internal void input_update();

#endif // MAIN_H
#ifndef MAIN_H
#define MAIN_H

#define FZ_ENABLE_WINDOW  1
#define FZ_WINDOW_NAME "nameless"
#define FZ_WINDOW_WIDTH  1280
#define FZ_WINDOW_HEIGHT 720

#define FZ_ENABLE_ASSERT  1
#define FZ_ENABLE_CONSOLE 1
#define FZ_ENABLE_OPENGL  1

#define CAMERA_SENSITIVITY 1.0f
#define CAMERA_SPEED       8.0f

//~ F Layer
#define F_ENABLE_OPENGL 1
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
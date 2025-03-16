#ifndef MAIN_H
#define MAIN_H

#define APP_NAME "nameless"
#define ENABLE_ASSERT 1

//~ Opengl(Glad)
#include "glad/glad.h"
#include "glad/glad.c"

//~ F Layer
#define F_ENABLE_OPENGL 1
#include "f_includes.h"

//~ *.h
#include "win32_platform.h"

//~ *.c
#include "win32_platform.c"

void program_init(); /* Run once at start of program*/
void program_tick(); /* Run every tick */

#endif // MAIN_H
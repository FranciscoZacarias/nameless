#ifndef F_INCLUDES_H
#define F_INCLUDES_H

/*
  Help:

  Enables:
    Create window:  #define F_ENABLE_WINDOW  1
    Enable opengl:  #define F_ENABLE_OPENGL  1
    Attach console: #define F_ENABLE_CONSOLE 1
    Enable assert:  #define F_ENABLE_ASSERT  1
  
  Config:
    #define CAMERA_SENSITIVITY 1.0f
    #define CAMERA_SPEED 8.0f

  Macros that provide Context:
  {
    COMPILER_CLANG
    COMPILER_MSVC
    COMPILER_GCC

    OS_WINDOWS
    OS_LINUX
    OS_MAC

    ARCH_X64
    ARCH_X86
    ARCH_ARM64
    ARCH_ARM32

    COMPILER_MSVC_YEAR

    ARCH_32BIT
    ARCH_64BIT
    ARCH_LITTLE_ENDIAN
  }

*/

//~ CLib
#include <stdlib.h>
#include <stdio.h> // TODO(fz): Move this to a f_stdio, so we can properly abstract into our String API
#include <string.h>
#include <math.h> // TODO(fz): Replace/Implement these functions -> sinf(), cosf(), tan(), atan2f(), sqrtf(), floor(), fminf(), fmaxf(), fabsf()

//~ Extern
#include "external/stb_sprintf.h"

//~ Single headers
#include "f_core.h"

//~ Headers
#include "f_math.h"
#include "f_memory.h"
#include "f_string.h"
#include "f_thread_context.h"
#include "f_input.h"
#include "f_win32.h"
#include "f_win32_window.h"

#if defined(F_ENABLE_OPENGL)
# include "extra/f_opengl_helper.h"
#endif 

//~ Extern
#define STB_SPRINTF_IMPLEMENTATION
#include "external/stb_sprintf.h"

//~ Source
#include "f_math.c"
#include "f_memory.c"
#include "f_string.c"
#include "f_thread_context.c"
#include "f_input.c"
#include "f_win32.c"
#include "f_win32_window.c"

#if defined(F_ENABLE_OPENGL)
# include "extra/f_opengl_helper.c"
#endif

#endif // F_INCLUDES_H
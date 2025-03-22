#include "main.h"

void application_init() {

}

void application_tick() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  SwapBuffers(DeviceContextHandle);
}
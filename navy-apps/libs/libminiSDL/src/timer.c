#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>
SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  return 1;
}
extern uint32_t start;
 uint32_t SDL_GetTicks() {
  uint32_t current = NDL_GetTicks();
  return current - start;
}

void SDL_Delay(uint32_t ms) {
}

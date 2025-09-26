#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};

int SDL_PushEvent(SDL_Event *ev)
{
  return 0;
}

int SDL_PollEvent(SDL_Event *ev)
{
  char buf[64];
  if (NDL_PollEvent(buf, sizeof(buf)))
  {
    char type[8];
    char key[16];
    if (sscanf(buf, "%s %s", type, key) == 2)
    {
      ev->key.type = (strcmp(type, "kd") == 0) ? SDL_KEYDOWN : SDL_KEYUP;
      for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++)
      {
        if (strcmp(keyname[i], key) == 0)
        {
          printf("catch the key : %s\n", keyname[i]);
          ev->key.keysym.sym = i;
          return 1;
        }
      }
    }
  }
  return 0;

}

int SDL_WaitEvent(SDL_Event *event)
{
  static int count = 0;
  memset(event, 0, sizeof(event));
  while (1)
  {
    if (SDL_PollEvent(event))
    {
      printf("call wait %drd time\n", count++);
      return 1;
    }
  }
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask)
{
  return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys)
{
  return NULL;
}

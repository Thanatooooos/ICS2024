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
bool handleEvent(SDL_Event *event, char *buf)
{
  if (NDL_PollEvent(buf, sizeof(buf)))
  {
    char action[2];
    strncpy(action, buf, 2);
    char name[16];
    strcpy(name, buf + 3);
    for (int i = 0; i < strlen(name); i++)
    {
      if (name[i] == ' ' || name[i] == '\n')
      {
        name[i] = '\0';
        break;
      }
    }
    if (strcmp(action, "ku") == 0)
    {
      event->type = SDL_KEYUP;
    }
    else if (strcmp(action, "kd") == 0)
    {
      event->type = SDL_KEYDOWN;
    }
    for (int i = 1; i < sizeof(keyname) / sizeof(keyname[0]); i++)
    {

      if (strcmp(keyname[i], name) == 0)
      {
        event->key.keysym.sym = i;
        break;
      }
    }
    return 1;
  }
  return 0;
}
int SDL_PollEvent(SDL_Event *ev)
{ 
  char buf[64];
  if(handleEvent(ev,buf)){
    return 1;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event)
{
  memset(event, 0, sizeof(event));
  while (1)
  {
    char buf[64];
    if(handleEvent(event,buf)){
      return 1;
    } 
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask)
{
  return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys)
{
  return NULL;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <assert.h>
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks()
{
  struct timeval tv;
  assert(gettimeofday(&tv, NULL) == 0);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len)
{
  FILE *fp = fopen("/dev/events", "r+");
  if(fp == NULL)return 0;
  fseek(fp, 0, SEEK_SET);
  if (fgets(buf, len, fp))
    // fread(buf,sizeof(char), len, fp);
    return strlen(buf);
  else
    return 0;

}

void NDL_OpenCanvas(int *w, int *h)
{
  if (getenv("NWM_APP"))
  {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1)
    {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }
    close(fbctl);
  }
  else
  {
    canvas_h = *h == 0 || canvas_h > screen_h ? screen_h : *h;
    canvas_w = *w == 0 || canvas_w > screen_w ? screen_w : *w;
    *h = canvas_h;
    *w = canvas_w;
    canvas_x = (screen_w - canvas_w) / 2;
    canvas_y = (screen_h - canvas_h) / 2;
  }
}

static void canvas2screen(int cx, int cy, int *sx, int *sy)
{
    *sx = cx;
    *sy = cy;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h)
{
   printf("enter draw\n");
  w = w ? w : canvas_w;
  h = h ? h : canvas_h;
  int fd = open("/dev/fb", 1);
  for (int i = 0; i < h; i++)
  {
    lseek(fd, ((y + canvas_y + i) * screen_w + (x + canvas_x)) * sizeof(uint32_t), SEEK_SET);
    write(fd, pixels + i * w, (w < canvas_w - x ? w : canvas_w - x) * sizeof(uint32_t));
  }
  assert(close(fd) == 0);
  printf("draw done\n");
  return;
}

void NDL_OpenAudio(int freq, int channels, int samples)
{
}

void NDL_CloseAudio()
{
}

int NDL_PlayAudio(void *buf, int len)
{
  return 0;
}

int NDL_QueryAudio()
{
  return 0;
}
uint32_t start = 0;
int NDL_Init(uint32_t flags)
{
  if (getenv("NWM_APP"))
  {
    evtdev = 3;
  }
  FILE *fp = fopen("/proc/dispinfo", "r");
  char buf[64];
  int nread = fread(buf, sizeof(char), sizeof(buf) - 1, fp);
  buf[nread] = '\0';
  int i = 0;
  while (buf[i] != ':')
    ++i;
  screen_w = atoi(buf + i + 1);
  ++i;
  while (buf[i] != ':')
    ++i;
  screen_h = atoi(buf + i + 1);
  start = NDL_GetTicks();
  return 0;
}

void NDL_Quit()
{
}

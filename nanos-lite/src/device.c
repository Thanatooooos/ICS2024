#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

int fs_open(const char *filename, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void* buf, size_t len);
int fs_close(int fd);

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *out = (char *)buf;
  for(int i = 0;i < len; i++){
    putch(out[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if(ev.keycode == AM_KEY_NONE) 
    return 0;
  char event[256];
  sprintf(event, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  if(ev.keydown == 0) ev.keycode = AM_KEY_NONE;
  strcpy(buf, event);
  return strlen(event);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width;
  int height = cfg.height;
  char screen[256];
  int size = sprintf(screen, "WIDTH: %d\nHEIGHT: %d\n", width, height);
  strcpy(buf, screen);
  return size;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width;
  offset /= 4;
  len /= 4;
  int x = offset % width;
  int y = offset / width;
  //printf("offset = %u len = %u\n",offset,len);
  io_write(AM_GPU_FBDRAW, x, y, (void *)buf, len , 1, true);
  return len;

}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

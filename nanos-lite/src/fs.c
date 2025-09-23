#include <fs.h>


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  [4] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
  [5] = {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int width = cfg.width;
  int height = cfg.height;
  file_table[FD_FB].size = width * height * sizeof(uint32_t);
}

int fs_open(const char *filename, int flags, int mode){
  for(int i = 0;i < sizeof(file_table)/sizeof(file_table[0]);i++){
    if(strcmp(filename,file_table[i].name)==0){
      file_table[i].open_offset = 0;
      return i;    
    }
  }
  printf("failed to find %s\n",filename);
  assert(0);
}

size_t fs_lseek(int fd, size_t offset, int whence){
  if(fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR){
    return 0;
  }
  if(whence == SEEK_SET){
    //assert(offset <= file_table[fd].size);
    file_table[fd].open_offset = offset;
  }
  else if(whence == SEEK_CUR){
    //assert(offset + file_table[fd].open_offset <= file_table[fd].size);
    file_table[fd].open_offset += offset;
  }
  else if(whence == SEEK_END){
    assert(offset <= 0);
    file_table[fd].open_offset = file_table[fd].size + offset;
  }
  else assert(0);
  return file_table[fd].open_offset;
}

size_t fs_read(int fd, void *buf, size_t len){
  size_t nread = 0;
  if(file_table[fd].read == NULL)
    nread = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  else 
    nread = file_table[fd].read(buf, 0, len);
  file_table[fd].open_offset += len;
  return nread;
}

size_t fs_write(int fd, const void* buf, size_t len){
  size_t nwrite = 0;
  if(file_table[fd].write == NULL)
    nwrite = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  else 
    nwrite = file_table[fd].write(buf, 0, len);
  file_table[fd].open_offset += len;
  return nwrite;
}
int fs_close(int fd){
  return 0;
}

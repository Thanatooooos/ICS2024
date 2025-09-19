#include <fs.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

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
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *filename, int flags, int mode){
  int fd = -1;
  for(int i = 0;i < 24;i++){
    if(strcmp(filename,file_table[i].name)==0){
      fd = i;
      file_table[fd].open_offset = 0;
      printf("get the file %s\n",file_table[i].name);
      return fd;    
    }
  }
  assert(0);
}

size_t fs_lseek(int fd, size_t offset, int whence){
  if(fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR){
    return 0;
  }
  if(whence == SEEK_SET){
    assert(offset <= file_table[fd].size);
    file_table[fd].open_offset = offset;
  }
  else if(whence == SEEK_CUR){
    assert(offset + file_table[fd].open_offset <= file_table[fd].size);
    file_table[fd].open_offset += offset;
  }
  else if(whence == SEEK_END){
    assert(offset <= 0);
    file_table[fd].open_offset = file_table[fd].size + offset;
  }
  else assert(0);
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  if(fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR){
    return 0;
  }
  static int count = 0;
  assert(len + file_table[fd].open_offset <= file_table[fd].size);
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  count++;
  printf("successfully read %drd time\n",count);
  return len;
}
size_t fs_write(int fd, const void* buf, size_t len){
  printf("enter fs_write\n");
  if(fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR){
    if(fd == FD_STDOUT || fd == FD_STDERR){
      char *out = (char *)buf;
      for(int i=0;i<len;i++){
        putch(out[i]);
      }
    }
    return 0;
  }
  assert(len + file_table[fd].open_offset <= file_table[fd].size);
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  printf("successfully write \n");
  file_table[fd].open_offset += len;
  return len;
}
int fs_close(int fd){
  return 0;
}

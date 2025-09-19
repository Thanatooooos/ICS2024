#include <common.h>
#include "syscall.h"

int fs_open(const char *filename, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void* buf, size_t len);
int fs_close(int fd);

void sys_yield(Context *c);
void sys_exit(Context *c);
void sys_write(Context *c);
void sys_brk(Context *c);
void sys_read(Context *c);
void sys_open(Context *c);
void sys_lseek(Context *c);
void sys_close(Context *c);

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0])
  {
  case SYS_exit:
    sys_exit(c);
    break;
  case SYS_yield:
    sys_yield(c);
    break;
  case SYS_write:
    printf("call write\n");
    sys_write(c);
    break;
  case SYS_brk:
    sys_brk(c);
    break;
  case SYS_open:
    sys_open(c);
    break;
  case SYS_close:
    sys_close(c);
    break;
  case SYS_lseek:
    sys_lseek(c);
    break;
  case SYS_read:
    sys_read(c);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
void sys_yield(Context *c)
{
  c->GPRx = 0;
  yield();
}
void sys_exit(Context *c)
{
  c->GPRx = 0;
  halt(c->GPRx);
}
void sys_write(Context *c)
{
  // if (c->GPR2 == 1 || c->GPR2 == 2)
  // {
  //   char *out = (char *)c->GPR3;
  //   size_t len = c->GPR4;
  //   for (int i = 0; i < len; i++)
  //   {
  //     putch(out[i]);
  //   }
  //   c->GPRx = len;
  // }
  printf("the write in point is %p\n",(void *)c->GPR3);
  size_t len = c->GPR4;
  fs_write(c->GPR2,(void *)c->GPR3,c->GPR4);
  c->GPRx = len;
}
void sys_brk(Context *c)
{
  c->GPRx = 0;
}
void sys_read(Context *c){
  fs_read(c->GPR2,(void *)c->GPR3,c->GPR4);
}
void sys_open(Context *c){
  fs_open((char *)c->GPR2,c->GPR3,c->GPR4);
}
void sys_close(Context *c){
  fs_close(c->GPR2);
}
void sys_lseek(Context *c){
  fs_lseek(c->GPR2,c->GPR3,c->GPR4);
}
#include <common.h>
#include "syscall.h"
#include <proc.h>

struct timeval
{
#ifdef __USE_TIME_BITS64
  __time64_t tv_sec;		/* Seconds.  */
  __suseconds64_t tv_usec;	/* Microseconds.  */
#else
  long tv_sec;		/* Seconds.  */
  long tv_usec;	/* Microseconds.  */
#endif
};

void naive_uload(PCB *pcb, const char *filename);
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
void sys_gettimeofday(Context *c);
void sys_execve(Context *c);

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
  case SYS_gettimeofday:
    sys_gettimeofday(c);
    break;
  case SYS_execve:
    sys_execve(c);
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
  char s[16] = "/bin/menu";
  c->GPR2 = (uintptr_t )s;
  sys_execve(c);
}
void sys_write(Context *c)
{
  c->GPRx = fs_write(c->GPR2,(void *)c->GPR3,c->GPR4);
}
void sys_brk(Context *c)
{

  c->GPRx = 0;
}
void sys_read(Context *c){
  c->GPRx = fs_read(c->GPR2,(void *)c->GPR3,c->GPR4);
}
void sys_open(Context *c){
  c->GPRx = fs_open((char *)c->GPR2,c->GPR3,c->GPR4);
}
void sys_close(Context *c){
  c->GPRx = fs_close(c->GPR2);
}
void sys_lseek(Context *c){
  c->GPRx = fs_lseek(c->GPR2,c->GPR3,c->GPR4);
}
void sys_gettimeofday(Context *c){
  struct timeval *tv = (struct timeval *)c->GPR2;
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us % 1000000;
  c->GPRx = 0;
}
void sys_execve(Context *c){
  naive_uload(NULL,(const char *)c->GPR2);
  c->GPRx = 0;
}
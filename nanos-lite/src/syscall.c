#include <common.h>
#include "syscall.h"

void sys_yield(Context *c);
void sys_exit(Context *c);
void sys_write(Context *c);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case 0:
      sys_exit(c);
      break;
    case 1:
      sys_yield(c);
      break;
    case 4:
      printf("fd = %d\n",c->GPR2);
      sys_write(c);
      break; 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
void sys_yield(Context *c){
  c->GPRx = 0;
  yield();
}
void sys_exit(Context *c){
  c->GPRx = 0;
  halt(c->GPRx);
}
void sys_write(Context *c){
  static int offset = 0;
    char *buf = (char *)c->GPR3;
    size_t len = (size_t)c->GPR4;
    putch(*(buf + offset));
    c->GPRx = len;
  offset++;
}
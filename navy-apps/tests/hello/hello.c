#include <unistd.h>
#include <stdio.h>

int main() {
  char message[64]="helloworld!\n";
  write(1,message, sizeof(message));
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  
  return 0;
}

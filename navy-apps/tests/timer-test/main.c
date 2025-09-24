#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include<assert.h>

uint32_t NDL_GetTicks()
{
  struct timeval tv;
  assert(gettimeofday(&tv, NULL) == 0);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000 ;
}
int main()
{

   uint32_t start = NDL_GetTicks();  // 用 uint32_t 更清晰

    while (1)
    {
        uint32_t cur = NDL_GetTicks();
        uint32_t elapsed = cur - start;

        if (elapsed >= 500)  // ✅ 500 毫秒 = 0.5 秒
        {
            printf("Hello, this message prints every 0.5 seconds!\n");
            start = cur;  // 重置计时
        }

        // 小小休眠，避免 CPU 占用过高
    }

    return 0;
}
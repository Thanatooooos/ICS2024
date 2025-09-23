#include<stdio.h>
#include<sys/time.h>

int main(){
    struct timeval start_time, current_time;
    double elapsed;

    // 获取初始时间
    gettimeofday(&start_time, NULL);

    // 无限循环，每 0.5 秒输出一次
    while (1) {
        // 获取当前时间
        gettimeofday(&current_time, NULL);

        // 计算经过的时间（秒）
        elapsed = (current_time.tv_sec - start_time.tv_sec) +
                  (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

        // 检查是否已经过了 0.5 秒
        if (elapsed >= 0.5) {
            printf("Hello, this message prints every 0.5 seconds!\n");

            // 更新起始时间为当前时间，开始下一个 0.5 秒周期
            start_time = current_time;
        }

        // 可选：短暂休眠，避免过度占用 CPU
        // 休眠 10 毫秒（10000 微秒）
    }

    return 0;
}
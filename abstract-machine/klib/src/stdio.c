#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
#define PRINT_MAX_SIZE 4096
  char out[PRINT_MAX_SIZE];
  va_list args;
  va_start(args, fmt);
  int ret = vsprintf(out, fmt, args);
  for (int i = 0; (i < ret) ; i++)
  {
    putch(out[i]);
  }
  va_end(args);
  return strlen(out);
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  char *s;
  int d;
  int count = 0;
  while (*fmt)
  {
    char c = *fmt++;
    if (c == '%')
    {
      c = *fmt++;
      switch (c)
      {
      case 's':
        s = va_arg(ap, char *);
        memmove(out + count, s, strlen(s));
        count += strlen(s);
        break;
      case 'd':
      {
        d = va_arg(ap, int);
        char str[256];
        itoa(d, str);
        memmove(out + count, str, strlen(str));
        count += strlen(str);
        break;
      }
      case 'c':
        char x = (char)va_arg(ap,int);
        out[count++] = x;
        break;
      case 'p':
        uintptr_t p = va_arg(ap,uintptr_t);
        char s[256];
        ptr_to_hex_string(p,s,256);
        memmove(out+count,s,strlen(s));
        count += strlen(s);
        break;
      default:
        out[count++] = c;
        break;
      }
    }
    else
    {
      out[count++] = c;
    }
  }
  out[count] = '\0';
  return count;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int count = vsprintf(out, fmt, args);
  va_end(args);
  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

void itoa(int s, char *str)
{
  size_t i = 0;
  if (s < 0)
  {
    s = -s;
    str[i++] = '-';
  }
  int a[1000];
  int count = 0;
  while (s > 0)
  {
    a[count++] = s % 10;
    s /= 10;
  }
  count--;
  while (count >= 0)
  {
    str[i++] = a[count--] + '0';
  }
  str[i] = '\0';
}
int ptr_to_hex_string(uintptr_t addr, char *str, int max_len) {
    if (max_len < 3) return -1;

    const char hex_digits[] = "0123456789abcdef";

    // 从 str[2] 开始写入十六进制数字（为 "0x" 预留空间）
    int pos = 2;

    // 特殊情况：地址为 0
    if (addr == 0) {
        if (max_len < 4) return -1;
        str[0] = '0';
        str[1] = 'x';
        str[2] = '0';
        str[3] = '\0';
        return 3;
    }

    // 记录起始位置，用于后续反转
    int start_pos = pos;

    // 从最低位开始提取十六进制数字
    while (addr > 0 && pos < max_len - 1) {
        str[pos] = hex_digits[addr & 0xF];
        addr >>= 4;
        pos++;
    }
    str[pos] = '\0';

    if (addr > 0) return -1; // 缓冲区不足

    int hex_len = pos - start_pos;

    // 反转从 start_pos 开始的 hex_len 个字符
    for (int j = 0; j < hex_len / 2; j++) {
        char temp = str[start_pos + j];
        str[start_pos + j] = str[start_pos + hex_len - 1 - j];
        str[start_pos + hex_len - 1 - j] = temp;
    }

    // 最后填入 "0x"
    str[0] = '0';
    str[1] = 'x';

    return hex_len + 2;
}

#endif

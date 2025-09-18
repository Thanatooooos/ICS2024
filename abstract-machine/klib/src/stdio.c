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
        free(str);
        break;
      }
      case 'c':
        char x = (char)va_arg(ap,int);
        out[count++] = x;
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
#endif

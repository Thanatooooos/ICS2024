#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

char *itoa(int s, char *str)
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
  return str;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
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
        s = va_arg(args, char *);
        memmove(out + count, s, strlen(s));
        count += strlen(s);
        break;
      case 'd':
      {
        d = va_arg(args, int);
        char str[256]; 
        itoa(d, str);
        memmove(out + count, str, strlen(str));
        count += strlen(str);
        free(str);
        break;
      }
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

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

#endif

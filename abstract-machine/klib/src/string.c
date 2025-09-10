#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s)
{
  size_t size = 0;
  do
  {
    size++;
    s++;
  } while ((*s) != '\0');
  return size;
}

char *strcpy(char *dst, const char *src)
{
  assert(dst != NULL && src != NULL);
  size_t i = 0;
  do
  {
    dst[i] = src[i];
    i++;
  } while (src[i] != '\0');

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{

  size_t i = 0;

  for (; i < n && src[i] != '\0'; i++)
  {
    dst[i] = src[i];
  }
  for (; i < n; i++)
  {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src)
{
  size_t i = 0;
  while (dst[i] != '\0')
    i++;

  for (int j = 0; src[j] != '\0'; j++)
  {
    dst[i] = src[j];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2)
{
  size_t i = 0;
  do
  {
    if (s1[i] < s2[i])
    {
      return -1;
    }
    if (s1[i] > s2[i])
    {
      return 1;
    }
    i++;
  } while (s1[i] != '\0' && s2[i] != '\0');
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  size_t i = 0;
  do
  {
    if (s1[i] < s2[i])
    {
      return -1;
    }
    if (s1[i] > s2[i])
    {
      return 1;
    }
    i++;
  } while (s1[i] != '\0' && s2[i] != '\0' && i < n);
  return 0;
}

void *memset(void *s, int c, size_t n)
{
  char *p = (char *)s;
  int i = 0;
  while (i < n)
  {
    /* code */
    p[i] = c;
    i++;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n)
{
  assert(dst && src);
  if (dst > src)
  {
    while (n--)
    {
      *((char *)dst + n) = *((char *)src + n);
    }
  }
  else
  {
    size_t i = 0;
    while (i < n)
    {
      *((char *)dst + i) = *((char *)src + i);
      i++;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n)
{
  assert(out && in);
  size_t i = 0;
  while (i < n)
  {
    *((char *)out + i) = *((char *)in + i);
    i++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  char *t1 = (char *)s1;
  char *t2 = (char *)s2;
  size_t i = 0;
  while (i < n)
  {
    if (t1[i] < t2[i])
    {
      return -1;
    }
    if (t1[i] > t2[i])
    {
      return 1;
    }
    i++;
  }
  return 0;
}

#endif

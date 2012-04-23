/*
** $Id: lzio.c,v 1.34 2011/07/15 12:35:32 roberto Exp $
** a generic input stream interface
** See Copyright Notice in lua.h
*/


#include <string.h>

#define lzio_c
#define LUA_CORE

#include "lua.h"

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"

#ifdef LUA_MBCS
int zmbgetc(ZIO *z) {
  if(!z->checked && z->n > 0) {
    z->checked = 1;
    z->isbin = (*z->p) == LUA_SIGNATURE[0];
  }
  if(z->isbin) return ((z)->n--)>0 ? cast_uchar(*(z)->p++) : luaZ_fill(z);
  if(z->n > 0) {
    char s[MB_LEN_MAX];
    wchar_t ret;
    int len = mbrlen(z->p, z->n, &z->mbs), l, i;
    memset(s, 0, MB_LEN_MAX);
    if(len <= 0) {
      z->p++; z->n--;
      return 0;
    }
    l = (z->n < len) ? z->n : len;
    for (i = 0; i < l; i++, z->n--) {
      s[i] = cast(unsigned char, *z->p++);
    }
    if(i < len) {
      z->p = z->reader(NULL, z->data, &z->n);
      if (z->p == NULL || z->n == 0) return EOZ;
    }
    for (; i < len; i++, z->n--) {
      s[i] = cast(unsigned char, *z->p++);
    }
    mbrtowc(&ret, s, len, &z->mbs);
    return cast(int, ret);
  }
  else return luaZ_fill(z);
}
#endif

int luaZ_fill (ZIO *z) {
  size_t size;
  lua_State *L = z->L;
  const char *buff;
  lua_unlock(L);
  buff = z->reader(L, z->data, &size);
  lua_lock(L);
  z->n = size - 1;
  if (buff == NULL || size == 0)
    return EOZ;
#ifdef LUA_MBCS
  z->n = size;
  z->p = buff;
  return zgetc(z);
#else
  z->n = size - 1;  /* discount char being returned */
  z->p = buff;
  return cast_uchar(*(z->p++));
#endif
}


void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
#ifdef LUA_MBCS
  z->checked = 0;
  z->isbin = 0;
  memset(&z->mbs, 0, sizeof(z->mbs));
#endif
}


/* --------------------------------------------------------------- read --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (z->n == 0) {  /* no bytes in buffer? */
      if (luaZ_fill(z) == EOZ)  /* try to read more */
        return n;  /* no more input; return number of missing bytes */
      else {
        z->n++;  /* luaZ_fill consumed first byte; put it back */
        z->p--;
      }
    }
    m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}

/* ------------------------------------------------------------------------ */
char *luaZ_openspace (lua_State *L, Mbuffer *buff, size_t n) {
  if (n > buff->buffsize) {
    if (n < LUA_MINBUFFER) n = LUA_MINBUFFER;
    luaZ_resizebuffer(L, buff, n);
  }
  return buff->buffer;
}



/*
** $Id: lzio.c,v 1.31 2005/06/03 20:15:29 roberto Exp $
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
#include <ctype.h>

int zgetc(ZIO *z) {
  if(z->isbin) return (((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z));
  if((z->n--) > 0) {
    if(isleadbyte(cast(unsigned char, *z->p))) {
      if((z->n--) > 0) {
        z->p+=2;
        return char2int(*(z->p-2))<<8|char2int(*(z->p-1));
      }
      else {
        unsigned char c = cast(unsigned char, *z->p);
        /* for DBCS. If there is DBCS's first byte character in termination of read buffer,
           Lua parser fail to read Double-byte Character. */
        size_t size;
        do {
          const char *buff = z->reader(NULL, z->data, &size);
          if (buff == NULL || size == 0) return EOZ;
          z->n = size;
          z->p = buff;
        } while((z->n--) == 0);
        return c<<8|char2int(*(z->p++));
      }
    }
    else return char2int(*(z->p++));
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
  if (buff == NULL || size == 0) return EOZ;
#ifdef LUA_MBCS
  z->n = size;
  z->p = buff;
  return zgetc(z);
#else
  z->n = size - 1;
  z->p = buff;
  return char2int(*(z->p++));
#endif
}


int luaZ_lookahead (ZIO *z) {
  if (z->n == 0) {
    if (luaZ_fill(z) == EOZ)
      return EOZ;
    else {
      z->n++;  /* luaZ_fill removed first byte; put back it */
      z->p--;
    }
  }
  return char2int(*z->p);
}


void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
  z->L = L;
  z->reader = reader;
  z->data = data;
  z->n = 0;
  z->p = NULL;
#ifdef LUA_MBCS
  z->isbin = luaZ_lookahead(z) == LUA_SIGNATURE[0];
#endif
}


/* --------------------------------------------------------------- read --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (luaZ_lookahead(z) == EOZ)
      return n;  /* return number of missing bytes */
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



#ifndef lua_epoll_h
#define lua_epoll_h

#include <lua.h>

static int el_add(lua_State *L);
static int el_delete(lua_State *L);
static int el_wait(lua_State *L);
int luaopen_epoll(lua_State *L);

#endif

#ifndef lua_socket_h
#define lua_socket_h

#include <lua.h>

static int setnonblock(int fd);
static int soc_setnonblock(lua_State *L);
static int soc_close(lua_State *L);
static int soc_read(lua_State *L);
static int soc_write(lua_State *L);
static int soc_connect(lua_State *L);
static int soc_listen(lua_State *L);
static int soc_accept(lua_State *L);
int luaopen_socket(lua_State *L);

#endif

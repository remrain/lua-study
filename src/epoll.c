#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <sys/epoll.h>
#include "epoll.h"

#define MAX_EVENT 64

static int epoll_fd;

static struct epoll_event ev, events[MAX_EVENT];

static int el_add(lua_State *L) {
    int i;
    ev.data.fd = luaL_checkinteger(L, 1);
    ev.events = luaL_checkinteger(L, 2);
    for (i = 3; i <= lua_gettop(L); i++) {
        ev.events|= luaL_checkinteger(L, i);
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    return 0;
}

static int el_delete(lua_State *L) {
    ev.data.fd = luaL_checkinteger(L, 1);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev.data.fd, &ev);
    return 0;
}

static int el_wait(lua_State *L) {
    int nfds, i, timeout = -1;
    if (lua_gettop(L) >= 1) {
        timeout = luaL_checkinteger(L, 1);
    }
    nfds = epoll_wait(epoll_fd, events, MAX_EVENT, timeout);
    lua_newtable(L);
    for (i = 0; i < nfds; i++) {
        lua_pushinteger(L, events[i].data.fd);
        lua_pushinteger(L, events[i].events);
        lua_settable(L, -3);
    }
    return 1;
}

static const luaL_Reg el_lib[] = {
    {"add",    el_add},
    {"wait",   el_wait},
    {"delete", el_delete},
    {NULL,     NULL}
};

static const struct {
    const char *name;
    int event;
} el_vars[] = {
    {"EPOLLIN",  EPOLLIN},
    {"EPOLLOUT", EPOLLOUT},
    {"EPOLLET",  EPOLLET},
    {NULL,       0}
};

int luaopen_epoll(lua_State *L) 
{
    int i;
    luaL_newlib(L, el_lib);
    for (i = 0; el_vars[i].name != NULL; i++) {
        lua_pushstring(L, el_vars[i].name);
        lua_pushinteger(L, el_vars[i].event);
        lua_settable(L, -3);
    }
    epoll_fd = epoll_create(2);
    return 1;
}



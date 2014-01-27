#include <unistd.h>
#include <lua.h>
#include <lauxlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "socket.h"

#define SET_NONBLOCK

#define BUFF_SIZE 65536
#define LISTEN_BACKLOG 4096

static char buff[BUFF_SIZE];

static struct sockaddr_in soc_addr = {0};
static socklen_t addr_len = sizeof(soc_addr);

static int setnonblock(int fd) {
    int flags;
    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

static int soc_setnonblock(lua_State *L) {
    int fd;
    fd = luaL_checkinteger(L, 1);
    setnonblock(fd);
    return 0;
}

static int soc_close(lua_State *L) {
    int fd, ret;
    fd = luaL_checkinteger(L, 1);
    ret = close(fd);
    lua_pushinteger(L, ret);
    if (ret < 1) {
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    return 1;
}

static int soc_read(lua_State *L) {
    int fd, nbyte;
    fd = luaL_checkinteger(L, 1);
    nbyte = read(fd, buff, BUFF_SIZE);
    if (nbyte <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    lua_pushlstring(L, buff, nbyte);
    return 1;
}

static int soc_write(lua_State *L) {
    int fd;
    size_t nstr;
    ssize_t nbyte;
    const char *str;

    fd = luaL_checkinteger(L, 1);
    str = luaL_checklstring(L, 2, &nstr);

    nbyte = write(fd, str, nstr);
    lua_pushinteger(L, (int)nbyte);
    if (nbyte <= 0) {
        lua_pushstring(L, strerror(errno));
        return 2;
    }
    return 1;
}

static int soc_connect(lua_State *L) {
    int soc, port;
    const char *addr;

    addr = lua_tostring(L, 1);
    port = luaL_checkinteger(L, 2);

    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        lua_pushinteger(L, -1);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    soc_addr.sin_family = AF_INET;
    inet_aton(addr, &soc_addr.sin_addr);
    soc_addr.sin_port = htons(port);

#ifdef SET_NONBLOCK
    setnonblock(soc);
    if (connect(soc, (struct sockaddr*)&soc_addr, sizeof(soc_addr)) != EINPROGRESS) {
#else
    if (connect(soc, (struct sockaddr*)&soc_addr, sizeof(soc_addr)) < 0) {
#endif
        lua_pushinteger(L, -1);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_pushinteger(L, soc);
    return 1;
}

static int soc_listen(lua_State *L) {
    int soc, port;
    port = luaL_checkinteger(L, 1);

    soc_addr.sin_family = AF_INET;
    soc_addr.sin_port = htons(port);
    soc_addr.sin_addr.s_addr = INADDR_ANY;

    soc = socket(PF_INET, SOCK_STREAM, 0);
#ifdef SET_NONBLOCK
    setnonblock(soc);
#endif

    if (bind(soc, (struct sockaddr*)&soc_addr, sizeof(soc_addr)) < 0
        || listen(soc, LISTEN_BACKLOG) < 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_pushinteger(L, soc);
    return 1;
}

static int soc_accept(lua_State *L) {
    int client_sd, listenfd;
    listenfd = luaL_checkinteger(L, 1);
    client_sd = accept(listenfd, (struct sockaddr*)&soc_addr, &addr_len);
    if (client_sd < 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

#ifdef SET_NONBLOCK
    setnonblock(client_sd);
#endif
    lua_pushinteger(L, client_sd);
    return 1;
}

static const luaL_Reg soc_lib[] = {
    {"setnonblock", soc_setnonblock},
    {"listen",      soc_listen},
    {"connect",     soc_connect},
    {"accept",      soc_accept},
    {"read",        soc_read},
    {"write",       soc_write},
    {"close",       soc_close},
    {NULL,          NULL}
};

int luaopen_socket(lua_State *L) 
{
    luaL_newlib(L, soc_lib);
    return 1;
}

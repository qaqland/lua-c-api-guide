
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

typedef struct {
    int count;
} Counter;

static int get_counter(lua_State *L) {
    Counter *c = (Counter *)lua_newuserdatauv(L, sizeof(Counter), 0);
    c->count = 0;
    return 1;
}

static int inc_counter(lua_State *L) {
    Counter *c = (Counter *)lua_touserdata(L, 1);
    int n = (int)luaL_optinteger(L, 2, 1);
    c->count += n;
    lua_pushinteger(L, c->count);
    return 1;
}

static int get_count(lua_State *L) {
    Counter *c = (Counter *)lua_touserdata(L, 1);
    lua_pushinteger(L, c->count);
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, get_counter);
    lua_setglobal(L, "get_counter");
    lua_pushcfunction(L, inc_counter);
    lua_setglobal(L, "inc_counter");
    lua_pushcfunction(L, get_count);
    lua_setglobal(L, "get_count");

    const char *script = "local c = get_counter()\n"
                         "print('initial:', get_count(c))\n"
                         "print('inc 5:', inc_counter(c, 5))\n"
                         "print('inc 1:', inc_counter(c))\n"
                         "c = nil\n"
                         "collectgarbage('collect')\n"
                         "print('done')\n";

    luaL_dostring(L, script);

    lua_close(L);
    puts("07-userdata: ok");
    return 0;
}


#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    lua_State *L = luaL_newstate();

    lua_pushnil(L);
    lua_pushboolean(L, 1);
    lua_pushinteger(L, 42);
    lua_pushnumber(L, 3.14);
    lua_pushstring(L, "hello");
    lua_pushlstring(L, "hi\x00world", 8);

    assert(lua_type(L, -1) == LUA_TSTRING);
    assert(lua_isinteger(L, -2) == 0);

    size_t len;
    const char *s = lua_tolstring(L, -1, &len);
    assert(strcmp(s, "hi") == 0);
    assert(len == 8);

    assert(lua_tointeger(L, 3) == 42);
    assert(lua_tonumber(L, 4) == 3.14);

    lua_settop(L, 0);
    lua_pushinteger(L, 100);
    assert(luaL_checkinteger(L, 1) == 100);
    assert(luaL_optinteger(L, 2, 99) == 99);

    lua_close(L);
    puts("02-types: ok");
    return 0;
}

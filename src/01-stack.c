
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
    assert(lua_gettop(L) == 5);
    assert(lua_type(L, -1) == LUA_TSTRING);

    lua_pop(L, 2);
    assert(lua_gettop(L) == 3);

    assert(lua_isinteger(L, -1) == 1);
    assert(strcmp(lua_tostring(L, -1), "42") == 0);

    lua_pushvalue(L, 1);
    lua_remove(L, 1);
    lua_insert(L, 1);

    lua_settop(L, 5);
    assert(lua_gettop(L) == 5);
    lua_settop(L, 2);
    assert(lua_gettop(L) == 2);

    lua_close(L);
    puts("01-stack: ok");
    return 0;
}

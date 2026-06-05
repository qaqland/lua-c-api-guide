
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dostring(L, "return 1 + 1");
    assert(lua_tointeger(L, -1) == 2);
    lua_pop(L, 1);

    luaL_dostring(L, "function add(a, b) return a + b end");

    lua_getglobal(L, "add");
    lua_pushinteger(L, 10);
    lua_pushinteger(L, 20);
    lua_pcall(L, 2, 1, 0);
    assert(lua_tointeger(L, -1) == 30);
    lua_pop(L, 1);

    luaL_dostring(L, "function pair() return 1, 2 end");

    lua_getglobal(L, "pair");
    lua_pcall(L, 0, LUA_MULTRET, 0);
    int nrets = lua_gettop(L);
    assert(nrets == 2);
    assert(lua_tointeger(L, 1) == 1);
    assert(lua_tointeger(L, 2) == 2);
    lua_pop(L, nrets);

    lua_close(L);
    puts("03-call-lua: ok");
    return 0;
}

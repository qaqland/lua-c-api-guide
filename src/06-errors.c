
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

static int c_divide(lua_State *L) {
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    if (b == 0)
        return luaL_error(L, "division by zero");
    lua_pushinteger(L, a / b);
    return 1;
}

static int safe_divide(lua_State *L) {
    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return luaL_error(L, "debug library not available");
    }
    lua_getfield(L, -1, "traceback");
    lua_remove(L, -2);

    lua_pushcfunction(L, c_divide);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);

    if (lua_pcall(L, 2, 1, -4) != LUA_OK) {

        return 1;
    }
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, c_divide);
    lua_setglobal(L, "c_divide");

    lua_pushcfunction(L, safe_divide);
    lua_setglobal(L, "safe_divide");

    luaL_dostring(L, "print(c_divide(10, 3))");

    assert(luaL_dostring(L, "print(c_divide('a', 3))") != LUA_OK);
    lua_pop(L, 1);

    assert(luaL_dostring(L, "print(c_divide(10, 0))") != LUA_OK);
    lua_pop(L, 1);

    luaL_dostring(L, "print(safe_divide(10, 0))");

    lua_close(L);
    puts("06-errors: ok");
    return 0;
}

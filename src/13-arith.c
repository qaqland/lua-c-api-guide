
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushinteger(L, 10);
    lua_pushinteger(L, 3);
    lua_arith(L, LUA_OPADD);
    assert(lua_tointeger(L, -1) == 13);
    lua_pop(L, 1);

    lua_pushinteger(L, 10);
    lua_pushinteger(L, 3);
    lua_arith(L, LUA_OPIDIV);
    assert(lua_tointeger(L, -1) == 3);
    lua_pop(L, 1);

    lua_pushinteger(L, 7);
    lua_arith(L, LUA_OPBNOT);
    assert(lua_tointeger(L, -1) == -8);
    lua_pop(L, 1);

    lua_pushinteger(L, 5);
    lua_pushinteger(L, 10);
    assert(lua_compare(L, -2, -1, LUA_OPLT) == 1);
    assert(lua_rawequal(L, -2, -1) == 0);
    lua_pop(L, 2);

    lua_pushstring(L, "Hello");
    lua_pushstring(L, " ");
    lua_pushstring(L, "Lua");
    lua_concat(L, 3);
    assert(strcmp(lua_tostring(L, -1), "Hello Lua") == 0);
    lua_pop(L, 1);

    lua_pushinteger(L, 42);
    lua_pushstring(L, " apples");
    lua_concat(L, 2);
    assert(strcmp(lua_tostring(L, -1), "42 apples") == 0);
    lua_pop(L, 1);

    lua_pushstring(L, "hello");
    lua_len(L, -1);
    assert(lua_tointeger(L, -1) == 5);
    lua_pop(L, 2);

    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_seti(L, -2, 1);
    lua_pushinteger(L, 2);
    lua_seti(L, -2, 2);
    lua_pushinteger(L, 3);
    lua_seti(L, -2, 3);
    lua_len(L, -1);
    assert(lua_tointeger(L, -1) == 3);
    lua_pop(L, 2);

    size_t n = lua_stringtonumber(L, "3.14159");
    assert(n == 8);
    assert(lua_tonumber(L, -1) == 3.14159);
    lua_pop(L, 1);

    n = lua_stringtonumber(L, "not a number");
    assert(n == 0);

    lua_pushinteger(L, 12345);
    size_t len;
    const char *s = luaL_tolstring(L, -1, &len);
    assert(strcmp(s, "12345") == 0);
    assert(len == 5);
    lua_pop(L, 2);

    s = luaL_gsub(L, "hello world", "world", "lua");
    assert(strcmp(s, "hello lua") == 0);
    lua_pop(L, 1);

    lua_close(L);
    puts("13-arith: ok");
    return 0;
}

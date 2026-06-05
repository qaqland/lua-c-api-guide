
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_newtable(L);

    lua_pushstring(L, "Alice");
    lua_setfield(L, -2, "name");
    lua_pushinteger(L, 30);
    lua_setfield(L, -2, "age");

    lua_pushstring(L, "lua");
    lua_seti(L, -2, 1);
    lua_pushstring(L, "c");
    lua_seti(L, -2, 2);

    lua_getfield(L, -1, "name");
    assert(strcmp(lua_tostring(L, -1), "Alice") == 0);
    lua_pop(L, 1);

    lua_geti(L, -1, 1);
    assert(strcmp(lua_tostring(L, -1), "lua") == 0);
    lua_pop(L, 1);

    assert(luaL_len(L, -1) == 2);

    lua_rawgeti(L, -1, 1);
    assert(strcmp(lua_tostring(L, -1), "lua") == 0);
    lua_pop(L, 1);

    lua_pop(L, 1);

    lua_close(L);
    puts("05-tables: ok");
    return 0;
}


#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaL_addstring(&b, "Hello");
    luaL_addchar(&b, ' ');
    luaL_addstring(&b, "World");
    luaL_pushresult(&b);
    assert(strcmp(lua_tostring(L, -1), "Hello World") == 0);
    lua_pop(L, 1);

    luaL_buffinit(L, &b);
    for (int i = 1; i <= 5; i++) {
        char *p = luaL_prepbuffsize(&b, 32);
        int n = snprintf(p, 32, "[%d] ", i);
        luaL_addsize(&b, (size_t)n);
    }
    luaL_pushresult(&b);
    assert(strcmp(lua_tostring(L, -1), "[1] [2] [3] [4] [5] ") == 0);
    lua_pop(L, 1);

    luaL_buffinit(L, &b);
    luaL_addstring(&b, "values: ");
    for (int i = 1; i <= 3; i++) {
        lua_pushinteger(L, i * 10);
        luaL_tolstring(L, -1, NULL);
        luaL_addvalue(&b);
        if (i < 3)
            luaL_addstring(&b, ", ");
    }
    luaL_pushresult(&b);
    assert(strcmp(lua_tostring(L, -1), "values: 10, 20, 30") == 0);
    lua_pop(L, 1);

    luaL_buffinit(L, &b);
    for (int i = 0; i < 1000; i++) {
        luaL_addstring(&b, "x");
    }
    luaL_pushresult(&b);
    size_t len;
    lua_tolstring(L, -1, &len);
    assert(len == 1000);
    lua_pop(L, 1);

    lua_close(L);
    puts("16-buffer: ok");
    return 0;
}

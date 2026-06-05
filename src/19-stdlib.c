
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

static int open_mymodule(lua_State *L) {
    lua_newtable(L);
    lua_pushstring(L, "hello from preload");
    lua_setfield(L, -2, "msg");
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();

    luaopen_base(L);
    luaopen_math(L);
    lua_setglobal(L, "math");
    luaopen_string(L);
    lua_setglobal(L, "string");

    luaL_dostring(L, "print('math.pi =', math.pi)");
    luaL_dostring(L, "print('string.upper =', string.upper('hi'))");
    luaL_dostring(L, "print(io)");

    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    lua_pushcfunction(L, open_mymodule);
    lua_setfield(L, -2, "mymodule");
    lua_pop(L, 1);

    luaopen_package(L);
    luaL_dostring(L, "local m = require('mymodule')\n"
                    "print('mymodule.msg =', m.msg)\n");

    luaL_openlibs(L);
    luaL_dostring(L, "print('os.time =', os.time())");

    lua_close(L);
    puts("19-stdlib: ok");
    return 0;
}

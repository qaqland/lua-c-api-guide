
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    const char *script = "function counter(max)\n"
                         "    for i = 1, max do\n"
                         "        coroutine.yield(i)\n"
                         "    end\n"
                         "    return 'done'\n"
                         "end\n";

    luaL_dostring(L, script);

    lua_State *co = lua_newthread(L);
    lua_getglobal(co, "counter");
    lua_pushinteger(co, 5);

    int status;
    int nresults;
    int narg = 1;
    int expected = 1;

    while ((status = lua_resume(co, L, narg, &nresults)) == LUA_YIELD) {
        assert(lua_tointeger(co, -nresults) == expected);
        lua_pop(co, nresults);
        narg = 0;
        expected++;
    }

    assert(status == LUA_OK);
    assert(nresults == 1);
    assert(strcmp(lua_tostring(co, 1), "done") == 0);
    lua_pop(co, nresults);

    lua_pop(L, 1);

    lua_close(L);
    puts("09-coroutine: ok");
    return 0;
}

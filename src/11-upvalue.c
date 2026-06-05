
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

static int logger(lua_State *L) {
    const char *prefix = lua_tostring(L, lua_upvalueindex(1));
    const char *msg = luaL_checkstring(L, 1);
    printf("[%s] %s\n", prefix, msg);
    return 0;
}

static int counter_next(lua_State *L) {
    int step = (int)lua_tointeger(L, lua_upvalueindex(1));
    int cur = (int)lua_tointeger(L, lua_upvalueindex(2));
    int max = (int)lua_tointeger(L, lua_upvalueindex(3));

    if (cur > max)
        return 0;

    lua_pushinteger(L, cur);
    lua_pushinteger(L, cur + step);
    lua_copy(L, -1, lua_upvalueindex(2));
    lua_pop(L, 1);
    return 1;
}

static int make_counter(lua_State *L) {
    int start = (int)luaL_optinteger(L, 1, 1);
    int step = (int)luaL_optinteger(L, 2, 1);
    int max = (int)luaL_optinteger(L, 3, 10);

    lua_pushinteger(L, step);
    lua_pushinteger(L, start);
    lua_pushinteger(L, max);
    lua_pushcclosure(L, counter_next, 3);
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushstring(L, "INFO");
    lua_pushcclosure(L, logger, 1);
    lua_setglobal(L, "log_info");

    lua_pushstring(L, "WARN");
    lua_pushcclosure(L, logger, 1);
    lua_setglobal(L, "log_warn");

    lua_pushcfunction(L, make_counter);
    lua_setglobal(L, "make_counter");

    const char *script = "log_info('system started')\n"
                         "log_warn('low memory')\n"
                         "print('counter 1 to 5:')\n"
                         "for v in make_counter(1, 1, 5) do\n"
                         "    print(' ', v)\n"
                         "end\n"
                         "print('counter 10 to 30 step 5:')\n"
                         "for v in make_counter(10, 5, 30) do\n"
                         "    print(' ', v)\n"
                         "end\n";

    luaL_dostring(L, script);

    lua_close(L);
    puts("11-upvalue: ok");
    return 0;
}

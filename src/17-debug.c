
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

static int hook_calls = 0;
static int hook_lines = 0;

static void my_hook(lua_State *L, lua_Debug *ar) {
    (void)L;
    if (ar->event == LUA_HOOKCALL)
        hook_calls++;
    else if (ar->event == LUA_HOOKLINE)
        hook_lines++;
}

static int traceback_msgh(lua_State *L) {
    luaL_traceback(L, L, lua_tostring(L, 1), 1);
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_sethook(L, my_hook, LUA_MASKCALL | LUA_MASKLINE, 0);

    luaL_dostring(L, "function fact(n)\n"
                    "    if n <= 1 then return 1 end\n"
                    "    return n * fact(n - 1)\n"
                    "end\n"
                    "return fact(5)\n");
    assert(hook_calls > 0);
    assert(hook_lines > 0);
    lua_pop(L, 1);
    lua_sethook(L, NULL, 0, 0);

    lua_getglobal(L, "fact");
    lua_Debug ar;
    assert(lua_getinfo(L, ">nuS", &ar) == 1);
    assert(ar.linedefined == 1);
    assert(ar.nups == 1);
    assert(ar.nparams == 1);
    assert(ar.isvararg == 0);

    lua_pushcfunction(L, traceback_msgh);
    luaL_loadstring(L, "function a() b() end\n"
                      "function b() c() end\n"
                      "function c() error('boom') end\n"
                      "a()\n");
    assert(lua_pcall(L, 0, 0, -2) != LUA_OK);
    assert(strstr(lua_tostring(L, -1), "boom") != NULL);
    lua_pop(L, 1);
    lua_pop(L, 1);

    luaL_dostring(L, "local x = 10\n"
                    "local f = function() return x end\n"
                    "return f\n");
    const char *name = lua_getupvalue(L, -1, 1);
    assert(name != NULL);
    assert(strcmp(name, "x") == 0);
    assert(lua_tointeger(L, -1) == 10);
    lua_pop(L, 1);

    lua_pushinteger(L, 99);
    name = lua_setupvalue(L, -2, 1);
    assert(name != NULL);
    lua_pcall(L, 0, 1, 0);
    assert(lua_tointeger(L, -1) == 99);
    lua_pop(L, 1);

    if (lua_gettop(L) > 0 && lua_isfunction(L, -1))
        lua_pop(L, 1);

    lua_close(L);
    puts("17-debug: ok");
    return 0;
}

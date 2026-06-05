
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int c_add(lua_State *L) {
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a + b);
    return 1;
}

static int c_reverse(lua_State *L) {
    size_t len;
    const char *s = luaL_checklstring(L, 1, &len);

    char *buf = malloc(len);
    for (size_t i = 0; i < len; i++)
        buf[i] = s[len - 1 - i];

    lua_pushlstring(L, buf, len);
    free(buf);
    return 1;
}

static int c_sum(lua_State *L) {
    int n = lua_gettop(L);
    lua_Number sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += luaL_checknumber(L, i);
    }
    lua_pushnumber(L, sum);
    return 1;
}

static const struct luaL_Reg mylib[] = {
    {"add", c_add}, {"reverse", c_reverse}, {"sum", c_sum}, {NULL, NULL}};

int luaopen_mylib(lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, c_add);
    lua_setglobal(L, "g_add");

    luaopen_mylib(L);
    lua_setglobal(L, "mylib");

    const char *script =
        "print('mylib.add(3,4) =', mylib.add(3, 4))\n"
        "print('mylib.reverse(\"abc\") =', mylib.reverse('abc'))\n"
        "print('mylib.sum(1,2,3,4) =', mylib.sum(1, 2, 3, 4))\n"
        "print('g_add(10,20) =', g_add(10, 20))\n";

    luaL_dostring(L, script);

    lua_close(L);
    puts("04-call-c: ok");
    return 0;
}

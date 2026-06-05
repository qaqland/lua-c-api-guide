
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

static char warn_buffer[256];
static int warn_len = 0;

static void my_warn(void *ud, const char *msg, int tocont) {
    (void)ud;
    size_t n = strlen(msg);
    if ((size_t)warn_len + n < sizeof(warn_buffer)) {
        memcpy(warn_buffer + warn_len, msg, n);
        warn_len += (int)n;
    }
    if (!tocont) {
        warn_buffer[warn_len] = '\0';
        printf("[WARN] %s\n", warn_buffer);
        warn_len = 0;
    }
}

static int close_resource(lua_State *L) {
    const char *name = lua_tostring(L, 1);
    printf("[close] resource '%s' is being closed\n", name);
    return 0;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_setwarnf(L, my_warn, NULL);

    lua_warning(L, "something", 1);
    lua_warning(L, " odd happened", 0);

    (void)luaL_dostring(L, "@on");
    (void)luaL_dostring(L, "warn('deprecated API used')");
    (void)luaL_dostring(L, "@off");

    size_t n = lua_stringtonumber(L, "42");
    assert(n == 3);
    assert(lua_tointeger(L, -1) == 42);
    lua_pop(L, 1);

    n = lua_stringtonumber(L, "3.14extra");
    assert(n == 0);

    lua_newtable(L);
    lua_pushstring(L, "my_resource");
    lua_setfield(L, -2, "name");

    lua_newtable(L);
    lua_pushcfunction(L, close_resource);
    lua_setfield(L, -2, "__close");
    lua_setmetatable(L, -2);

    lua_toclose(L, -1);
    lua_pop(L, 1);

    void *ud;
    lua_Alloc allocf = lua_getallocf(L, &ud);
    assert(allocf != NULL);

    lua_close(L);
    puts("18-warn: ok");
    return 0;
}

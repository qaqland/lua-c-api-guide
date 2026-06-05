
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int ref;
    char name[32];
} Callback;

static Callback cb = {LUA_NOREF, "my_callback"};

static int set_callback(lua_State *L) {

    if (cb.ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, cb.ref);
    }

    luaL_checktype(L, 1, LUA_TFUNCTION);
    cb.ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

static int fire_callback(lua_State *L) {
    if (cb.ref == LUA_NOREF || cb.ref == LUA_REFNIL) {
        return luaL_error(L, "no callback registered");
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, cb.ref);
    lua_pushstring(L, "event data");
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
        lua_pop(L, 1);
        return 0;
    } else {
        return lua_error(L);
    }
}

static int clear_callback(lua_State *L) {
    (void)L;
    if (cb.ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, cb.ref);
        cb.ref = LUA_NOREF;
    }
    return 0;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushcfunction(L, set_callback);
    lua_setglobal(L, "set_callback");
    lua_pushcfunction(L, fire_callback);
    lua_setglobal(L, "fire_callback");
    lua_pushcfunction(L, clear_callback);
    lua_setglobal(L, "clear_callback");

    const char *script = "set_callback(function(msg)\n"
                         "    print('Lua callback got:', msg)\n"
                         "    return 'ack'\n"
                         "end)\n"
                         "fire_callback()\n"
                         "clear_callback()\n"
                         "-- 再次注册然后 fire\n"
                         "set_callback(function(msg) return 'second' end)\n"
                         "fire_callback()\n"
                         "clear_callback()\n";

    luaL_dostring(L, script);

    lua_close(L);
    puts("15-ref: ok");
    return 0;
}

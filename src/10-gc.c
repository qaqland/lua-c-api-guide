
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

#define BOX_MT "BoxMeta"

typedef struct {
    int id;
} Box;

static int box_gc(lua_State *L) {
    Box *b = (Box *)luaL_checkudata(L, 1, BOX_MT);
    printf("[gc] Box #%d collected\n", b->id);
    return 0;
}

static int box_new(lua_State *L) {
    int id = (int)luaL_checkinteger(L, 1);
    Box *b = (Box *)lua_newuserdatauv(L, sizeof(Box), 0);
    b->id = id;
    luaL_getmetatable(L, BOX_MT);
    lua_setmetatable(L, -2);
    return 1;
}

int luaopen_box(lua_State *L) {
    luaL_newmetatable(L, BOX_MT);
    lua_pushcfunction(L, box_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, box_new);
    lua_setfield(L, -2, "new");
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaopen_box(L);
    lua_setglobal(L, "box");

    int kbytes = lua_gc(L, LUA_GCCOUNT, 0);
    assert(kbytes > 0);

    const char *script = "local t = {}\n"
                         "for i = 1, 5 do\n"
                         "    t[i] = box.new(i)\n"
                         "end\n"
                         "print('created 5 boxes')\n"
                         "for i = 1, 5 do\n"
                         "    t[i] = nil\n"
                         "end\n"
                         "collectgarbage('collect')\n"
                         "print('after gc')\n";

    luaL_dostring(L, script);

    kbytes = lua_gc(L, LUA_GCCOUNT, 0);
    assert(kbytes > 0);

    lua_close(L);
    puts("10-gc: ok");
    return 0;
}


#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

#define VEC2_MT "Vec2Meta"

typedef struct {
    double x, y;
} Vec2;

static Vec2 *check_vec2(lua_State *L, int idx) {
    return (Vec2 *)luaL_checkudata(L, idx, VEC2_MT);
}

static int vec2_tostring(lua_State *L) {
    Vec2 *v = check_vec2(L, 1);
    lua_pushfstring(L, "Vec2(%f, %f)", v->x, v->y);
    return 1;
}

static int vec2_add(lua_State *L) {
    Vec2 *a = check_vec2(L, 1);
    Vec2 *b = check_vec2(L, 2);
    Vec2 *res = (Vec2 *)lua_newuserdatauv(L, sizeof(Vec2), 0);
    res->x = a->x + b->x;
    res->y = a->y + b->y;
    luaL_getmetatable(L, VEC2_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int vec2_len(lua_State *L) {
    Vec2 *v = check_vec2(L, 1);
    lua_pushnumber(L, v->x * v->x + v->y * v->y);
    return 1;
}

static int vec2_eq(lua_State *L) {
    Vec2 *a = check_vec2(L, 1);
    Vec2 *b = check_vec2(L, 2);
    lua_pushboolean(L, a->x == b->x && a->y == b->y);
    return 1;
}

static int vec2_new(lua_State *L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    Vec2 *v = (Vec2 *)lua_newuserdatauv(L, sizeof(Vec2), 0);
    v->x = x;
    v->y = y;
    luaL_getmetatable(L, VEC2_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int vec2_index(lua_State *L) {
    Vec2 *v = check_vec2(L, 1);
    const char *key = luaL_checkstring(L, 2);
    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, v->x);
        return 1;
    }
    if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, v->y);
        return 1;
    }

    luaL_getmetatable(L, VEC2_MT);
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);
    return 1;
}

int luaopen_vec2(lua_State *L) {

    luaL_newmetatable(L, VEC2_MT);
    lua_pushcfunction(L, vec2_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushcfunction(L, vec2_add);
    lua_setfield(L, -2, "__add");
    lua_pushcfunction(L, vec2_len);
    lua_setfield(L, -2, "__len");
    lua_pushcfunction(L, vec2_eq);
    lua_setfield(L, -2, "__eq");
    lua_pushcfunction(L, vec2_index);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushcfunction(L, vec2_new);
    lua_setfield(L, -2, "new");
    return 1;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaopen_vec2(L);
    lua_setglobal(L, "vec2");

    const char *script = "local vec2 = vec2\n"
                         "local a = vec2.new(1, 2)\n"
                         "local b = vec2.new(3, 4)\n"
                         "local c = a + b\n"
                         "print('a =', a)\n"
                         "print('b =', b)\n"
                         "print('a + b =', c)\n"
                         "print('a.x =', a.x)\n"
                         "print('#a (len_sq) =', #a)\n"
                         "print('a == b ?', a == b)\n"
                         "local a2 = vec2.new(1, 2)\n"
                         "print('a == a2 ?', a == a2)\n";

    luaL_dostring(L, script);

    lua_close(L);
    puts("08-metatable: ok");
    return 0;
}

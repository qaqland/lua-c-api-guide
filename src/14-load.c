
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

static char bytecode[4096];
static size_t bytecode_len = 0;

static int writer(lua_State *L, const void *p, size_t sz, void *ud) {
    (void)L;
    (void)ud;
    if (bytecode_len + sz > sizeof(bytecode)) {
        return 1;
    }
    memcpy(bytecode + bytecode_len, p, sz);
    bytecode_len += sz;
    return 0;
}

static const char *reader(lua_State *L, void *ud, size_t *sz) {
    (void)L;
    (void)ud;
    if (bytecode_len == 0) {
        *sz = 0;
        return NULL;
    }
    *sz = bytecode_len;
    bytecode_len = 0;
    return bytecode;
}

int main(void) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    const char *code = "return function(x) return x * 2 end";
    luaL_loadbufferx(L, code, strlen(code), "inline", "t");
    lua_pcall(L, 0, 1, 0);
    lua_pushinteger(L, 21);
    lua_pcall(L, 1, 1, 0);
    assert(lua_tointeger(L, -1) == 42);
    lua_pop(L, 1);

    luaL_loadstring(L, "return function(a,b) return a+b end");
    lua_pcall(L, 0, 1, 0);

    bytecode_len = 0;
    lua_dump(L, writer, NULL, 0);
    assert(bytecode_len > 0);
    lua_pop(L, 1);

    lua_load(L, reader, NULL, "from_mem", "b");
    lua_pushinteger(L, 10);
    lua_pushinteger(L, 32);
    lua_pcall(L, 2, 1, 0);
    assert(lua_tointeger(L, -1) == 42);
    lua_pop(L, 1);

    lua_close(L);
    puts("14-load: ok");
    return 0;
}


#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t total_allocated = 0;
static size_t total_freed = 0;
static size_t current_used = 0;

static void *my_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
    (void)ud;
    (void)osize;

    if (nsize == 0) {
        if (ptr) {
            total_freed += osize;
            current_used -= osize;
            free(ptr);
        }
        return NULL;
    } else {
        void *newp = realloc(ptr, nsize);
        if (newp && ptr == NULL) {

            total_allocated += nsize;
            current_used += nsize;
        } else if (newp && ptr != NULL) {

            current_used = current_used - osize + nsize;
            total_allocated += (nsize > osize) ? (nsize - osize) : 0;
        }
        return newp;
    }
}

static int my_panic(lua_State *L) {
    const char *msg = lua_tostring(L, -1);
    fprintf(stderr, "PANIC: unprotected error in call to Lua API (%s)\n",
            msg ? msg : "?");
    return 0;
}

int main(void) {

    lua_State *L = lua_newstate(my_alloc, NULL);
    if (!L) {
        fputs("cannot create state\n", stderr);
        return 1;
    }

    lua_atpanic(L, my_panic);

    assert(lua_version(L) == 504);

    luaL_openlibs(L);

    {
        int *ctx = (int *)lua_getextraspace(L);
        *ctx = 42;
        assert(*(int *)lua_getextraspace(L) == 42);
    }

    size_t before = current_used;
    (void)luaL_dostring(L, "local t = {} for i = 1, 1000 do t[i] = i end");
    assert(current_used > before);

    lua_State *co = lua_newthread(L);
    lua_pushstring(L, "hello from main");
    lua_xmove(L, co, 1);
    assert(strcmp(lua_tostring(co, -1), "hello from main") == 0);
    lua_pop(co, 1);

    void *ud;
    lua_Alloc allocf = lua_getallocf(L, &ud);
    assert(allocf == my_alloc);
    assert(ud == NULL);

    lua_close(L);
    assert(total_allocated > 0);
    assert(total_freed > 0);
    puts("12-state: ok");
    return 0;
}

#include <lua.h>
#include <stdio.h>

int main(void) {
    printf("Lua version: %.0f\n", lua_version(NULL));
    return 0;
}

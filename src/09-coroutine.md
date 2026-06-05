# 协程（Coroutine）

Lua 的协程是**非对称协程**（asymmetric coroutine），支持 `yield`/`resume`。C API 允许 C
代码创建、恢复和中断 Lua 协程。

## 核心概念

- **Thread**：在 Lua C API 中，协程的类型是 `LUA_TTHREAD`，用 `lua_State*` 表示
- 主线程也是一个 `lua_State*`，由 `luaL_newstate` 创建
- 协程有自己的调用栈和全局环境，但共享同一个全局表

## 创建协程

```c
lua_State *co = lua_newthread(L);   // 在当前栈压入 thread，返回其状态机指针 
```

创建后，协程的栈是空的。需要把要运行的函数压入它的栈：

```c
lua_State *co = lua_newthread(L);
lua_getglobal(co, "my_coroutine_func");   // 在 co 的栈上压入函数 
```

## 恢复（Resume）与让出（Yield）

### 从 C 恢复协程

```c
int status = lua_resume(co, L, nargs, &nresults);
```

| 返回值 | 含义 |
|--------|------|
| `LUA_OK` | 协程正常结束 |
| `LUA_YIELD` | 协程执行了 `coroutine.yield()`，可再次 resume |
| 其他 | 出错，错误信息在 `co` 的栈顶 |

### 在 C 函数中让出

被 Lua 协程调用的 C 函数也可以让出执行权，但需要使用 **C 延续点（C continuation）**：

```c
static int my_c_func(lua_State *L) {
        // ... 做一些工作 ... 
    return lua_yieldk(L, nresults, ctx, my_continuation);
}

static int my_continuation(lua_State *L, int status, lua_KContext ctx) {
        // 协程被 resume 后，从这里继续执行 
    return 0;
}
```

> 在标准 Lua 5.4 中，C 函数中直接调用 `lua_yield` 是允许的（在协程内）。但如果是在主线程中调用，会报错。

## 典型使用模式

```c
lua_State *co = lua_newthread(L);
lua_getglobal(co, "producer");   // 协程入口函数 

int nresults;
while ((status = lua_resume(co, L, 0, &nresults)) == LUA_YIELD) {
        // 协程 yield 了一个值，在 co 栈上 
    printf("yielded: %s\n", lua_tostring(co, -1));
    lua_pop(co, nresults);       // 清理返回值 
        // 可以再次 resume 
}

if (status != LUA_OK) {
    fprintf(stderr, "coroutine error: %s\n", lua_tostring(co, -1));
}
```

## 完整代码

```c
{{#include 09-coroutine.c}}
```

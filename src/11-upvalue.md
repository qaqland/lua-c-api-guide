# Upvalue 与 C 闭包

Lua C API 中的 `lua_CFunction` 本身是无状态的。如果希望 C 函数携带私有数据（类似 Lua 闭包中的 upvalue），需要使用
**C 闭包（C closure）**。

## 核心 API

```c
void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
```

- 将栈顶的 `n` 个值捕获为 upvalue
- 然后压入一个 C 闭包（类型为 `LUA_TFUNCTION`）


```c
#define lua_upvalueindex(i)  (LUA_REGISTRYINDEX - (i))
```

- 通过伪索引访问闭包的 upvalue
- `lua_upvalueindex(1)` 访问第 1 个 upvalue，`lua_upvalueindex(2)` 访问第 2 个，以此类推

> `lua_pushcfunction(L, f)` 只是 `lua_pushcclosure(L, f, 0)` 的宏，表示没有 upvalue。

## 最简单的示例：带前缀的日志函数

```c
static int logger(lua_State *L) {
    const char *prefix = lua_tostring(L, lua_upvalueindex(1));
    const char *msg    = luaL_checkstring(L, 1);
    printf("[%s] %s\n", prefix, msg);
    return 0;
}

// 注册时 
lua_pushstring(L, "ERROR");
lua_pushcclosure(L, logger, 1);   // 捕获 1 个 upvalue 
lua_setglobal(L, "log_error");
```

Lua 中调用：

```lua
log_error("disk full")   -- 输出: [ERROR] disk full
```

## Upvalue 是引用还是拷贝？

Upvalue 在创建闭包时**按值捕获**。如果 upvalue 是 table，捕获的是 table 引用；如果 upvalue 是
string/number，捕获的是当时的值。后续通过 `lua_upvalueindex` 读写的是闭包内部保存的那份值。

## 修改 Upvalue

可以通过 `lua_pushvalue` + `lua_setupvalue` 或直接在函数内通过栈操作修改：

```c
static int counter_next(lua_State *L) {
    int n = lua_tointeger(L, lua_upvalueindex(1));
    n = n + 1;
    lua_pushinteger(L, n);
    lua_copy(L, -1, lua_upvalueindex(1));       // 写回 upvalue 
    lua_pop(L, 1);
    return 1;
}
```

> `lua_copy(L, fromidx, toidx)` 将 `fromidx` 的值复制到 `toidx`，不修改栈高度。

## 用 Upvalue 实现迭代器

这是 C 闭包最经典的用法之一：

```c
static int iter_next(lua_State *L) {
    int i = lua_tointeger(L, lua_upvalueindex(1));
    int max = lua_tointeger(L, lua_upvalueindex(2));
    if (i > max) return 0;       // 结束迭代 
    lua_pushinteger(L, i);
    lua_pushinteger(L, i + 1);
    lua_copy(L, -1, lua_upvalueindex(1));       // 更新计数器 
    lua_pop(L, 1);
    return 1;
}

static int make_iter(lua_State *L) {
    int max = luaL_checkinteger(L, 1);
    lua_pushinteger(L, 1);         // upvalue 1: 当前值 
    lua_pushinteger(L, max);       // upvalue 2: 上限 
    lua_pushcclosure(L, iter_next, 2);
    return 1;
}
```

Lua 中使用：

```lua
for x in make_iter(5) do
    print(x)
end
```

## 与 Lua 闭包的区别

| 特性 | Lua 闭包 | C 闭包 |
|------|---------|--------|
| upvalue 数量 | 无限制 | 最多 255 个（通常够用） |
| upvalue 类型 | 任意 Lua 值 | 任意 Lua 值 |
| 共享 upvalue | 支持（同一局部变量的多个闭包共享） | 不共享（每个 C 闭包独立拷贝） |

C 闭包的 upvalue 在闭包创建时被**复制**到闭包内部。即使后续修改了原始栈上的值，已创建的闭包不受影响。

## 常见模式：模块级别的配置表

```c
// 创建模块时，把整个配置 table 作为 upvalue 
lua_newtable(L);
// ... 填充配置 ... 
lua_pushcclosure(L, my_function, 1);  // 配置表作为 upvalue 1 
```

这样 `my_function` 内部无需通过全局变量或 registry 查找配置，直接通过 `lua_upvalueindex(1)`
访问，性能更好，也更封装。

## 完整代码

```c
{{#include 11-upvalue.c}}
```

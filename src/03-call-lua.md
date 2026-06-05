# 从 C 调用 Lua

C 代码可以加载并执行 Lua 代码，也可以直接调用 Lua 函数。

## 执行 Lua 代码

最简单的方式是使用辅助库的 `luaL_dostring` 和 `luaL_dofile`（第 14 章会讲更底层的加载机制）：

```c
// 执行字符串
if (luaL_dostring(L, "return 1 + 1") != LUA_OK) {
    // 出错，错误信息在栈顶
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
}
```

它们实际上是两步的封装：先加载，再执行。

`lua_pcall` 用于在保护模式下调用栈上的函数，这是 C 与 Lua 交互的核心机制。第 6 章会深入讲解错误处理。

## 调用 Lua 函数

假设 Lua 全局环境中有一个函数 `add`：

```lua
-- config.lua
function add(a, b)
    return a + b
end
```

在 C 中调用它：

```c
// 1. 将函数压栈
// lua_getglobal 从全局表读取变量并压入栈（第 5 章会讲 table 操作）
lua_getglobal(L, "add");

// 2. 压入参数
lua_pushinteger(L, 10);
lua_pushinteger(L, 20);

// 3. 调用: 2 个参数, 1 个返回值
if (lua_pcall(L, 2, 1, 0) == LUA_OK) {
    lua_Integer result = lua_tointeger(L, -1);
    lua_pop(L, 1);       // 弹出返回值 
} else {
    fprintf(stderr, "error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
}
```

### pcall 参数详解

```c
int lua_pcall(lua_State *L, int nargs, int nresults, int msgh);
```

| 参数 | 含义 |
|------|------|
| `nargs` | 参数个数（必须在栈顶紧挨着函数） |
| `nresults` | 期望的返回值个数，或 `LUA_MULTRET`（全部） |
| `msgh` | 错误处理函数的栈索引，0 表示无 |

调用前栈布局：`[ ... | function | arg1 | arg2 | ... | argN ]`（栈顶是最后一个参数）

调用成功后，函数和参数被弹出，返回值留在栈上。

## 读取 Lua 返回值

如果 Lua 函数返回多个值：

```c
lua_getglobal(L, "multi_return");
lua_pcall(L, 0, LUA_MULTRET, 0);
int nrets = lua_gettop(L);   // 计算返回了多少个值
```

## 完整代码

```c
{{#include 03-call-lua.c}}
```

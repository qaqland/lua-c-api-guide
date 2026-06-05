# 从 Lua 调用 C

将 C 函数暴露给 Lua 是扩展 Lua 的主要方式。这允许你用 C/C++ 编写高性能模块，供 Lua 脚本调用。

## C 函数原型

所有可被 Lua 调用的 C 函数必须符合以下签名：

```c
int my_c_function(lua_State *L);
```

- 参数通过栈传递，`lua_gettop(L)` 可获取参数个数
- 返回值通过压栈实现，`return n` 表示返回 n 个值

## 最小示例

```c
static int c_add(lua_State *L) {
    lua_Integer a = luaL_checkinteger(L, 1);
    lua_Integer b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a + b);
    return 1;       // 1 个返回值 
}
```

## 注册到 Lua

### 方式一：注册为全局函数

```c
lua_pushcfunction(L, c_add);
lua_setglobal(L, "c_add");   // Lua 中可直接调用 c_add(1,2) 
```

### 方式二：组织为模块（推荐）

使用 `luaL_Reg` 数组批量注册：

```c
static const struct luaL_Reg mylib[] = {
    {"add", c_add},
    {"sub", c_sub},
    {NULL, NULL}        // 哨兵，必须以此结尾
};

// luaL_newlib 内部会创建 table 并把函数填入（第 5 章会讲 table）
int luaopen_mylib(lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;                    // 返回 table
}
```

在 Lua 中：

```lua
local mylib = require("mylib")
print(mylib.add(10, 20))
```

对于 `require` 能正确找到 `luaopen_mylib`，通常需要将 C 代码编译为共享库（`.so` / `.dll`），并放入 Lua 的
`package.cpath` 搜索路径。第 19 章会讲标准库加载和模块搜索机制。

## 完整代码

```c
{{#include 04-call-c.c}}
```

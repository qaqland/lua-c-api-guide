# 错误处理

Lua 使用**异常机制**（`longjmp`）处理错误。在 C 代码中调用 Lua 时，必须注意错误传播，否则会导致程序崩溃。

## 保护模式调用

### `lua_pcall`

始终用 `lua_pcall` 代替 `lua_call`（除非你在一个被 Lua 调用的 C 函数内部，且确定不会出错）。

```c
if (lua_pcall(L, nargs, nresults, msgh) != LUA_OK) {
        // 出错 
    const char *errmsg = lua_tostring(L, -1);
    fprintf(stderr, "Lua error: %s\n", errmsg);
    lua_pop(L, 1);       // 弹出错误信息 
}
```

### 错误处理函数（msgh）

`pcall` 的第 4 个参数可以指定一个**错误处理函数**。当发生错误时，Lua
会先调用这个函数，将原始错误信息传给它，然后将处理函数的返回值作为最终错误信息。

常见用途是打印调用栈：

```c
lua_getglobal(L, "debug");
lua_getfield(L, -1, "traceback");
lua_remove(L, -2);   // 移除 debug table，保留 traceback 函数 

// 此时 traceback 函数在栈顶，其索引就是 -1 
if (lua_pcall(L, nargs, nresults, -2) != LUA_OK) {  // 注意索引计算 
    ...
}
```

## 从 C 报告错误

### 抛出 Lua 错误

在注册给 Lua 的 C 函数中，如果发现参数错误，应该使用 `luaL_error` 或 `luaL_argerror`：

```c
static int myfunc(lua_State *L) {
    if (!lua_isinteger(L, 1)) {
        return luaL_argerror(L, 1, "expected integer");           // 不会返回 
    }
        // ... 
}
```

### 格式化错误信息

```c
return luaL_error(L, "bad value at index %d: %s", idx, reason);
```

> `luaL_error` 使用 `lua_error` 抛出异常，**不会返回到调用者**。

## C 函数中的保护模式

如果你想在 C 函数内部保护一段代码，直接在 C 函数内组织好栈，然后使用 `lua_pcall` 即可。`lua_cpcall` 已在 Lua 5.2 中移除，不再可用。

## 常见错误场景

| 场景 | 结果 | 正确做法 |
|------|------|----------|
| `lua_call` 在保护模式外调用出错代码 | 程序 `longjmp` 到未知位置，崩溃 | 使用 `lua_pcall` |
| `lua_tostring` 对非 string/number 使用 | 返回 NULL | 先用 `lua_isstring` 检查 |
| 栈索引越界 | 未定义行为 | 用 `lua_gettop` 确认栈大小 |
| `lua_tolstring` 后修改字符串 | 未定义行为 | 复制到 C 缓冲区后再修改 |

## 完整代码

```c
{{#include 06-errors.c}}
```

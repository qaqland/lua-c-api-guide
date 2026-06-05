# 调试接口

Lua 提供了一套完整的 C 级别调试 API，允许宿主程序检查调用栈、设置钩子（hook）、操作局部变量和 upvalue。

## Hook 机制

通过 `lua_sethook` 可以在特定事件发生时执行回调：

```c
void lua_sethook(lua_State *L, lua_Hook f, int mask, int count);
```

| 事件掩码 | 触发时机 |
|----------|----------|
| `LUA_MASKCALL` | 调用函数时 |
| `LUA_MASKRET` | 函数返回时 |
| `LUA_MASKLINE` | 执行新行时 |
| `LUA_MASKCOUNT` | 每执行 `count` 条指令时 |

Hook 函数签名：

```c
typedef void (*lua_Hook)(lua_State *L, lua_Debug *ar);
```

`ar->event`
表示当前事件类型：`LUA_HOOKCALL`、`LUA_HOOKRET`、`LUA_HOOKLINE`、
`LUA_HOOKCOUNT`、`LUA_HOOKTAILCALL`。

> 在 hook 中通常只能做安全检查、计数或轻量信息收集，不应执行复杂 Lua 操作。

## 获取调用栈信息

```c
int lua_getstack(lua_State *L, int level, lua_Debug *ar);
int lua_getinfo(lua_State *L, const char *what, lua_Debug *ar);
```

- `lua_getstack` 填充 `ar` 中的 `i_ci`（CallInfo 指针）
- `lua_getinfo` 根据 `what` 字符串查询详细信息

`what` 字符含义：

| 字符 | 获取信息 |
|------|----------|
| `n` | `name` 和 `namewhat`（函数名） |
| `S` | `source`、`srclen`、`linedefined`、`lastlinedefined`、`what`、`short_src` |
| `l` | `currentline` |
| `u` | `nups`、`nparams`、`isvararg` |
| `t` | `istailcall` |
| `r` | `ftransfer`、`ntransfer`（尾调用传值信息） |
| `L` | 将活跃行号压入栈（table） |
| `f` | 将正在运行的函数压入栈 |

## Lua 函数的 Upvalue 操作

这与第 11 章的 **C 闭包 upvalue** 不同，这里操作的是 **Lua 函数的 upvalue**：

```c
const char *lua_getupvalue(lua_State *L, int funcindex, int n);
const char *lua_setupvalue(lua_State *L, int funcindex, int n);
```

- `funcindex` 指向栈上的 Lua 函数
- `n` 是 upvalue 索引（从 1 开始）
- 返回 upvalue 名称

## 辅助库：luaL\_traceback

```c
void luaL_traceback(lua_State *L, lua_State *L1,
                    const char *msg, int level);
```

将 `L1` 的调用栈回溯信息压入 `L` 的栈。这是生成错误堆栈最便捷的方式。

## 完整代码

```c
{{#include 17-debug.c}}
```

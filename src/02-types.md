# 数据类型转换

Lua 是动态类型语言，C 是静态类型语言。交互时需要在两者之间转换。

## Lua ↔ C 类型对照

| Lua 类型 | C API 检查 | C API 读取 | C API 写入 |
|----------|-----------|-----------|-----------|
| nil | `lua_isnil` | — | `lua_pushnil` |
| boolean | `lua_isboolean` | `lua_toboolean` | `lua_pushboolean` |
| integer | `lua_isinteger` | `lua_tointeger` | `lua_pushinteger` |
| number (float) | `lua_isnumber` | `lua_tonumber` | `lua_pushnumber` |
| string | `lua_isstring`\* | `lua_tostring` | `lua_pushstring` |
| table | `lua_istable` | — | `lua_newtable` |
| function | `lua_isfunction` | — | `lua_pushcfunction` |
| userdata | `lua_isuserdata` | `lua_touserdata` | `lua_newuserdatauv` |
| thread | `lua_isthread` | `lua_tothread` | — |

> \*注意：`lua_isstring` 对 number 也返回 true，因为 number 可以自动转换为 string。

## 类型检查与转换

### 安全读取（带默认值）

```c
lua_Integer n = luaL_optinteger(L, 1, 10);   // 第1参数不是整数则返回10 
lua_Number  x = luaL_optnumber(L, 2, 0.0);
const char *s = luaL_optstring(L, 3, "default");
```

### 严格检查（C 函数参数校验）

```c
lua_Integer n = luaL_checkinteger(L, 1);     // 类型不对则抛出 Lua 错误 
lua_Number  x = luaL_checknumber(L, 2);
const char *s = luaL_checkstring(L, 3);
```

## 字符串处理

`lua_tostring` 返回的指针指向 Lua 内部字符串，**只要该字符串还在栈上或未被 GC，指针就有效**。不要 free 它。

对于包含 `\0` 的二进制字符串，使用 `lua_pushlstring` / `lua_tolstring`：

```c
size_t len;
const char *data = lua_tolstring(L, -1, &len);  // 获取长度和内容 
lua_pushlstring(L, data, len);                  // 压入二进制串 
```

## boolean 陷阱

`lua_toboolean` 对以下值返回 `0`（false）：

- `nil`
- `false`
- 其余所有值（包括 `0` 和空字符串）都返回 `1`（true）

这与 C 的“0 为假”逻辑不同，需特别注意。

## 完整代码

```c
{{#include 02-types.c}}
```

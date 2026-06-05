# 算术、比较与字符串操作

Lua C API 提供了直接在**栈上**执行算术运算、比较和字符串操作的函数，无需在 Lua 代码中完成。

## 栈上算术运算

```c
void lua_arith(lua_State *L, int op);
```

操作数必须从栈顶弹出：

- 一元运算（`UNM`、`BNOT`）：弹出 1 个，压入结果
- 二元运算（`ADD`、`SUB` 等）：弹出 2 个，压入结果

支持的操作码：

| 宏 | 含义 |
|----|------|
| `LUA_OPADD` | 加法 `+` |
| `LUA_OPSUB` | 减法 `-` |
| `LUA_OPMUL` | 乘法 `*` |
| `LUA_OPMOD` | 取模 `%` |
| `LUA_OPPOW` | 幂 `^` |
| `LUA_OPDIV` | 除法 `/` |
| `LUA_OPIDIV` | 整除 `//` |
| `LUA_OPBAND` | 按位与 `&` |
| `LUA_OPBOR` | 按位或 `\|` |
| `LUA_OPBXOR` | 按位异或 `~` |
| `LUA_OPSHL` | 左移 `<<` |
| `LUA_OPSHR` | 右移 `>>` |
| `LUA_OPUNM` | 取负 `-` |
| `LUA_OPBNOT` | 按位非 `~` |

> 操作数类型必须兼容（整数或浮点），否则会触发错误。在 `pcall` 保护外使用需谨慎。

## 比较

```c
int lua_compare(lua_State *L, int idx1, int idx2, int op);
int lua_rawequal(lua_State *L, int idx1, int idx2);
```

| 宏 | 含义 |
|----|------|
| `LUA_OPEQ` | 等于 `==` |
| `LUA_OPLT` | 小于 `<` |
| `LUA_OPLE` | 小于等于 `<=` |

- `lua_compare` 触发元方法（`__eq`、`__lt`、`__le`）
- `lua_rawequal` 不触发元方法，直接比较原始值

返回值：1 表示比较成立，0 表示不成立。

## 连接

```c
void lua_concat(lua_State *L, int n);
```

将栈顶 `n` 个值弹出，连接成一个字符串压回栈顶。如果值是 number，会自动转换为 string。会触发 `__concat` 元方法。

## 长度

```c
void lua_len(lua_State *L, int idx);
```

将 `#idx` 的结果压入栈顶。对 table 会触发 `__len` 元方法。对 string 返回字节长度。

## 字符串转数字

```c
size_t lua_stringtonumber(lua_State *L, const char *s);
```

将字符串 `s` 转换为 number/integer 并压入栈。返回转换消耗的字节数（0 表示失败）。

## 辅助库字符串函数

### luaL\_tolstring

```c
const char *luaL_tolstring(lua_State *L, int idx, size_t *len);
```

调用 `tostring(idx)`，将结果压入栈顶并返回字符串指针。与 `lua_tolstring` 不同，它会触发 `__tostring` 元方法。

### luaL\_gsub

```c
const char *luaL_gsub(lua_State *L, const char *s,
                       const char *p, const char *r);
```

将字符串 `s` 中的所有 `p` 替换为 `r`，结果压入栈顶并返回指针。

## 完整代码

```c
{{#include 13-arith.c}}
```

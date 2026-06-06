# 加载与字节码

除了 `luaL_loadstring` 和 `luaL_loadfile`，Lua C API
提供了更底层的加载接口，支持从任意来源读取代码、加载预编译字节码、以及将 Lua 函数序列化为字节码。

## 底层加载：lua\_load

```c
int lua_load(lua_State *L, lua_Reader reader, void *data,
             const char *chunkname, const char *mode);
```

| 参数 | 说明 |
|------|------|
| `reader` | 回调函数，每次提供一块源代码/字节码 |
| `data` | 用户数据，传给 reader |
| `chunkname` | 用于错误信息和调试信息的名字 |
| `mode` | `"t"`(文本)、`"b"`(字节码)、`"bt"`(两者) |

返回值：`LUA_OK`、`LUA_ERRSYNTAX`、`LUA_ERRMEM`、`LUA_ERRGCMM`。

> `LUA_ERRGCMM` 表示在解析过程中 GC 的 `__gc` 元方法出错。`luaL_loadfilex` 在文件打开/读取失败时还可能返回 `LUA_ERRFILE`。

### Reader 回调

```c
typedef const char * (*lua_Reader)(lua_State *L, void *ud, size_t *sz);
```

- 返回指向数据块的指针，并通过 `*sz` 设置大小
- 没有更多数据时返回 `NULL`

## 辅助库加载函数

```c
int luaL_loadfilex(lua_State *L, const char *filename, const char *mode);
int luaL_loadbufferx(lua_State *L, const char *buff, size_t sz,
                     const char *name, const char *mode);
```

- `luaL_loadfilex` 是 `luaL_loadfile` 的可指定模式版本
- `luaL_loadbufferx` 从内存缓冲区加载，可指定模式

> 出于安全考虑，默认模式通常是 `"bt"`。如果你只信任文本源码，可以强制 `"t"`。

## 序列化：lua\_dump

```c
int lua_dump(lua_State *L, lua_Writer writer, void *data, int strip);
```

将栈顶的 Lua 函数序列化为字节码。`strip` 为真时移除调试信息（减小体积但错误信息会变差）。

### Writer 回调

```c
typedef int (*lua_Writer)(lua_State *L, const void *p, size_t sz, void *ud);
```

返回 0 表示成功，非 0 表示出错（会触发 `lua_dump` 返回错误码）。

## 典型应用场景

1. **自定义加载器**：从加密文件、网络流、内存映射文件加载 Lua 代码
2. **字节码缓存**：将常用脚本预编译为字节码，运行时直接加载二进制 buffer
3. **沙箱**：限制只允许加载文本源码（`"t"`），防止加载不可信字节码

## 字节码兼容性

Lua 字节码**不保证跨版本兼容**。Lua 5.4 的字节码不能直接在 5.3 运行，反之亦然。即使是 5.4 的小版本之间，也建议重新编译。

## 完整代码

```c
{{#include 14-load.c}}
```

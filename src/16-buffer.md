# 辅助缓冲区（luaL\_Buffer）

当需要在 C 代码中逐步构建一个长字符串时，反复使用 `lua_pushstring` + `lua_concat` 效率很低。Lua 辅助库提供了专门的
**Buffer** 机制。

## 基本用法

```c
luaL_Buffer b;
luaL_buffinit(L, &b);              // 初始化 
luaL_addstring(&b, "Hello");       // 追加 
luaL_addchar(&b, ' ');
luaL_addstring(&b, "Lua");
luaL_pushresult(&b);               // 将结果压入栈 
```

Buffer 内部使用一块局部数组作为初始空间，超出后自动通过 Lua 分配器扩展。

## 常用 API

| 函数/宏 | 说明 |
|---------|------|
| `luaL_buffinit(L, B)` | 初始化 Buffer |
| `luaL_prepbuffsize(B, sz)` | 确保剩余空间 ≥ sz，返回可写入的 char\* |
| `luaL_prepbuffer(B)` | 同上，使用默认大小 `LUAL_BUFFERSIZE` |
| `luaL_addchar(B, c)` | 追加单个字符 |
| `luaL_addstring(B, s)` | 追加 C 字符串 |
| `luaL_addlstring(B, s, l)` | 追加指定长度的字符串 |
| `luaL_addvalue(B)` | 将栈顶值弹出并追加到 buffer |
| `luaL_pushresult(B)` | 完成，将最终字符串压入栈 |
| `luaL_pushresultsize(B, sz)` | 完成并指定结果大小 |
| `luaL_buffinitsize(L, B, sz)` | 初始化并预分配 sz 空间 |
| `luaL_bufflen(B)` | 当前已写入长度 |
| `luaL_buffaddr(B)` | 当前缓冲区的 char\* 地址 |

## 高级用法：直接写入缓冲区

```c
luaL_Buffer b;
char *p = luaL_prepbuffsize(&b, 256);
// 直接向 p 写入最多 256 字节 
size_t written = sprintf(p, "value=%d", 42);
luaL_addsize(&b, written);   // 通知 buffer 实际写入量 
luaL_pushresult(&b);
```

## 将栈值追加到 buffer

`luaL_addvalue` 非常有用，可以将 Lua 值（如 `tostring` 后的数字）追加：

```c
luaL_Buffer b;
luaL_buffinit(L, &b);
lua_pushinteger(L, 42);
luaL_tolstring(L, -1, NULL);   // 转换为 string 压栈 
luaL_addvalue(&b);             // 弹出并追加 
luaL_pushresult(&b);
```

> 注意：`luaL_addvalue` 会**弹出**栈顶值。

## 完整代码

```c
{{#include 16-buffer.c}}
```

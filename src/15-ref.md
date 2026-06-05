# 引用系统

Lua C API 提供了一套轻量的**引用机制**，用于在 C 代码中安全地引用 Lua 值，防止其被 GC 回收。

## 核心概念

Registry 是一个全局 table（只能通过伪索引 `LUA_REGISTRYINDEX` 访问）。引用系统利用 Registry 中的某个子 table
来保存被引用的值：

```c
int luaL_ref(lua_State *L, int t);
void luaL_unref(lua_State *L, int t, int ref);
```

| 常量 | 值 | 含义 |
|------|-----|------|
| `LUA_NOREF` | -2 | 无效引用 |
| `LUA_REFNIL` | -1 | 引用 nil（不存入 table）|

## 使用流程

```c
// 1. 把要引用的值压入栈 
lua_pushstring(L, "important data");

// 2. 创建引用（值会被弹出或移入 registry）
int ref = luaL_ref(L, LUA_REGISTRYINDEX);

// ... 之后任意时刻 ... 

// 3. 通过引用取回值 
lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
// 栈顶是 "important data" 

// 4. 释放引用，允许 GC 回收 
luaL_unref(L, LUA_REGISTRYINDEX, ref);
```

## 引用表的自动复用

`luaL_ref` 使用一种简单但高效的策略：

- 引用表的第一个元素（整数 key `0`）保存**下一个可用的空闲槽位**
- 当释放引用时，该槽位被链入空闲列表
- 因此引用值不会无限增长

> 你可以自己创建任意 table 作为引用表（不一定是 Registry），只要遵守同样的约定即可。

## 典型应用场景

1. **C 对象持有 Lua 回调**：C 结构体中保存一个 `int ref`，指向 Lua 函数。需要调用时通过 `lua_rawgeti` 取回。
2. **反向映射**：从 C 指针查找对应的 Lua userdata/table。
3. **延迟处理**：将事件相关的 Lua 值暂存，等事件触发时再取出。

## 与全局变量的区别

- 全局变量（`lua_setglobal`）用字符串 key，容易被 Lua 脚本意外覆盖
- 引用用整数 key，C 代码独占管理，更安全、更快

## 直接使用 Registry

除了 `luaL_ref`，也可以用 light userdata 作为 key 直接操作 Registry，适合存储少量 C 指针或私有数据：

```c
static char MY_KEY;   // 静态变量地址在进程内唯一

// 存入
lua_pushlightuserdata(L, (void *)&MY_KEY);
lua_pushlightuserdata(L, (void *)my_ptr);
lua_rawset(L, LUA_REGISTRYINDEX);

// 取出
lua_pushlightuserdata(L, (void *)&MY_KEY);
lua_rawget(L, LUA_REGISTRYINDEX);
void *my_ptr = lua_touserdata(L, -1);
lua_pop(L, 1);

// 删除
lua_pushlightuserdata(L, (void *)&MY_KEY);
lua_pushnil(L);
lua_rawset(L, LUA_REGISTRYINDEX);
```

这种方式不需要管理整数 ref，key 天然唯一且不会被 Lua 脚本伪造。

## 完整代码

```c
{{#include 15-ref.c}}
```

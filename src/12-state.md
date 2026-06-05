# 状态机与内存管理

除了 `luaL_newstate()` 这种开箱即用的方式，Lua C API 还提供了更底层的状态机控制接口。

## 自定义内存分配器

`luaL_newstate()` 内部使用标准 `malloc`/`free`。如果你需要自定义内存管理（如使用内存池、追踪泄漏、限制总量），可以使用
`lua_newstate`：

```c
lua_State *lua_newstate(lua_Alloc f, void *ud);
```

`lua_Alloc` 的签名：

```c
typedef void * (*lua_Alloc)(void *ud, void *ptr, size_t osize, size_t nsize);
```

| 场景 | 参数值 | 行为 |
|------|--------|------|
| 分配新块 | `ptr=NULL`, `osize=0`, `nsize>0` | 分配 `nsize` 字节 |
| 重新分配 | `ptr!=NULL`, `osize=原大小`, `nsize>0` | 调整为 `nsize` 字节 |
| 释放 | `ptr!=NULL`, `nsize=0` | 释放 `ptr` |

`osize` 的值还暗示了分配用途（Lua 内部用此做统计）：

- `osize == LUA_TSTRING` 等：为特定类型分配
- 其他值：原始字节数

## Extra Space

每个 `lua_State` 在头部预留了一块额外空间（大小由 `LUAI_EXTRASPACE` 决定），可通过宏访问：

```c
void *extra = lua_getextraspace(L);
```

常用于存储与状态机关联的自定义上下文指针，避免全局变量。

## 查询与替换分配器

```c
lua_Alloc old = lua_getallocf(L, &old_ud);
lua_setallocf(L, my_alloc, my_ud);
```

## 栈转移

当有两个 `lua_State*`（如主线程和协程）时，可以用 `lua_xmove` 在它们之间转移栈值：

```c
lua_xmove(from, to, n);   // 从 'from' 栈顶弹出 n 个值，压入 'to' 
```

`lua_xmove` 要求两个状态机共享同一个全局表（即由 `lua_newthread` 创建的协程）。任意两个独立状态机之间不能使用

> `lua_xmove`。

## 版本查询

```c
lua_Number ver = lua_version(L);
```

返回 `LUA_VERSION_NUM`（如 504），可用于运行时版本检查。

## Panic 函数

当 Lua 错误发生在没有任何保护帧的情况下（极其严重的编程错误），Lua 会调用 **panic 函数**：

```c
lua_CFunction old = lua_atpanic(L, my_panic);
```

默认 panic 函数打印错误信息并调用 `abort()`。嵌入到游戏引擎或 GUI 程序时，通常需要替换为自定义实现以优雅退出。

## 完整代码

```c
{{#include 12-state.c}}
```

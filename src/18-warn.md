# 警告系统与杂项

Lua 5.4 引入了**警告系统**，允许 Lua 核心和 C 代码发出非致命警告信息。此外还有一些零散的实用 API。

## 警告系统

### 设置警告处理函数

```c
void lua_setwarnf(lua_State *L, lua_WarnFunction f, void *ud);
```

`lua_WarnFunction` 签名：

```c
typedef void (*lua_WarnFunction)(void *ud, const char *msg, int tocont);
```

- `tocont == 0`：这是一条完整警告的结束
- `tocont != 0`：后续还有片段，应继续拼接

### 发出警告

```c
void lua_warning(lua_State *L, const char *msg, int tocont);
```

C 代码或绑定库可以用此向宿主程序报告非致命问题。

默认行为（未设置警告函数时）警告被忽略。嵌入到 IDE 或游戏引擎时，建议设置自定义函数将警告路由到日志系统。

## 字符串转数字（栈操作）

```c
size_t lua_stringtonumber(lua_State *L, const char *s);
```

将字符串 `s` 解析为 Lua number/integer 并压入栈。返回成功解析的字符数（0 表示失败）。

## 关闭槽位（to-be-closed）

Lua 5.4 支持 `<close>` 变量。C API 中对应：

```c
void lua_toclose(lua_State *L, int idx);
void lua_closeslot(lua_State *L, int idx);
```

- `lua_toclose` 将栈位置 `idx` 标记为 to-be-closed。当该位置被弹出（如作用域结束）时，如果值有 `__close`
  元方法，Lua 会调用它。
- `lua_closeslot` 强制立即关闭指定槽位。

典型用途：在 C 函数中创建需要确保清理的资源对象（如临时锁、事务句柄），即使后续发生错误也能正确释放。

## 分配器查询与替换

```c
lua_Alloc lua_getallocf(lua_State *L, void **ud);
void lua_setallocf(lua_State *L, lua_Alloc f, void *ud);
```

用于在运行时替换内存分配器。例如，某些场景下需要临时切换为带统计的分配器。

## C 栈深度限制

```c
int lua_setcstacklimit(lua_State *L, unsigned int limit);
```

设置 C 调用栈的使用上限（以字节计）。防止 C 代码递归过深导致栈溢出。返回之前的限制值。

## 完整代码

```c
{{#include 18-warn.c}}
```

# 虚拟栈（The Stack）

Lua C API 的核心是**虚拟栈**。Lua 与 C 之间不直接共享变量，所有数据交换都通过栈完成。

## 状态机的创建与销毁

操作栈之前，需要先创建一个 Lua 状态机（`lua_State`）：

```c
lua_State *L = luaL_newstate();   // 创建状态机
// ... 使用栈 ...
lua_close(L);                      // 销毁状态机，释放资源
```

`luaL_newstate` 使用默认内存分配器创建一个全新的 Lua 状态机。后续第 12 章会介绍如何自定义分配器。

## 栈的基本特性

- Lua 管理栈的内存，栈会自动增长
- C 代码负责**保持栈平衡**：压入多少就要相应弹出多少
- 栈索引从 **1** 开始表示从底向上，从 **-1** 开始表示从顶向下


```
        index         value
  top    [5] [-1]    "hello"
  |      [4] [-2]    3.14
  |      [3] [-3]    42
  |      [2] [-4]    true
  bottom [1] [-5]    nil
```

## 常用栈操作

| 操作 | 函数示例 | 说明 |
|------|----------|------|
| 压栈 | `lua_pushnil`, `lua_pushboolean`, ... | 将值压入栈顶 |
| 查询 | `lua_type`, `lua_isnumber`, ... | 获取栈中值的类型或内容 |
| 控制 | `lua_pop`, `lua_remove`, ... | 调整栈的内容 |
| 大小 | `lua_gettop`, `lua_settop` | 获取/设置栈高度 |

## 索引规则

- **绝对索引**：`1` 到 `lua_gettop(L)`，从栈底开始
- **相对索引**：`-1` 到 `-lua_gettop(L)`，从栈顶开始
- **伪索引**：如 `LUA_REGISTRYINDEX`，用于访问注册表，不在栈上

> **惯例**：Lua C API 中绝大多数接受栈索引的函数同时支持正索引和负索引，`-1` 始终代表栈顶。

## 栈平衡

C 函数操作的是共享栈，返回前必须确保栈回到预期状态。推荐在进入 C 函数时记录栈底，返回前恢复：

```c
static int myfunc(lua_State *L) {
    int base = lua_gettop(L);
    // ... 各种中间栈操作 ...
    lua_settop(L, base);       // 清理中间值
    lua_pushinteger(L, result);
    return 1;
}
```

## 栈检查

在 C 函数被 Lua 调用时，栈空间可能有限。如果你需要压入大量值，应先检查：

```c
lua_checkstack(L, 20);  // 成功返回 1，失败返回 0 
```

## 完整代码

```c
{{#include 01-stack.c}}
```

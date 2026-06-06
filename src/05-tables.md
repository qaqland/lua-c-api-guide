# 操作 Table

Table 是 Lua 唯一的数据结构，掌握 C API 中的 table 操作至关重要。

## 创建 Table

```c
lua_newtable(L);           // 等价于 lua_createtable(L, 0, 0) 
lua_createtable(L, narr, nrec);  // 预分配数组部分 narr、哈希部分 nrec 
```

## 读写字段

假设栈顶是一个 table。

### 通用 API（支持任意类型键）

```c
// 读：t[key]
lua_pushstring(L, "name");     // 压入 key
lua_gettable(L, -2);           // 查询，弹出 key，结果压栈
// 此时栈顶是 t["name"] 的值

// 写：t[key] = value
lua_pushstring(L, "name");     // 压入 key
lua_pushstring(L, "Alice");    // 压入 value
lua_settable(L, -3);           // 弹出 key 和 value，设置到 table
```

> `lua_gettable` 和 `lua_settable` 会触发 `__index` / `__newindex` 元方法。

### 便捷 API（字符串键与整数键）

当 key 为字符串或整数时，可使用更便捷的函数，无需手动压入 key：

```c
// 字符串键：t.name = "Alice"
lua_pushstring(L, "Alice");    // 压入 value
lua_setfield(L, -2, "name");   // 弹出 value，设置到 table

// 整数键：t[1] = "lua"
lua_pushstring(L, "lua");      // 压入 value
lua_seti(L, -2, 1);            // 弹出 value，设置到 table
```

> `lua_getfield` / `lua_setfield` / `lua_geti` / `lua_seti` 同样会触发对应的元方法。

## 便捷 API 汇总

| 操作 | 函数 | 说明 |
|------|------|------|
| 获取整数键值 | `lua_geti(L, idx, n)` | `t[n]`，结果压栈 |
| 设置整数键值 | `lua_seti(L, idx, n)` | `t[n] = value`，弹出栈顶 value |
| 获取字符串键值 | `lua_getfield(L, idx, k)` | `t.k`，结果压栈 |
| 设置字符串键值 | `lua_setfield(L, idx, k)` | `t.k = v`，弹出栈顶 v |
| 原始获取 | `lua_rawget` / `lua_rawgeti` / `lua_rawgetp` | 绕过元方法 |
| 原始设置 | `lua_rawset` / `lua_rawseti` / `lua_rawsetp` | 绕过元方法 |

## 遍历 Table

```c
lua_pushnil(L);   // 第一个 key 
while (lua_next(L, table_index) != 0) {
        // 此时栈顶：value (-1), key (-2) 
        // 处理 key-value... 

    lua_pop(L, 1);       // 弹出 value，保留 key 供下一次迭代 
}
```

`lua_next` 直接遍历 table 的内部存储，**不触发任何元方法**（包括 `__pairs`）。如果需要仅遍历数组部分，可用 `lua_rawlen` 获取长度后按整数下标遍历。

## 数组长度

```c
lua_Integer len = luaL_len(L, -1);   // 调用 #t，触发 __len 
size_t rawlen = lua_rawlen(L, -1);   // 不触发元方法 
```

## 完整代码

```c
{{#include 05-tables.c}}
```

# Userdata

Userdata 是 Lua 用来在脚本中保存**任意 C 数据**的类型。它是 C 扩展 Lua 的基石。

## 两种 Userdata

| 类型 | API | 特点 |
|------|-----|------|
| **Full userdata** | `lua_newuserdatauv(L, size, nuvalue)` | Lua 分配内存，受 GC 管理 |
| **Light userdata** | `lua_pushlightuserdata(L, p)` | 仅保存 C 指针，不受 GC 管理 |

## Full Userdata 详解

### 创建

```c
// 分配一块大小为 sizeof(MyStruct) 的内存
MyStruct *obj = (MyStruct *)lua_newuserdatauv(L, sizeof(MyStruct), 0);
obj->x = 0;
obj->y = 0;
```

> `lua_newuserdatauv` 的第三个参数是关联的 uservalue 数量（Lua 5.4），
> 用于将 Lua 值与 userdata 关联。此处设为 0 表示不关联。
>
> `lua_newuserdata(L, size)` 是兼容宏，等价于 `lua_newuserdatauv(L, size, 1)`，
> 默认创建 1 个 uservalue。

创建后，userdata 在 Lua 中是一个完整对象，可以赋给变量、放入 table、
作为参数传递。当不再被引用时，Lua GC 会自动回收它。

### 读写

C 代码通过返回的指针直接读写内存：

```c
// 从 Lua 获取 userdata
MyStruct *obj = (MyStruct *)lua_touserdata(L, 1);
obj->x += 10;
```

Lua 代码只能持有 userdata 引用，不能直接访问其字段。
通常通过 C 函数来读写（第 8 章会讲 metatable 如何让 userdata 支持 `.` 访问）。

## Light Userdata

用于传递已存在的 C 指针：

```c
void *some_c_ptr = ...;
lua_pushlightuserdata(L, some_c_ptr);
lua_setglobal(L, "c_handle");
```

**注意**：Light userdata 不受 GC 管理，生命周期由 C 代码保证。
它也没有独立的元表（所有 light userdata 共享同一个元表）。

## 完整代码

```c
{{#include 07-userdata.c}}
```

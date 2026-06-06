# Metatable

Metatable 让 C 可以为 userdata 和 table 定义自定义行为（运算符重载、面向对象等）。

## 常用元方法

| 元方法 | 触发时机 |
|--------|----------|
| `__index` | 访问不存在的字段（table）；访问任何字段（full userdata） |
| `__newindex` | 给不存在的字段赋值（table）；给任何字段赋值（full userdata，无 `__newindex` 时报错） |
| `__gc` | GC 回收对象时 |
| `__len` | 使用 `#` 运算符 |
| `__tostring` | `tostring()` 或 `print()` |
| `__eq`, `__lt`, `__le` | 比较运算符 |
| `__add`, `__sub`, `__mul`, `__div` 等 | 算术运算符 |
| `__call` | 将对象当作函数调用 |

## 在 C 中设置 Metatable

### 为 Userdata 设置（最常见）

```c
// 注册 metatable 到注册表 
void register_my_meta(lua_State *L) {
    luaL_newmetatable(L, "MyType");       // 若不存在则创建 

    lua_pushcfunction(L, my_tostring);
    lua_setfield(L, -2, "__tostring");

    lua_pushcfunction(L, my_add);
    lua_setfield(L, -2, "__add");

    lua_pop(L, 1);       // 弹出 metatable 
}

// 创建对象时 
MyType *obj = lua_newuserdatauv(L, sizeof(MyType), 0);
luaL_getmetatable(L, "MyType");
lua_setmetatable(L, -2);
```

### `__index` 的两种模式

**模式 A：`__index` 是一个 table**

```c
lua_newtable(L);                     // 方法表 
lua_pushcfunction(L, method1);
lua_setfield(L, -2, "method1");
lua_pushcfunction(L, method2);
lua_setfield(L, -2, "method2");

luaL_newmetatable(L, "MyType");
lua_pushvalue(L, -2);                // 复制方法表 
lua_setfield(L, -2, "__index");      // mt.__index = methods 
lua_pop(L, 2);
```

这是实现面向对象风格 `obj:method()` 最简洁的方式。

**模式 B：`__index` 是一个函数**

适合需要动态计算字段，或字段名不确定的情况。

### `__gc` 元方法（析构）

当 userdata 被 GC 回收时，如果其 metatable 有 `__gc` 字段，Lua 会调用它。
这是释放 C 资源（如文件句柄、网络连接）的正确位置：

```c
static int mystruct_gc(lua_State *L) {
    MyStruct *obj = (MyStruct *)luaL_checkudata(L, 1, "MyStructMeta");
    printf("MyStruct %p collected\n", (void *)obj);
    return 0;
}
```

### 类型安全检查

用 `luaL_checkudata` 确保参数是指定类型的 userdata：

```c
MyStruct *obj = (MyStruct *)luaL_checkudata(L, 1, "MyStructMeta");
```

如果类型不匹配，会自动抛出 Lua 错误。

### 注册表（Registry）

`luaL_newmetatable` 实际上是在 **Lua Registry** 中创建一个以字符串为 key 的 table。
Registry 是一个全局的、对 C 代码可见的 table，Lua 脚本无法直接访问
（除非用 debug 库）。它是 C 模块间共享数据的常用机制。

```c
// 手动读写 registry
lua_pushstring(L, "my_key");
lua_gettable(L, LUA_REGISTRYINDEX);   // 读取 registry["my_key"]

lua_pushstring(L, "my_key");
lua_pushinteger(L, 42);
lua_settable(L, LUA_REGISTRYINDEX);   // registry["my_key"] = 42
```

## 完整代码

```c
{{#include 08-metatable.c}}
```

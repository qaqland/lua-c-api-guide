# 标准库加载

Lua 的核心解释器本身只提供虚拟机、垃圾回收和 C API。所有内置函数（`print`、`math.sin`、`string.format`
等）都来自**标准库**，由独立的 C 模块实现。

## lualib.h

头文件 `lualib.h` 声明了所有标准库的打开函数：

| 函数 | 提供的 Lua 模块/全局 |
|------|---------------------|
| `luaopen_base(L)` | `_G`、assert、print、type、pairs 等基础函数 |
| `luaopen_coroutine(L)` | `coroutine` |
| `luaopen_table(L)` | `table` |
| `luaopen_io(L)` | `io` |
| `luaopen_os(L)` | `os` |
| `luaopen_string(L)` | `string` |
| `luaopen_utf8(L)` | `utf8` |
| `luaopen_math(L)` | `math` |
| `luaopen_debug(L)` | `debug` |
| `luaopen_package(L)` | `package`、`require` |

## 打开全部标准库

```c
#include <lualib.h>

luaL_openlibs(L);   // 打开所有标准库 
```

这等价于依次调用上述所有 `luaopen_*` 函数。

## 按需加载

在沙箱或内存受限环境中，通常只加载必要的库：

```c
luaopen_base(L);
luaopen_math(L);
luaopen_string(L);
// 不加载 io/os/debug，限制文件系统和环境访问 
```

## 自定义 package 搜索器

`luaopen_package` 注册了 `require` 机制和默认搜索路径。你可以：

1. **修改 `package.path` / `package.cpath`** 指向自定义目录
2. **插入自定义搜索器**到 `package.searchers` 中
3. **完全替换** `require` 的实现（手动注册同名全局函数）

## 预加载模块

`lauxlib.h` 中定义了 Registry key：

```c
#define LUA_PRELOAD_TABLE "_PRELOAD"
```

可以通过在预加载表中注册函数，让 `require` 在未找到文件时调用你的加载器：

```c
luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
lua_pushcfunction(L, my_custom_loader);
lua_setfield(L, -2, "mymodule");
lua_pop(L, 1);
```

之后在 Lua 中 `require("mymodule")` 就会调用 `my_custom_loader`。

## 完整代码

```c
{{#include 19-stdlib.c}}
```

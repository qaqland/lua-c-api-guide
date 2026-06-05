# 总览与准备

> 本指南由 **Kimi K2.6** 编写生成。
>
> 仓库地址：https://github.com/qaqland/lua-c-api-guide

## 什么是 Lua C API

Lua 被设计为一个**可嵌入的脚本语言**。它提供了一组 C 语言接口，允许宿主程序（Host Program）：

- 创建和销毁 Lua 状态机（`lua_State`）
- 在 C 和 Lua 之间传递数据
- 从 C 调用 Lua 函数
- 将 C 函数暴露给 Lua 脚本
- 操作 Lua 中的 table、userdata、metatable 等高级类型

## 关键头文件

只需关注两个头文件：

| 头文件 | 作用 |
|--------|------|
| `lua.h` | 核心 API。所有以 `lua_` 开头的函数 |
| `lauxlib.h` | 辅助库（auxiliary library）。以 `luaL_` 开头的辅助函数，让代码更简洁安全 |

通常 C 文件顶部只需要：

```c
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>   // 如需打开标准库 
```

## 编译环境

需要预先安装 Lua 5.4 开发包（如 `lua5.4-dev`）。下面介绍两种编译方式：手动用 `gcc`，以及本指南使用的
[Meson](https://mesonbuild.com/)。

### 手动编译

通过 `pkg-config` 获取头文件和库路径，手动调用 `gcc`：

```bash
gcc 00-overview.c $(pkg-config --cflags --libs lua5.4) -o 00-overview
./00-overview
```

### Meson（本指南使用）

Meson 自动通过 `pkg-config` 获取 Lua 5.4 的编译参数，无需手动指定 `-I`、`-L`、`-l`：

```bash
meson setup build
meson compile -C build
./build/src/01_stack
```

## 最小可运行程序

```c
{{#include 00-overview.c}}
```

这个程序只打印 Lua 版本号，验证开发环境是否正确链接了 Lua 库。后续章节会逐步介绍状态机的创建、使用和销毁。

## 参考资源

- [Lua 5.4 参考手册 — C API](https://www.lua.org/manual/5.4/manual.html#4)
- [Lua 5.4 参考手册 — 标准库](https://www.lua.org/manual/5.4/manual.html#6)

源码头文件（Lua 5.4 官方仓库）：

- [`lua.h`](https://github.com/lua/lua/blob/v5.4.8/lua.h) — 核心 C API 声明
- [`lauxlib.h`](https://github.com/lua/lua/blob/v5.4.8/lauxlib.h) — 辅助库声明
- [`luaconf.h`](https://github.com/lua/lua/blob/v5.4.8/luaconf.h) — 编译配置与平台适配
- [`lualib.h`](https://github.com/lua/lua/blob/v5.4.8/lualib.h) — 标准库加载函数

## 致谢

感谢 [st0nie](https://github.com/st0nie) 提供支持。

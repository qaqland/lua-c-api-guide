# Lua C API 交互指南

> 面向有 C 和 Lua 基础、但尚未接触 Lua C API 的开发者。
>
> 本指南由 **Kimi K2.6** 编写生成。

本指南通过**文档 + 可编译代码示例**的方式，系统讲解 Lua 与 C 的交互机制，涵盖从基础栈操作到高级主题（userdata、metatable、协程、GC、调试接口）的完整内容。

所有示例代码使用系统的 **Lua 5.4 开发包**编译（通过 `pkg-config lua5.4` 自动获取头文件和库路径）。

---

## 目录

| 章节 | 文档 | 示例 |
|------|------|------|
| 总览与准备 | [src/00-overview.md](src/00-overview.md) | — |
| 01. 虚拟栈 | [src/01-stack.md](src/01-stack.md) | `src/01-stack.c` |
| 02. 数据类型转换 | [src/02-types.md](src/02-types.md) | `src/02-types.c` |
| 03. 从 C 调用 Lua | [src/03-call-lua.md](src/03-call-lua.md) | `src/03-call-lua.c` |
| 04. 从 Lua 调用 C | [src/04-call-c.md](src/04-call-c.md) | `src/04-call-c.c` |
| 05. 操作 Table | [src/05-tables.md](src/05-tables.md) | `src/05-tables.c` |
| 06. 错误处理 | [src/06-errors.md](src/06-errors.md) | `src/06-errors.c` |
| 07. Userdata | [src/07-userdata.md](src/07-userdata.md) | `src/07-userdata.c` |
| 08. Metatable | [src/08-metatable.md](src/08-metatable.md) | `src/08-metatable.c` |
| 09. 协程 | [src/09-coroutine.md](src/09-coroutine.md) | `src/09-coroutine.c` |
| 10. 垃圾回收 | [src/10-gc.md](src/10-gc.md) | `src/10-gc.c` |
| 11. Upvalue 与 C 闭包 | [src/11-upvalue.md](src/11-upvalue.md) | `src/11-upvalue.c` |
| 12. 状态机与内存 | [src/12-state.md](src/12-state.md) | `src/12-state.c` |
| 13. 算术比较与字符串 | [src/13-arith.md](src/13-arith.md) | `src/13-arith.c` |
| 14. 加载与字节码 | [src/14-load.md](src/14-load.md) | `src/14-load.c` |
| 15. 引用系统 | [src/15-ref.md](src/15-ref.md) | `src/15-ref.c` |
| 16. 缓冲区 | [src/16-buffer.md](src/16-buffer.md) | `src/16-buffer.c` |
| 17. 调试接口 | [src/17-debug.md](src/17-debug.md) | `src/17-debug.c` |
| 18. 警告与杂项 | [src/18-warn.md](src/18-warn.md) | `src/18-warn.c` |
| 19. 标准库 | [src/19-stdlib.md](src/19-stdlib.md) | `src/19-stdlib.c` |


---

## 阅读文档

文档使用 [mdBook](https://github.com/rust-lang/mdBook) 构建为在线图书：

```bash
# 构建静态站点
mdbook build

# 本地预览（带热重载）
mdbook serve --open
```

构建输出在 `book/` 目录，可直接用浏览器打开 `book/index.html`。

## 编译示例

```bash
meson setup build
meson compile -C build
./build/src/01-stack
```

需要系统安装 Lua 5.4 开发包（例如 `lua5.4-dev` 或 `lua-devel`），Meson 通过 `pkg-config lua5.4` 自动获取编译参数。

---

## 核心原则

在深入各章节前，请记住 Lua C API 的三条核心设计原则：

1. **一切皆栈**：C 与 Lua 之间的所有数据交换都通过 Lua 的虚拟栈完成。
2. **C 侧无自动内存管理**：栈由 Lua 管理，但 C 代码必须显式平衡栈（push/pop 成对）。
3. **错误通过 `longjmp` 传播**：保护模式（pcall）之外发生的 Lua 错误会非局部跳转到最近的恢复点，C 代码必须正确处理。

---

## 贡献

本项目完全由 AI 生成，不接受外部代码或文档的直接贡献（Pull Request）。如有问题或建议，请通过 Issue 反馈，维护者会根据描述生成修复。

感谢 [st0nie](https://github.com/st0nie) 提供支持。

# Lua 5.4 C API 交互指南 — Agent 说明

> 本文档面向 AI 编程助手。阅读前请确认你已了解：本项目是一个**中文**的 Lua 5.4 C API 教程，通过成对的 Markdown 文档 + 可编译 C 示例代码进行教学。

---

## 项目概述

本项目（`lua-c-api-guide`）是一份面向有 C 和 Lua 基础、但尚未接触 Lua C API 的开发者的交互式教程。

- **语言**：文档与注释主要使用简体中文。
- **内容形式**：每个主题由一篇 `.md` 文档和一份可独立编译运行的 `.c` 示例组成。
- **生成历史**：本指南由 **Kimi K2.6** 编写生成。
- **目标读者**：C 开发者，希望学习如何在宿主程序中嵌入 Lua 5.4。

核心教学范围涵盖：

- Lua 虚拟栈操作
- C 与 Lua 之间的数据类型转换
- 从 C 调用 Lua 函数 / 从 Lua 调用 C 函数
- Table、Userdata、Metatable、协程、GC、调试接口等高级主题
- 标准库加载、引用系统、辅助缓冲区、Upvalue 与 C 闭包

---

## 技术栈与依赖

| 组件 | 用途 | 版本/说明 |
|------|------|-----------|
| C (C99) | 示例代码语言 | `c_std=c99`，`warning_level=2` |
| Lua 5.4 | 嵌入式脚本引擎 | 通过 `pkg-config lua5.4` 自动发现 |
| Meson | C 代码构建系统 | `meson.build` + `src/meson.build` |
| mdBook | 文档站点生成 | `book.toml` 配置，输出到 `book/` |
| clang-format | C 代码格式化 | 配置见 `.clang-format` |
| comrak (可选) | Markdown 格式化 | 仅用于 `make fmt-doc` |

**系统依赖**：

```bash
# Debian/Ubuntu
sudo apt install lua5.4-dev meson clang-format mdbook

# Fedora/RHEL
sudo dnf install lua-devel meson clang-format mdbook
```

---

## 目录结构

```
.
├── src/                    # 源码与文档源文件
│   ├── SUMMARY.md          # mdBook 目录结构定义
│   ├── 00-overview.md      # 总览（参考资源合并于此）
│   ├── 00-overview.c       # 最小可运行验证程序
│   ├── 01-stack.md         # 章节文档
│   ├── 01-stack.c          # 对应可编译示例
│   ├── ...                 # 02 ~ 19 依此类推
│   └── 19-stdlib.md        # 最后一章
├── book/                   # mdBook 构建输出（静态站点）
│   └── .nojekyll           # 用于 GitHub Pages 部署
├── build/                  # Meson 构建目录
├── meson.build             # 根级 Meson 配置
├── src/meson.build         # 示例程序列表与编译目标
├── book.toml               # mdBook 配置
├── Makefile                # 便捷命令封装
├── .clang-format           # C 代码风格配置
└── .gitignore              # 忽略 book/、build/ 等
```

**命名规则**：

- 章节编号使用两位数字前缀：`01-stack`、`02-types` … `19-stdlib`。
- 每个编号对应一对同名 `.md` + `.c` 文件。
- `00-overview` 只有一篇文档和一个极简验证程序。

---

## 构建与运行

### 编译 C 示例

```bash
# 首次配置
meson setup build

# 编译所有示例
meson compile -C build

# 运行单个示例
./build/src/01-stack
```

Meson 自动通过 `pkg-config lua5.4` 获取 `-I`、`-L`、`-l` 参数，无需手动指定。

### 构建文档站点

```bash
# 生成静态站点到 book/
mdbook build

# 本地预览（带热重载）
mdbook serve --open
```

构建输出位于 `book/`，可直接用浏览器打开 `book/index.html`。

### Makefile 便捷命令

```bash
make build      # 等价于 build-src + build-doc
make build-src  # 自动 meson setup build（如未配置）并编译
make build-doc  # mdbook build
make fmt        # 格式化 C 源码 + Markdown
make fmt-src    # clang-format -i src/*.c
make fmt-doc    # comrak -i src/*.md
```

### 运行测试

```bash
meson test -C build   # 运行所有示例，assert 失败即测试失败
```

---

## 代码风格与规范

### C 代码格式

配置位于 `.clang-format`：

```yaml
ColumnLimit: 80
IndentWidth: 4
UseTab: Never
```

**编码约定**：

- 使用 C99 标准。
- 缩进为 4 个空格，不使用 Tab。
- 行宽限制 80 列。
- 头文件包含顺序：先 Lua 头文件（`<lauxlib.h>`、`<lua.h>`、`<lualib.h>`），再标准 C 头文件。
- 每个 `.c` 文件都是独立可执行程序，包含 `main()` 函数。
- Lua C 函数使用 `static` 限定，返回值表示压入栈的结果数量。
- 错误处理优先使用 `luaL_check*` / `luaL_opt*` 系列辅助函数。
- 调用 Lua 代码优先使用 `lua_pcall` 或 `luaL_dostring` 检查返回值 `LUA_OK`。
- 示例代码使用 `assert` 验证 API 行为后的状态（栈高度、类型、返回值内容），而非单纯检查"操作是否成功"。

### Markdown 文档

- 文档语言为简体中文。
- 使用 mdBook 的 `{{#include filename.c}}` 语法嵌入对应 C 代码。
- 每个 `.md` 文件末尾应有"完整代码"小节，通过 `{{#include}}` 嵌入整份 `.c` 源码。
- 章节结构使用 ATX 标题（`#`）。

---

## 修改注意事项

### 新增章节

1. 在 `src/` 下创建新的 `NN-topic.md` 与 `NN-topic.c`（如适用）。
2. 将新章节加入 `src/SUMMARY.md`，确保 mdBook 能正确生成目录。
3. 在 `src/meson.build` 的 `examples` 数组中添加对应名称（不含扩展名），Meson 会自动生成可执行目标和测试。
4. 在新 `.md` 文件末尾添加"完整代码"小节，通过 `{{#include NN-topic.c}}` 嵌入源码。
5. 运行 `meson compile -C build` 验证编译通过。
6. 运行 `meson test -C build` 验证所有示例 assert 通过。
7. 运行 `mdbook build` 验证文档生成无报错。

### 修改现有章节

- 文档与代码**必须同步更新**。mdBook 通过 `{{#include}}` 直接引用 `.c` 文件，修改 C 代码会立即反映到文档中。
- 保持示例程序的可独立运行性：每个 `.c` 文件应能单独编译并执行。
- 若修改导致章节编号变化，需同步更新文件名、`SUMMARY.md`、`meson.build` 中的 `examples` 数组，以及所有文档内引用的"第 X 章"。
- 若删除章节，需同步删除 `SUMMARY.md` 条目、`meson.build` 中的名称、`src/` 下的 `.md` 与 `.c` 文件，并顺延后续章节编号。

---

## 测试策略

本项目使用 **meson test** 作为自动化测试入口。每个可执行示例都被注册为测试目标，`assert` 失败即返回非零退出码，测试失败。

测试方式如下：

1. **编译测试**：`meson compile -C build` 零警告、零错误通过。
2. **自动化测试**：`meson test -C build` 运行所有示例，assert 失败即测试失败。
3. **文档构建测试**：`mdbook build` 成功且无 broken link 或 include 错误。

---

## 部署说明

- `book/` 目录是完整的静态站点，可直接托管到任何静态文件服务器。
- 仓库已包含 `book/.nojekyll`，支持直接作为 GitHub Pages 源目录发布。
- `build/` 目录为本地构建产物，不应提交到版本控制（已在 `.gitignore` 中排除）。

---

## 安全与最佳实践提示

本项目为教学用途，代码示例旨在清晰展示 API 用法。修改时请注意：

- **栈平衡**：C 函数返回前必须确保栈处于预期状态；中间计算可用 `lua_settop(L, base)` 清理。
- **错误传播**：裸调用 `lua_call` 会导致 `longjmp` 跳过 C 栈帧。非受信代码务必使用 `lua_pcall`。
- **指针生命周期**：`lua_tostring` / `lua_tolstring` 返回的指针在对应值被弹出或 GC 后失效，不要长期持有。
- **线程安全**：`lua_State*` 不是线程安全的，禁止在多个 OS 线程中无保护地共享同一个状态机指针。

更详细的编码规范请参阅各章节正文及 `src/01-stack.md` 中的"栈平衡"小节。

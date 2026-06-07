# Lua 5.4 参考手册（中英文对照 + 源码注解）

> 本文档基于 Lua 5.4 源码目录中的 `manual/manual.of` 逐行翻译，并结合 `lapi.c`、`lauxlib.c`、`lua.h`、`lgc.c`、`ltm.c` 等源码进行理解注解。
>
> 每段英文原文后紧跟中文翻译；对其中较复杂的关键词和概念，单独用列表展开解释；涉及实现细节时，结合源码（如 `lapi.c`、`lauxlib.c`、`lua.h`、`lgc.c`、`ltm.c` 等）搜索印证并补充注解。后续续写时请保持相同格式。

---

# 1 – Introduction（引言）

Lua is a powerful, efficient, lightweight, embeddable scripting language.
Lua 是一门强大、高效、轻量、可嵌入的脚本语言。

- **powerful**（强大）：虽然体积小，但表达能力很强，支持多种编程范式。
- **efficient**（高效）：基于寄存器的虚拟机（见 `lvm.c`），执行速度快。
- **lightweight**（轻量）：标准库精简，核心可裁剪，适合资源受限环境。
- **embeddable**（可嵌入）：以 C 库形式实现（见 `lapi.c`），可轻松嵌入 C/C++ 程序。
- **scripting language**（脚本语言）：解释执行字节码，无需编译为机器码。

It supports procedural programming, object-oriented programming, functional programming, data-driven programming, and data description.
它支持过程式编程、面向对象编程、函数式编程、数据驱动编程和数据描述。

- **procedural programming**（过程式）：按步骤执行的编程方式。
- **object-oriented programming**（OOP，面向对象）：通过表（table）和元表（metatable）模拟类和对象。
- **functional programming**（函数式）：函数是第一类值（first-class values），可作参数、返回值。
- **data-driven programming**（数据驱动）：程序逻辑由数据决定。
- **data description**（数据描述）：常用于配置文件、数据表示。

Lua combines simple procedural syntax with powerful data description constructs based on associative arrays and extensible semantics.
Lua 将简洁的过程式语法与基于关联数组和可扩展语义的强大数据描述构造相结合。

- **associative arrays**（关联数组）：即 Lua 的 table，可用任意值（除 nil/NaN 外）作索引。
- **extensible semantics**（可扩展语义）：通过元表（metatable）和元方法（metamethod）自定义行为。

Lua is dynamically typed, runs by interpreting bytecode with a register-based virtual machine, and has automatic memory management with a generational garbage collection, making it ideal for configuration, scripting, and rapid prototyping.
Lua 是动态类型的，通过基于寄存器的虚拟机解释字节码运行，并采用分代垃圾回收实现自动内存管理，使其非常适合配置、脚本编写和快速原型开发。

- **dynamically typed**（动态类型）：运行时确定类型，区别于 C 的静态类型。
- **register-based virtual machine**（基于寄存器的虚拟机）：指令操作寄存器而非栈，效率更高（见 `lvm.c` 与 `lopcodes.h`）。
- **generational garbage collection**（分代垃圾回收）：GC 有增量和分代两种模式（见 `lgc.c`）。

Lua is implemented as a library, written in clean C, the common subset of standard C and C++.
Lua 以库的形式实现，使用 clean C（标准 C 与 C++ 的公共子集）编写。

- **implemented as a library**（以库实现）：核心不依赖主函数，由宿主程序链接调用（见 `lapi.c`）。
- **clean C**：兼容 C89/C99 和 C++，无复杂扩展。

The Lua distribution includes a host program called lua, which uses the Lua library to offer a complete, standalone Lua interpreter, for interactive or batch use.
Lua 发行版包含一个名为 `lua` 的宿主程序，它使用 Lua 库提供一个完整的独立 Lua 解释器，支持交互式或批处理使用。

- **host program**（宿主程序）：即 `lua.c` 中的 `main` 函数，演示如何嵌入 Lua 库。
- **standalone interpreter**（独立解释器）：不依赖外部宿主，可直接运行 `.lua` 文件。

Lua is intended to be used both as a powerful, lightweight, embeddable scripting language for any program that needs one, and as a powerful but lightweight and efficient stand-alone language.
Lua 旨在既成为任何需要脚本语言的应用的强大、轻量、可嵌入的脚本语言，也成为一门强大但轻量且高效的独立语言。

As an extension language, Lua has no notion of a "main" program: it works embedded in a host client, called the embedding program or simply the host.
作为扩展语言，Lua 没有 "main" 程序的概念：它以嵌入方式工作在宿主客户端中，该客户端称为 embedding program（宿主程序）或简称为 host（宿主）。

- **embedding program**（宿主程序）：调用 Lua C API 加载/执行代码的外部程序。

(Frequently, this host is the stand-alone lua program.)
（通常，这个宿主就是独立的 `lua` 程序。）

The host program can invoke functions to execute a piece of Lua code, can write and read Lua variables, and can register C functions to be called by Lua code.
宿主程序可以调用函数来执行一段 Lua 代码，可以读写 Lua 变量，还可以注册 C 函数供 Lua 代码调用。

- **register C functions**（注册 C 函数）：通过 `lua_pushcfunction` / `lua_setglobal` 将 C 函数暴露给 Lua（见 `lapi.c`）。

Through the use of C functions, Lua can be augmented to cope with a wide range of different domains, thus creating customized programming languages sharing a syntactical framework.
通过使用 C 函数，Lua 可以被扩展以应对各种不同的领域，从而创建共享同一语法框架的定制编程语言。

Lua is free software, and is provided as usual with no guarantees, as stated in its license.
Lua 是自由软件，按惯例不提供任何担保，具体见其许可证。

The implementation described in this manual is available at Lua's official web site, www.lua.org.
本手册中描述的实现可在 Lua 官方网站 `www.lua.org` 获取。

Like any other reference manual, this document is dry in places.
与其他参考手册一样，本文档在某些地方较为枯燥。

For a discussion of the decisions behind the design of Lua, see the technical papers available at Lua's web site.
关于 Lua 设计决策的讨论，请参阅 Lua 网站上的技术论文。

For a detailed introduction to programming in Lua, see Roberto's book, Programming in Lua.
关于 Lua 编程的详细介绍，请参阅 Roberto 的著作 *Programming in Lua*（《Lua 程序设计》）。

---

# 2 – Basic Concepts（基本概念）

This section describes the basic concepts of the language.
本节描述语言的基本概念。

## 2.1 – Values and Types（值与类型）

Lua is a dynamically typed language. This means that variables do not have types; only values do. There are no type definitions in the language. All values carry their own type.
Lua 是一门动态类型语言。这意味着变量没有类型，只有值才有类型。语言中没有类型定义，所有值都自带类型。

- **dynamically typed**（动态类型）：运行时确定类型，区别于 C 的静态类型。
- **values carry their own type**（值自带类型）：每个 `TValue` 内部有类型标记（`tt_` 字段，见 `lobject.h`）。

All values in Lua are first-class values. This means that all values can be stored in variables, passed as arguments to other functions, and returned as results.
Lua 中的所有值都是一等值（first-class values）。这意味着所有值都可以存储在变量中、作为参数传递给其他函数、并作为结果返回。

- **first-class values**（一等值）：函数、表等均可像数字一样自由传递。

There are eight basic types in Lua: nil, boolean, number, string, function, userdata, thread, and table.
Lua 中有八种基本类型：`nil`、`boolean`、`number`、`string`、`function`、`userdata`、`thread` 和 `table`。

- 源码对应：见 `lua.h` 中的枚举 `LUA_TNIL`、`LUA_TBOOLEAN`、`LUA_TNUMBER` 等。

The type nil has one single value, nil, whose main property is to be different from any other value; it often represents the absence of a useful value.
类型 `nil` 只有一个值 `nil`，其主要属性是与其他任何值都不同；它通常表示没有有用的值。

- **nil**：在源码中定义为 `LUA_TNIL`，空指针语义。

The type boolean has two values, false and true. Both nil and false make a condition false; they are collectively called false values. Any other value makes a condition true.
类型 `boolean` 有两个值：`false` 和 `true`。`nil` 和 `false` 都会使条件为假；它们统称为假值（false values）。其他任何值都会使条件为真。

- **false values**（假值）：条件判断时只有 `nil` 和 `false` 为假，数字 0 和空字符串都为真。

Despite its name, false is frequently used as an alternative to nil, with the key difference that false behaves like a regular value in a table, while a nil in a table represents an absent key.
尽管名为 `false`，但它经常被用作 `nil` 的替代，关键区别在于 `false` 在表中表现得像一个普通值，而表中的 `nil` 表示一个不存在的键。

- table 中 nil 的含义：设置 `t[k] = nil` 会删除键 `k`（见 `ltable.c` 中 `luaH_setnil`）。

The type number represents both integer numbers and real (floating-point) numbers, using two subtypes: integer and float.
类型 `number` 使用两个子类型表示整数和实数（浮点数）：`integer` 和 `float`。

- 源码实现：`lobject.h` 中 `TValue` 通过 `n`（`lua_Number`，即 double）或 `i`（`lua_Integer`，即 long long）存储。

Standard Lua uses 64-bit integers and double-precision (64-bit) floats, but you can also compile Lua so that it uses 32-bit integers and/or single-precision (32-bit) floats. The option with 32 bits for both integers and floats is particularly attractive for small machines and embedded systems. (See macro LUA_32BITS in file luaconf.h.)
标准 Lua 使用 64 位整数和双精度（64 位）浮点数，但也可以编译 Lua 使其使用 32 位整数和/或单精度（32 位）浮点数。对于小型机器和嵌入式系统，整数和浮点数都使用 32 位的选项尤其具有吸引力。（参见文件 `luaconf.h` 中的宏 `LUA_32BITS`。）

Unless stated otherwise, any overflow when manipulating integer values wrap around, according to the usual rules of two-complement arithmetic.
除非另有说明，操作整数值时的任何溢出都会回绕（wrap around），按照补码运算的通常规则。

- **wrap around**（回绕）：整数溢出不按错误处理，而是直接截断为补码表示（见 `lvm.c` 中整数运算实现）。

(In other words, the actual result is the unique representable integer that is equal modulo 2^n to the mathematical result, where n is the number of bits of the integer type.)
（换句话说，实际结果是唯一可表示的整数，它等于数学结果对 2^n 取模，其中 n 是整数类型的位数。）

Lua has explicit rules about when each subtype is used, but it also converts between them automatically as needed (see §2.2). Therefore, the programmer may choose to mostly ignore the difference between integers and floats or to assume complete control over the representation of each number.
Lua 对何时使用每个子类型有明确的规则，但也会根据需要自动在它们之间转换（见 §2.2）。因此，程序员可以选择大多忽略整数和浮点数之间的区别，或者完全控制每个数字的表示方式。

The type string represents immutable sequences of bytes.
类型 `string` 表示不可变的字节序列。

- **immutable**（不可变）：字符串一旦创建就不能修改，修改操作会创建新字符串（见 `lstring.c`）。

Lua is 8-bit clean: strings can contain any 8-bit value, including embedded zeros ('\0').
Lua 是 8-bit clean 的：字符串可以包含任何 8 位值，包括嵌入的零（`'\0'`）。

- 8-bit clean：字符串内部用长度+指针表示，不以 `\0` 结尾（见 `TString` 结构，`lstring.h`）。

Lua is also encoding-agnostic; it makes no assumptions about the contents of a string. The length of any string in Lua must fit in a Lua integer.
Lua 也是编码无关的，它对字符串的内容不做任何假设。Lua 中任何字符串的长度必须能放入一个 Lua 整数。

Lua can call (and manipulate) functions written in Lua and functions written in C (see §3.4.10). Both are represented by the type function.
Lua 可以调用（并操作）用 Lua 编写的函数和用 C 编写的函数（见 §3.4.10）。两者都用类型 `function` 表示。

- C 函数：原型为 `int (*)(lua_State *)`，通过 `lua_CFunction` 类型定义（见 `lua.h`）。

The type userdata is provided to allow arbitrary C data to be stored in Lua variables. A userdata value represents a block of raw memory.
类型 `userdata` 用于允许任意的 C 数据存储在 Lua 变量中。一个 userdata 值表示一块原始内存。

- **userdata**：分为 full userdata（内存由 Lua 管理，见 `lua_newuserdata`）和 light userdata（仅 C 指针，见 `lua_pushlightuserdata`）。

There are two kinds of userdata: full userdata, which is an object with a block of memory managed by Lua, and light userdata, which is simply a C pointer value.
userdata 有两种：full userdata（完整用户数据），是由 Lua 管理内存的对象；和 light userdata（轻量用户数据），它仅仅是一个 C 指针值。

Userdata has no predefined operations in Lua, except assignment and identity test. By using metatables, the programmer can define operations for full userdata values (see §2.4). Userdata values cannot be created or modified in Lua, only through the C API. This guarantees the integrity of data owned by the host program and C libraries.
Lua 中 userdata 没有预定义的操作，只有赋值和恒等性测试。通过使用元表，程序员可以为 full userdata 定义操作（见 §2.4）。Userdata 值不能在 Lua 中创建或修改，只能通过 C API 进行。这保证了宿主程序和 C 库所拥有的数据的完整性。

- 源码保障：`lapi.c` 中 `lua_newuserdatauv` 分配内存并关联元表，Lua 脚本无法直接操作内部数据。

The type thread represents independent threads of execution and it is used to implement coroutines (see §2.6). Lua threads are not related to operating-system threads. Lua supports coroutines on all systems, even those that do not support threads natively.
类型 `thread` 表示独立的执行线程，用于实现协程（见 §2.6）。Lua 线程与操作系统线程无关。Lua 在所有系统上都支持协程，甚至包括那些原生不支持线程的系统。

- **thread**：对应 `lua_State` 结构，每个协程有独立的栈（见 `lstate.h`）。

The type table implements associative arrays, that is, arrays that can have as indices not only numbers, but any Lua value except nil and NaN.
类型 `table` 实现关联数组，即不仅可以使用数字作为索引，还可以使用除 `nil` 和 `NaN` 之外的任何 Lua 值作为索引的数组。

- **table**：核心数据结构，见 `ltable.c` / `ltable.h` 中 `Table` 结构的哈希表+数组混合实现。

(Not a Number is a special floating-point value used by the IEEE 754 standard to represent undefined numerical results, such as 0/0.)
（Not a Number 是 IEEE 754 标准使用的特殊浮点值，用于表示未定义的数值结果，例如 `0/0`。）

Tables can be heterogeneous; that is, they can contain values of all types (except nil). Any key associated to the value nil is not considered part of the table. Conversely, any key that is not part of a table has an associated value nil.
表可以是异构的（heterogeneous）；也就是说，它们可以包含所有类型的值（除了 `nil`）。任何与值 `nil` 关联的键都不被视为表的一部分。反之，任何不属于表的键都有关联的值 `nil`。

Tables are the sole data-structuring mechanism in Lua; they can be used to represent ordinary arrays, lists, symbol tables, sets, records, graphs, trees, etc.
表是 Lua 中唯一的数据结构机制；它们可以用来表示普通数组、列表、符号表、集合、记录、图、树等。

To represent records, Lua uses the field name as an index. The language supports this representation by providing a.name as syntactic sugar for a["name"].
为了表示记录（records），Lua 使用字段名作为索引。语言通过提供 `a.name` 作为 `a["name"]` 的语法糖来支持这种表示。

There are several convenient ways to create tables in Lua (see §3.4.9).
Lua 中有几种方便的方式来创建表（见 §3.4.9）。

Like indices, the values of table fields can be of any type. In particular, because functions are first-class values, table fields can contain functions. Thus tables can also carry methods (see §3.4.11).
与索引一样，表字段的值可以是任何类型。特别地，由于函数是一等值，表字段可以包含函数。因此表也可以携带方法（methods）（见 §3.4.11）。

The indexing of tables follows the definition of raw equality in the language. The expressions a[i] and a[j] denote the same table element if and only if i and j are raw equal (that is, equal without metamethods).
表的索引遵循语言中原始相等的定义。表达式 `a[i]` 和 `a[j]` 表示同一个表元素，当且仅当 `i` 和 `j` 是原始相等的（即不调用元方法的情况下相等）。

- **raw equal**：直接比较值，不触发 `__eq` 元方法（见 `lobject.c` 中 `luaV_rawequalobj`）。

In particular, floats with integral values are equal to their respective integers (e.g., 1.0 == 1). To avoid ambiguities, any float used as a key that is equal to an integer is converted to that integer. For instance, if you write a[2.0] = true, the actual key inserted into the table will be the integer 2.
特别地，具有整数值的浮点数与其对应的整数相等（例如 `1.0 == 1`）。为了避免歧义，任何用作键且等于整数的浮点数都会被转换为该整数。例如，如果你写 `a[2.0] = true`，实际插入表的键将是整数 `2`。

- 源码行为：`ltable.c` 中 `luaH_set` 会将浮点键转换为整数键存储。

Tables, functions, threads, and (full) userdata values are objects: variables do not actually contain these values, only references to them. Assignment, parameter passing, and function returns always manipulate references to such values; these operations do not imply any kind of copy.
表、函数、线程和（full）userdata 值是**对象**（objects）：变量实际上并不**包含**这些值，只包含对它们的**引用**（references）。赋值、参数传递和函数返回总是操作对这些值的引用；这些操作不意味着任何复制。

- **GC 对象**：这些类型在源码中都是 `GCObject`（见 `lstate.h`），由垃圾收集器管理生命周期。

The library function type returns a string describing the type of a given value (see type).
库函数 `type` 返回一个描述给定值类型的字符串（见 `type`）。

## 2.2 – Environments and the Global Environment（环境与全局环境）

As we will discuss further in §3.2 and §3.3.3, any reference to a free name (that is, a name not bound to any declaration) var is syntactically translated to _ENV.var.
如我们将在 §3.2 和 §3.3.3 中进一步讨论的，对自由名称（即未绑定到任何声明的名称）`var` 的任何引用在语法上被转换为 `_ENV.var`。

- **free name**（自由名称）：未在局部声明的名称，默认从 `_ENV` 查找。

Moreover, every chunk is compiled in the scope of an external local variable named _ENV (see §3.3.2), so _ENV itself is never a free name in a chunk.
此外，每个代码块都在一个名为 `_ENV` 的外部局部变量的作用域内编译（见 §3.3.2），因此 `_ENV` 本身在一个代码块中永远不会是自由名称。

Despite the existence of this external _ENV variable and the translation of free names, _ENV is a completely regular name. In particular, you can define new variables and parameters with that name.
尽管存在这个外部 `_ENV` 变量和自由名称的转换，`_ENV` 仍然是一个完全普通的名称。特别地，你可以用这个名称定义新的变量和参数。

Each reference to a free name uses the _ENV that is visible at that point in the program, following the usual visibility rules of Lua (see §3.5).
每次对自由名称的引用都使用程序中该点可见的 `_ENV`，遵循 Lua 通常的可见性规则（见 §3.5）。

Any table used as the value of _ENV is called an environment.
任何用作 `_ENV` 值的表都称为**环境**（environment）。

Lua keeps a distinguished environment called the global environment. This value is kept at a special index in the C registry (see §4.5).
Lua 维护一个特殊的环境，称为**全局环境**（global environment）。该值保存在 C 注册表的一个特殊索引中（见 §4.5）。

- **C registry**（注册表）：一个特殊的 Lua 表，供 C 代码存储数据，全局环境保存在 `LUA_RIDX_GLOBALS` 索引处（见 `lua.h`）。

In Lua, the global variable _G is initialized with this same value. (_G is never used internally, so changing its value will affect only your own code.)
在 Lua 中，全局变量 `_G` 被初始化为相同的值。（`_G` 从不被内部使用，因此更改其值只会影响你自己的代码。）

When Lua loads a chunk, the default value for its _ENV variable is the global environment (see load). Therefore, by default, free names in Lua code refer to entries in the global environment and, therefore, they are also called global variables.
当 Lua 加载一个代码块时，其 `_ENV` 变量的默认值是全局环境（见 `load`）。因此，默认情况下，Lua 代码中的自由名称引用全局环境中的条目，因此它们也被称为**全局变量**（global variables）。

Moreover, all standard libraries are loaded in the global environment and some functions there operate on that environment. You can use load (or loadfile) to load a chunk with a different environment.
此外，所有标准库都加载在全局环境中，其中一些函数操作该环境。你可以使用 `load`（或 `loadfile`）加载具有不同环境的代码块。

(In C, you have to load the chunk and then change the value of its first upvalue; see lua_setupvalue.)
（在 C 中，你必须先加载代码块，然后更改其第一个上值；参见 `lua_setupvalue`。）

## 2.3 – Error Handling（错误处理）

Several operations in Lua can raise an error. An error interrupts the normal flow of the program, which can continue by catching the error.
Lua 中的多个操作可能会**引发**（raise）错误。错误会中断程序的正常流程，程序可以通过**捕获**（catching）错误来继续。

Lua code can explicitly raise an error by calling the error function. (This function never returns.)
Lua 代码可以通过调用 `error` 函数显式地引发错误。（此函数永不返回。）

- `error` 实现：内部调用 `luaD_throw`（见 `ldo.c`），使用 `longjmp` 展开栈。

To catch errors in Lua, you can do a protected call, using pcall (or xpcall). The function pcall calls a given function in protected mode. Any error while running the function stops its execution, and control returns immediately to pcall, which returns a status code.
要在 Lua 中捕获错误，你可以执行一次**受保护调用**（protected call），使用 `pcall`（或 `xpcall`）。函数 `pcall` 在**受保护模式**（protected mode）下调用给定函数。运行该函数时的任何错误都会停止其执行，控制立即返回到 `pcall`，`pcall` 返回一个状态码。

- **protected mode**：`luaD_rawrunprotected`（见 `ldo.c`）建立保护帧，捕获 `longjmp`。

Because Lua is an embedded extension language, Lua code starts running by a call from C code in the host program. (When you use Lua standalone, the lua application is the host program.)
由于 Lua 是嵌入式扩展语言，Lua 代码通过宿主程序中 C 代码的调用来开始运行。（当你独立使用 Lua 时，`lua` 应用程序就是宿主程序。）

Usually, this call is protected; so, when an otherwise unprotected error occurs during the compilation or execution of a Lua chunk, control returns to the host, which can take appropriate measures, such as printing an error message.
通常，这个调用是受保护的；因此，当在编译或执行 Lua 代码块期间发生原本未受保护的错误时，控制会返回给宿主，宿主可以采取适当的措施，例如打印错误消息。

Whenever there is an error, an error object is propagated with information about the error. Lua itself only generates errors whose error object is a string, but programs can generate errors with any value as the error object.
每当发生错误时，一个**错误对象**（error object）会被传播，携带有关错误的信息。Lua 本身只生成错误对象为字符串的错误，但程序可以用任何值作为错误对象来生成错误。

It is up to the Lua program or its host to handle such error objects. For historical reasons, an error object is often called an error message, even though it does not have to be a string.
由 Lua 程序或其宿主来处理这些错误对象。由于历史原因，错误对象常被称为**错误消息**（error message），即使它不必是字符串。

When you use xpcall (or lua_pcall, in C) you can give a message handler to be called in case of errors. This function is called with the original error object and returns a new error object. It is called before the error unwinds the stack, so that it can gather more information about the error, for instance by inspecting the stack and creating a stack traceback.
当你使用 `xpcall`（或在 C 中使用 `lua_pcall`）时，你可以指定一个**消息处理函数**（message handler），在发生错误时被调用。该函数以原始错误对象作为参数，并返回一个新的错误对象。它在错误展开栈之前被调用，因此它可以收集有关错误的更多信息，例如通过检查栈并创建栈回溯。

- `lua_pcall`：最后一个参数 `msgh` 是消息处理函数在栈中的索引（见 `lapi.c`）。

This message handler is still protected by the protected call; so, an error inside the message handler will call the message handler again. If this loop goes on for too long, Lua breaks it and returns an appropriate message.
此消息处理函数仍然受受保护调用的保护；因此，消息处理函数内部的错误会再次调用消息处理函数。如果这个循环持续太久，Lua 会中断它并返回一条适当的消息。

The message handler is called only for regular runtime errors. It is not called for memory-allocation errors nor for errors while running finalizers or other message handlers.
消息处理函数只针对常规运行时错误被调用。它不会被调用于内存分配错误，也不会在运行终结器或其他消息处理函数时的错误。

Lua also offers a system of warnings (see warn). Unlike errors, warnings do not interfere in any way with program execution. They typically only generate a message to the user, although this behavior can be adapted from C (see lua_setwarnf).
Lua 还提供了一套**警告**系统（见 `warn`）。与错误不同，警告不会以任何方式干扰程序的执行。它们通常只向用户生成一条消息，尽管这种行为可以从 C 中调整（见 `lua_setwarnf`）。


## 2.4 – Metatables and Metamethods（元表与元方法）

Every value in Lua can have a metatable.
Lua 中的每个值都可以有一个**元表**（metatable）。

- **metatable**（元表）：一个普通的 Lua 表，定义值在特定事件下的行为。通过 `lua_getmetatable` / `lua_setmetatable` 操作（见 `lapi.c`）。

This metatable is an ordinary Lua table that defines the behavior of the original value under certain events.
这个**元表**是一个普通的 Lua 表，它定义了原始值在特定事件下的行为。

You can change several aspects of the behavior of a value by setting specific fields in its metatable.
你可以在元表中设置特定字段来改变值的多个行为方面。

For instance, when a non-numeric value is the operand of an addition, Lua checks for a function in the field __add of the value's metatable. If it finds one, Lua calls this function to perform the addition.
例如，当一个非数值是加法操作数时，Lua 会检查该值元表中 `__add` 字段的函数。如果找到，Lua 会调用此函数来执行加法。

The key for each event in a metatable is a string with the event name prefixed by two underscores; the corresponding value is called a metavalue.
元表中每个事件的键是一个以两个下划线为前缀的事件名字符串；对应的值称为**元值**（metavalue）。

For most events, the metavalue must be a function, which is then called a metamethod.
对于大多数事件，元值必须是一个函数，此时它被称为**元方法**（metamethod）。

In the previous example, the key is the string "__add" and the metamethod is the function that performs the addition.
在前面的例子中，键是字符串 `"__add"`，元方法是执行加法的函数。

Unless stated otherwise, a metamethod can in fact be any callable value, which is either a function or a value with a __call metamethod.
除非另有说明，元方法实际上可以是任何**可调用值**（callable value），即函数或具有 `__call` 元方法的值。

You can query the metatable of any value using the getmetatable function.
你可以使用 `getmetatable` 函数查询任何值的元表。

Lua queries metamethods in metatables using a raw access (see rawget).
Lua 使用原始访问在元表中查询元方法（见 `rawget`）。

- **raw access**：直接读取表字段，不触发 `__index` 元方法（见 `ltable.c`）。

You can replace the metatable of tables using the setmetatable function.
你可以使用 `setmetatable` 函数替换表的元表。

You cannot change the metatable of other types from Lua code, except by using the debug library.
你不能从 Lua 代码中更改其他类型的元表，除非使用 debug 库。

Tables and full userdata have individual metatables, although multiple tables and userdata can share their metatables.
表和 full userdata 有独立的元表，尽管多个表和 userdata 可以共享它们的元表。

Values of all other types share one single metatable per type; that is, there is one single metatable for all numbers, one for all strings, etc.
所有其他类型的值按类型共享一个元表；也就是说，所有数字共享一个元表，所有字符串共享一个元表，等等。

By default, a value has no metatable, but the string library sets a metatable for the string type (see §6.4).
默认情况下，值没有元表，但字符串库为字符串类型设置了元表（见 §6.4）。

A detailed list of operations controlled by metatables is given next. Each event is identified by its corresponding key. By convention, all metatable keys used by Lua are composed by two underscores followed by lowercase Latin letters.
下面详细列出了由元表控制的操作列表。每个事件由其对应的键标识。按照惯例，Lua 使用的所有元表键都由两个下划线后跟小写拉丁字母组成。

**__add**

The addition (+) operation. If any operand for an addition is not a number, Lua will try to call a metamethod. It starts by checking the first operand (even if it is a number); if that operand does not define a metamethod for __add, then Lua will check the second operand. If Lua can find a metamethod, it calls the metamethod with the two operands as arguments, and the result of the call (adjusted to one value) is the result of the operation. Otherwise, if no metamethod is found, Lua raises an error.
加法（`+`）操作。如果加法的任一操作数不是数字，Lua 将尝试调用元方法。它首先检查第一个操作数（即使它是数字）；如果该操作数没有为 `__add` 定义元方法，那么 Lua 将检查第二个操作数。如果 Lua 能找到元方法，它会用两个操作数作为参数调用该元方法，调用的结果（调整为单个值）就是操作的结果。否则，如果没有找到元方法，Lua 会引发错误。

- **源码路径**：`ltm.c` 中 `luaT_callTM` 负责调用元方法，`lvm.c` 中执行算术运算时检查。

**__sub**

The subtraction (-) operation. Behavior similar to the addition operation.
减法（`-`）操作。行为类似于加法操作。

**__mul**

The multiplication (*) operation. Behavior similar to the addition operation.
乘法（`*`）操作。行为类似于加法操作。

**__div**

The division (/) operation. Behavior similar to the addition operation.
除法（`/`）操作。行为类似于加法操作。

**__mod**

The modulo (%) operation. Behavior similar to the addition operation.
取模（`%`）操作。行为类似于加法操作。

**__pow**

The exponentiation (^) operation. Behavior similar to the addition operation.
幂运算（`^`）操作。行为类似于加法操作。

**__unm**

The negation (unary -) operation. Behavior similar to the addition operation.
取反（一元 `-`）操作。行为类似于加法操作。

**__idiv**

The floor division (//) operation. Behavior similar to the addition operation.
向下取整除法（`//`）操作。行为类似于加法操作。

**__band**

The bitwise AND (&) operation. Behavior similar to the addition operation, except that Lua will try a metamethod if any operand is neither an integer nor a float coercible to an integer (see §2.2).
按位与（`&`）操作。行为类似于加法操作，区别在于 Lua 只有在任一操作数既不是整数也不是可强制转换为整数的浮点数时才会尝试元方法（见 §2.2）。

**__bor**

The bitwise OR (|) operation. Behavior similar to the bitwise AND operation.
按位或（`|`）操作。行为类似于按位与操作。

**__bxor**

The bitwise exclusive OR (binary ~) operation. Behavior similar to the bitwise AND operation.
按位异或（二元 `~`）操作。行为类似于按位与操作。

**__bnot**

The bitwise NOT (unary ~) operation. Behavior similar to the bitwise AND operation.
按位非（一元 `~`）操作。行为类似于按位与操作。

**__shl**

The bitwise left shift (<<) operation. Behavior similar to the bitwise AND operation.
按位左移（`<<`）操作。行为类似于按位与操作。

**__shr**

The bitwise right shift (>>) operation. Behavior similar to the bitwise AND operation.
按位右移（`>>`）操作。行为类似于按位与操作。

**__concat**

The concatenation (..) operation. Behavior similar to the addition operation, except that Lua will try a metamethod if any operand is neither a string nor a number (which is always coercible to a string).
连接（`..`）操作。行为类似于加法操作，区别在于 Lua 只有在任一操作数既不是字符串也不是数字（数字总是可以强制转换为字符串）时才会尝试元方法。

**__len**

The length (#) operation. If the object is not a string, Lua will try its metamethod. If there is a metamethod, Lua calls it with the object as argument, and the result of the call (always adjusted to one value) is the result of the operation. If there is no metamethod but the object is a table, then Lua uses the table length operation (see §3.4.7). Otherwise, Lua raises an error.
长度（`#`）操作。如果对象不是字符串，Lua 将尝试其元方法。如果有元方法，Lua 用对象作为参数调用它，调用的结果（总是调整为单个值）就是操作的结果。如果没有元方法但对象是表，那么 Lua 使用表长度操作（见 §3.4.7）。否则，Lua 会引发错误。

**__eq**

The equal (==) operation. Behavior similar to the addition operation, except that Lua will try a metamethod only when the values being compared are either both tables or both full userdata and they are not primitively equal. The result of the call is always converted to a boolean.
等于（`==`）操作。行为类似于加法操作，区别在于 Lua 只有在被比较的值都是表或都是 full userdata 且它们不是原始相等时才会尝试元方法。调用的结果总是被转换为布尔值。

**__lt**

The less than (<) operation. Behavior similar to the addition operation, except that Lua will try a metamethod only when the values being compared are neither both numbers nor both strings. Moreover, the result of the call is always converted to a boolean.
小于（`<`）操作。行为类似于加法操作，区别在于 Lua 只有在被比较的值不都是数字也不都是字符串时才会尝试元方法。此外，调用的结果总是被转换为布尔值。

**__le**

The less equal (<=) operation. Behavior similar to the less than operation.
小于等于（`<=`）操作。行为类似于小于操作。

**__index**

The indexing access operation table[key]. This event happens when table is not a table or when key is not present in table. The metavalue is looked up in the metatable of table.
索引访问操作 `table[key]`。当 `table` 不是表或 `key` 不在 `table` 中时，触发此事件。元值在 `table` 的元表中查找。

The metavalue for this event can be either a function, a table, or any value with an __index metavalue. If it is a function, it is called with table and key as arguments, and the result of the call (adjusted to one value) is the result of the operation. Otherwise, the final result is the result of indexing this metavalue with key. This indexing is regular, not raw, and therefore can trigger another __index metavalue.
此事件的元值可以是函数、表或任何具有 `__index` 元方法的值。如果是函数，它会以 `table` 和 `key` 作为参数被调用，调用的结果（调整为单个值）就是操作的结果。否则，最终结果是用 `key` 索引此元值的结果。此索引是常规的，不是原始的，因此可以触发另一个 `__index` 元方法。

- **递归索引**：`__index` 为元表时会递归查找，这也是实现继承的基础机制。

**__newindex**

The indexing assignment table[key] = value. Like the index event, this event happens when table is not a table or when key is not present in table. The metavalue is looked up in the metatable of table.
索引赋值 `table[key] = value`。与索引事件一样，当 `table` 不是表或 `key` 不在 `table` 中时触发此事件。元值在 `table` 的元表中查找。

Like with indexing, the metavalue for this event can be either a function, a table, or any value with an __newindex metavalue. If it is a function, it is called with table, key, and value as arguments. Otherwise, Lua repeats the indexing assignment over this metavalue with the same key and value. This assignment is regular, not raw, and therefore can trigger another __newindex metavalue.
与索引一样，此事件的元值可以是函数、表或任何具有 `__newindex` 元方法的值。如果是函数，它会以 `table`、`key` 和 `value` 作为参数被调用。否则，Lua 在此元值上重复执行相同的键和值的索引赋值。此赋值是常规的，不是原始的，因此可以触发另一个 `__newindex` 元方法。

Whenever a __newindex metavalue is invoked, Lua does not perform the primitive assignment. If needed, the metamethod itself can call rawset to do the assignment.
每当调用 `__newindex` 元方法时，Lua 不执行原始赋值。如果需要，元方法本身可以调用 `rawset` 来完成赋值。

**__call**

The call operation func(args). This event happens when Lua tries to call a non-function value (that is, func is not a function). The metamethod is looked up in func.
调用操作 `func(args)`。当 Lua 尝试调用一个非函数值时触发此事件（即 `func` 不是函数）。元方法在 `func` 中查找。

If present, the metamethod is called with func as its first argument, followed by the arguments of the original call (args). All results of the call are the results of the operation. This is the only metamethod that allows multiple results.
如果存在，元方法以 `func` 作为其第一个参数被调用，后跟原始调用的参数（`args`）。调用的所有结果就是操作的结果。这是唯一允许多个结果的元方法。

In addition to the previous list, the interpreter also respects the following keys in metatables: __gc (see §2.5.3), __close (see §2.5.4), __mode (see §2.5.5), and __name.
除了上述列表之外，解释器还尊重元表中的以下键：`__gc`（见 §2.5.3）、`__close`（见 §2.5.4）、`__mode`（见 §2.5.5）和 `__name`。

(The entry __name, when it contains a string, may be used by tostring and in error messages.)
（当 `__name` 包含字符串时，它可能被 `tostring` 和错误消息使用。）

For the unary operators (negation, length, and bitwise NOT), the metamethod is computed and called with a dummy second operand, equal to the first one. This extra operand is only to simplify Lua's internals (by making these operators behave like a binary operation) and may be removed in future versions. For most uses this extra operand is irrelevant.
对于一元运算符（取反、长度和按位非），元方法以第二个虚拟操作数计算和调用，该操作数等于第一个操作数。这个额外的操作数只是为了简化 Lua 的内部实现（使这些运算符表现得像二元运算），并可能在未来的版本中移除。对于大多数用途，这个额外的操作数无关紧要。

Because metatables are regular tables, they can contain arbitrary fields, not only the event names defined above. Some functions in the standard library (e.g., tostring) use other fields in metatables for their own purposes.
因为元表是普通的表，它们可以包含任意字段，不仅限于上述定义的事件名。标准库中的一些函数（例如 `tostring`）使用元表中的其他字段来实现自己的目的。

It is a good practice to add all needed metamethods to a table before setting it as a metatable of some object. In particular, the __gc metamethod works only when this order is followed (see §2.5.3). It is also a good practice to set the metatable of an object right after its creation.
在将表设置为某个对象的元表之前，最好将所有需要的元方法添加到该表中。特别地，`__gc` 元方法只有在遵循此顺序时才有效（见 §2.5.3）。在创建对象后立即设置其元表也是一个好习惯。

## 2.5 – Garbage Collection（垃圾回收）

Lua performs automatic memory management. This means that you do not have to worry about allocating memory for new objects or freeing it when the objects are no longer needed.
Lua 执行自动内存管理。这意味着你不必担心为新对象分配内存或在对象不再需要时释放内存。

Lua manages memory automatically by running a garbage collector to collect all dead objects. All memory used by Lua is subject to automatic management: strings, tables, userdata, functions, threads, internal structures, etc.
Lua 通过运行一个**垃圾收集器**（garbage collector）来自动管理内存，以收集所有**死亡**（dead）对象。Lua 使用的所有内存都受自动管理：字符串、表、userdata、函数、线程、内部结构等。

- **自动内存管理**：`lgc.c` 实现完整的标记-清除/分代 GC 算法，无需手动 `free`。

An object is considered dead as soon as the collector can be sure the object will not be accessed again in the normal execution of the program.
当收集器能够确定对象在程序的正常执行中不会再被访问时，该对象就被认为是**死亡**的。

("Normal execution" here excludes finalizers, which can resurrect dead objects (see §2.5.3), and excludes also operations using the debug library.)
（这里的 "正常执行" 不包括终结器，它可以复活死亡对象（见 §2.5.3），也不包括使用 debug 库的操作。）

Note that the time when the collector can be sure that an object is dead may not coincide with the programmer's expectations. The only guarantees are that Lua will not collect an object that may still be accessed in the normal execution of the program, and it will eventually collect an object that is inaccessible from Lua.
请注意，收集器能够确定对象死亡的时间可能与程序员的预期不一致。唯一的保证是 Lua 不会收集在程序正常执行中仍可能被访问的对象，并且最终会收集从 Lua 不可访问的对象。

(Here, inaccessible from Lua means that neither a variable nor another live object refer to the object.)
（这里，从 Lua 不可访问意味着没有变量或其他存活对象引用该对象。）

Because Lua has no knowledge about C code, it never collects objects accessible through the registry (see §4.5), which includes the global environment (see §2.2).
由于 Lua 对 C 代码一无所知，它永远不会收集可通过注册表访问的对象（见 §4.5），包括全局环境（见 §2.2）。

The garbage collector (GC) in Lua can work in two modes: incremental and generational.
Lua 中的垃圾收集器（GC）可以工作在两种模式下：增量（incremental）和分代（generational）。

The default GC mode with the default parameters are adequate for most uses. However, programs that waste a large proportion of their time allocating and freeing memory can benefit from other settings.
默认的 GC 模式及默认参数对于大多数用途都是足够的。然而，花费大量时间分配和释放内存的程序可以从其他设置中受益。

Keep in mind that the GC behavior is non-portable both across platforms and across different Lua releases; therefore, optimal settings are also non-portable.
请记住，GC 行为是不可移植的，既不可跨平台移植，也不可跨不同的 Lua 版本移植；因此，最优设置也是不可移植的。

You can change the GC mode and parameters by calling lua_gc (in C) or collectgarbage in Lua. You can also use these functions to control the collector directly (e.g., to stop and restart it).
你可以通过调用 `lua_gc`（在 C 中）或 `collectgarbage`（在 Lua 中）来更改 GC 模式和参数。你还可以使用这些函数直接控制收集器（例如停止和重新启动它）。

- **GC 控制**：`lua_gc` 支持 `LUA_GCSTOP`、`LUA_GCRESTART`、`LUA_GCCOLLECT` 等命令（见 `lua.h`）。

### 2.5.1 – Incremental Garbage Collection（增量垃圾回收）

In incremental mode, each GC cycle performs a mark-and-sweep collection in small steps interleaved with the program's execution.
在增量模式下，每个 GC 周期以小步骤执行标记-清除收集，与程序的执行交错进行。

- **mark-and-sweep**（标记-清除）：GC 先标记所有可达对象，然后清除未标记的对象。

In this mode, the collector uses three numbers to control its garbage-collection cycles: the garbage-collector pause, the garbage-collector step multiplier, and the garbage-collector step size.
在此模式下，收集器使用三个数字来控制其垃圾回收周期：**垃圾收集器暂停**（garbage-collector pause）、**垃圾收集器步进乘数**（garbage-collector step multiplier）和**垃圾收集器步进大小**（garbage-collector step size）。

The garbage-collector pause controls how long the collector waits before starting a new cycle. The collector starts a new cycle when the use of memory hits n% of the use after the previous collection. Larger values make the collector less aggressive. Values equal to or less than 100 mean the collector will not wait to start a new cycle. A value of 200 means that the collector waits for the total memory in use to double before starting a new cycle. The default value is 200; the maximum value is 1000.
垃圾收集器暂停控制收集器在开始新周期之前等待多长时间。当内存使用量达到上一次收集后使用量的 n% 时，收集器开始新周期。较大的值使收集器不那么激进。小于或等于 100 的值意味着收集器不会等待就开始新周期。200 的值意味着收集器等待总内存使用量翻倍后才开始新周期。默认值是 200；最大值是 1000。

The garbage-collector step multiplier controls the speed of the collector relative to memory allocation, that is, how many elements it marks or sweeps for each kilobyte of memory allocated. Larger values make the collector more aggressive but also increase the size of each incremental step. You should not use values less than 100, because they make the collector too slow and can result in the collector never finishing a cycle. The default value is 100; the maximum value is 1000.
垃圾收集器步进乘数控制收集器相对于内存分配的速度，也就是说，每分配一千字节内存，它标记或清除多少元素。较大的值使收集器更激进，但也增加了每个增量步骤的大小。你不应使用小于 100 的值，因为它们使收集器太慢，可能导致收集器永远无法完成一个周期。默认值是 100；最大值是 1000。

The garbage-collector step size controls the size of each incremental step, specifically how many bytes the interpreter allocates before performing a step. This parameter is logarithmic: A value of n means the interpreter will allocate 2^n bytes between steps and perform equivalent work during the step. A large value (e.g., 60) makes the collector a stop-the-world (non-incremental) collector. The default value is 13, which means steps of approximately 8 Kbytes.
垃圾收集器步进大小控制每个增量步骤的大小，特别是解释器在执行步骤之前分配多少字节。此参数是对数级的：值 n 意味着解释器将在步骤之间分配 2^n 字节，并在步骤期间执行等效的工作。较大的值（例如 60）使收集器成为 stop-the-world（非增量）收集器。默认值是 13，这意味着步骤大约为 8 KB。

### 2.5.2 – Generational Garbage Collection（分代垃圾回收）

In generational mode, the collector does frequent minor collections, which traverses only objects recently created. If after a minor collection the use of memory is still above a limit, the collector does a stop-the-world major collection, which traverses all objects.
在分代模式下，收集器执行频繁的**小型**（minor）收集，只遍历最近创建的对象。如果在小型收集后内存使用量仍高于限制，收集器会执行一次 stop-the-world 的**大型**（major）收集，遍历所有对象。

The generational mode uses two parameters: the minor multiplier and the major multiplier.
分代模式使用两个参数：**小型乘数**（minor multiplier）和**大型乘数**（major multiplier）。

The minor multiplier controls the frequency of minor collections. For a minor multiplier x, a new minor collection will be done when memory grows x% larger than the memory in use after the previous major collection. For instance, for a multiplier of 20, the collector will do a minor collection when the use of memory gets 20% larger than the use after the previous major collection. The default value is 20; the maximum value is 200.
小型乘数控制小型收集的频率。对于小型乘数 x，当内存增长量比上一次大型收集后的内存使用量高出 x% 时，会执行新的小型收集。例如，乘数为 20 时，当内存使用量比上一次大型收集后的使用量高出 20% 时，收集器会执行一次小型收集。默认值是 20；最大值是 200。

The major multiplier controls the frequency of major collections. For a major multiplier x, a new major collection will be done when memory grows x% larger than the memory in use after the previous major collection. For instance, for a multiplier of 100, the collector will do a major collection when the use of memory gets larger than twice the use after the previous collection. The default value is 100; the maximum value is 1000.
大型乘数控制大型收集的频率。对于大型乘数 x，当内存增长量比上一次大型收集后的内存使用量高出 x% 时，会执行新的大型收集。例如，乘数为 100 时，当内存使用量比上一次收集后的使用量高出一倍时，收集器会执行一次大型收集。默认值是 100；最大值是 1000。

- **分代 GC 源码**：`lgc.c` 中 `genstep` 控制小型/大型收集的触发逻辑。

### 2.5.3 – Garbage-Collection Metamethods（垃圾回收元方法 / 终结器）

You can set garbage-collector metamethods for tables and, using the C API, for full userdata (see §2.4). These metamethods, called finalizers, are called when the garbage collector detects that the corresponding table or userdata is dead.
你可以为表设置垃圾收集器元方法，并且通过 C API 为 full userdata 设置（见 §2.4）。这些元方法称为**终结器**（finalizers），当垃圾收集器检测到相应的表或 userdata 死亡时被调用。

- **finalizer**（终结器）：在对象被 GC 回收前执行清理操作，类似 C++ 的析构函数。

Finalizers allow you to coordinate Lua's garbage collection with external resource management such as closing files, network or database connections, or freeing your own memory.
终结器允许你协调 Lua 的垃圾回收与外部资源管理，例如关闭文件、网络或数据库连接，或释放你自己的内存。

For an object (table or userdata) to be finalized when collected, you must mark it for finalization.
要使一个对象（表或 userdata）在收集时被终结，你必须**标记**它以供终结。

You mark an object for finalization when you set its metatable and the metatable has a __gc metamethod. Note that if you set a metatable without a __gc field and later create that field in the metatable, the object will not be marked for finalization.
你在设置其元表且元表具有 `__gc` 元方法时标记一个对象以供终结。请注意，如果你设置了一个没有 `__gc` 字段的元表，稍后才在元表中创建该字段，该对象将不会被标记以供终结。

- **标记时机**：`lapi.c` 中 `lua_setmetatable` 会检查元表是否有 `__gc`，若有则调用 `luaC_checkfinalizer`。

When a marked object becomes dead, it is not collected immediately by the garbage collector. Instead, Lua puts it in a list. After the collection, Lua goes through that list. For each object in the list, it checks the object's __gc metamethod: If it is present, Lua calls it with the object as its single argument.
当一个被标记的对象死亡时，它不会被垃圾收集器立即收集。相反，Lua 将其放入一个列表中。收集完成后，Lua 遍历该列表。对于列表中的每个对象，它检查对象的 `__gc` 元方法：如果存在，Lua 用对象作为其唯一参数调用它。

At the end of each garbage-collection cycle, the finalizers are called in the reverse order that the objects were marked for finalization, among those collected in that cycle; that is, the first finalizer to be called is the one associated with the object marked last in the program.
在每个垃圾回收周期结束时，终结器按照对象被标记以供终结的相反顺序被调用，只限于该周期中收集的那些对象；也就是说，第一个被调用的终结器是程序中最后标记的对象关联的终结器。

The execution of each finalizer may occur at any point during the execution of the regular code.
每个终结器的执行可能发生在常规代码执行期间的任何时刻。

Because the object being collected must still be used by the finalizer, that object (and other objects accessible only through it) must be resurrected by Lua.
因为被收集的对象仍然必须被终结器使用，该对象（以及仅通过它可访问的其他对象）必须由 Lua **复活**（resurrected）。

Usually, this resurrection is transient, and the object memory is freed in the next garbage-collection cycle.
通常，这种复活是短暂的，对象内存在下一个垃圾回收周期中被释放。

However, if the finalizer stores the object in some global place (e.g., a global variable), then the resurrection is permanent.
然而，如果终结器将对象存储在某个全局位置（例如全局变量），那么复活是永久的。

Moreover, if the finalizer marks a finalizing object for finalization again, its finalizer will be called again in the next cycle where the object is dead.
此外，如果终结器再次标记一个正在终结的对象以供终结，其终结器将在下一个对象死亡的周期中再次被调用。

In any case, the object memory is freed only in a GC cycle where the object is dead and not marked for finalization.
无论如何，对象内存只在对象死亡且未被标记以供终结的 GC 周期中被释放。

When you close a state (see lua_close), Lua calls the finalizers of all objects marked for finalization, following the reverse order that they were marked.
当你关闭一个状态（见 `lua_close`）时，Lua 调用所有被标记以供终结的对象的终结器，按照它们被标记的相反顺序。

If any finalizer marks objects for collection during that phase, these marks have no effect.
如果任何终结器在该阶段标记对象以供收集，这些标记没有效果。

Finalizers cannot yield nor run the garbage collector.
终结器不能 yield 也不能运行垃圾收集器。

Because they can run in unpredictable times, it is good practice to restrict each finalizer to the minimum necessary to properly release its associated resource.
因为它们可能在不可预测的时间运行，最佳实践是将每个终结器限制在正确释放其关联资源所必需的最小范围内。

Any error while running a finalizer generates a warning; the error is not propagated.
运行终结器时的任何错误都会生成警告；错误不会被传播。

### 2.5.4 – Weak Tables（弱表）

A weak table is a table whose elements are weak references. A weak reference is ignored by the garbage collector. In other words, if the only references to an object are weak references, then the garbage collector will collect that object.
**弱表**（weak table）是其元素为**弱引用**（weak references）的表。弱引用被垃圾收集器忽略。换句话说，如果对对象的唯一引用是弱引用，那么垃圾收集器将收集该对象。

- **弱引用**：不影响对象的生命周期，对象可被正常 GC 回收。

A weak table can have weak keys, weak values, or both. A table with weak values allows the collection of its values, but prevents the collection of its keys. A table with both weak keys and weak values allows the collection of both keys and values. In any case, if either the key or the value is collected, the whole pair is removed from the table.
弱表可以有弱键、弱值，或两者都有。具有弱值的表允许收集其值，但阻止收集其键。具有弱键和弱值的表允许收集键和值。无论如何，如果键或值被收集，整个键值对都会从表中移除。

The weakness of a table is controlled by the __mode field of its metatable. This metavalue, if present, must be one of the following strings: "k", for a table with weak keys; "v", for a table with weak values; or "kv", for a table with both weak keys and values.
表的弱性由其元表的 `__mode` 字段控制。如果存在，此元值必须是以下字符串之一：`"k"`，表示具有弱键的表；`"v"`，表示具有弱值的表；或 `"kv"`，表示具有弱键和弱值的表。

- **__mode 实现**：`lgc.c` 中扫描表时会根据 `__mode` 决定是否追踪键/值的引用。

A table with weak keys and strong values is also called an ephemeron table. In an ephemeron table, a value is considered reachable only if its key is reachable. In particular, if the only reference to a key comes through its value, the pair is removed.
具有弱键和强值的表也称为 **ephemeron 表**。在 ephemeron 表中，值仅当其键可到达时才被认为是可到达的。特别地，如果对键的唯一引用来自其值，该键值对会被移除。

Any change in the weakness of a table may take effect only at the next collect cycle. In particular, if you change the weakness to a stronger mode, Lua may still collect some items from that table before the change takes effect.
表弱性的任何更改可能只在下一个收集周期生效。特别地，如果你将弱性更改为更强的模式，Lua 仍可能在更改生效之前从该表中收集一些条目。

Only objects that have an explicit construction are removed from weak tables. Values, such as numbers and light C functions, are not subject to garbage collection, and therefore are not removed from weak tables (unless their associated values are collected).
只有具有显式构造的对象才会从弱表中移除。值，例如数字和轻量 C 函数，不受垃圾收集影响，因此不会从弱表中移除（除非其关联的值被收集）。

Although strings are subject to garbage collection, they do not have an explicit construction and their equality is by value; they behave more like values than like objects. Therefore, they are not removed from weak tables.
尽管字符串受垃圾收集影响，但它们没有显式构造，且它们的相等性是按值的；它们的行为更像值而不是对象。因此，它们不会从弱表中移除。

Resurrected objects (that is, objects being finalized and objects accessible only through objects being finalized) have a special behavior in weak tables. They are removed from weak values before running their finalizers, but are removed from weak keys only in the next collection after running their finalizers, when such objects are actually freed.
复活的对象（即正在被终结的对象和仅通过正在被终结的对象可访问的对象）在弱表中有特殊的行为。它们会在运行其终结器之前从弱值中移除，但只有在运行其终结器之后的下一个收集中才从弱键中移除，此时这些对象被实际释放。

This behavior allows the finalizer to access properties associated with the object through weak tables.
这种行为允许终结器通过弱表访问与对象关联的属性。

If a weak table is among the resurrected objects in a collection cycle, it may not be properly cleared until the next cycle.
如果弱表在一次收集周期的复活对象中，它可能要到下一个周期才能被正确清除。

## 2.6 – Coroutines（协程）

Lua supports coroutines, also called collaborative multithreading.
Lua 支持协程，也称为**协作式多线程**（collaborative multithreading）。

- **协作式多线程**：协程主动让出（yield）执行权，而非被操作系统抢占。

A coroutine in Lua represents an independent thread of execution. Unlike threads in multithread systems, however, a coroutine only suspends its execution by explicitly calling a yield function.
Lua 中的协程表示一个独立的执行线程。然而，与多线程系统中的线程不同，协程只能通过显式调用 yield 函数来挂起其执行。

You create a coroutine by calling coroutine.create. Its sole argument is a function that is the main function of the coroutine.
你通过调用 `coroutine.create` 来创建协程。其唯一参数是协程的主函数。

The create function only creates a new coroutine and returns a handle to it (an object of type thread); it does not start the coroutine.
`create` 函数只创建一个新协程并返回一个句柄（类型为 `thread` 的对象）；它不会启动协程。

- **lua_newthread**：C API 对应函数，创建新 `lua_State` 并压入栈（见 `lapi.c`）。

You execute a coroutine by calling coroutine.resume. When you first call coroutine.resume, passing as its first argument a thread returned by coroutine.create, the coroutine starts its execution by calling its main function. Extra arguments passed to coroutine.resume are passed as arguments to that function.
你通过调用 `coroutine.resume` 来执行协程。当你第一次调用 `coroutine.resume` 时，将作为其第一个参数传递给 `coroutine.create` 返回的线程，协程通过调用其主函数来启动执行。传递给 `coroutine.resume` 的额外参数作为该函数的参数传递。

After the coroutine starts running, it runs until it terminates or yields.
协程开始运行后，它会一直运行直到终止或**让出**（yields）。

A coroutine can terminate its execution in two ways: normally, when its main function returns (explicitly or implicitly, after the last instruction); and abnormally, if there is an unprotected error.
协程可以通过两种方式终止其执行：正常地，当其主函数返回时（显式或隐式，在最后一条指令之后）；以及异常地，如果有未受保护的错误。

In case of normal termination, coroutine.resume returns true, plus any values returned by the coroutine main function.
在正常终止的情况下，`coroutine.resume` 返回 `true`，加上协程主函数返回的任何值。

In case of errors, coroutine.resume returns false plus the error object.
在错误的情况下，`coroutine.resume` 返回 `false` 加上错误对象。

In this case, the coroutine does not unwind its stack, so that it is possible to inspect it after the error with the debug API.
在这种情况下，协程不会展开其栈，因此可以在错误后使用 debug API 检查它。

- **错误处理**：`ldo.c` 中 `lua_resume` 使用 `luaD_rawrunprotected` 保护执行，错误时保留栈状态。

A coroutine yields by calling coroutine.yield. When a coroutine yields, the corresponding coroutine.resume returns immediately, even if the yield happens inside nested function calls (that is, not in the main function, but in a function directly or indirectly called by the main function).
协程通过调用 `coroutine.yield` 来让出。当协程让出时，对应的 `coroutine.resume` 立即返回，即使让出发生在嵌套函数调用中（也就是说，不在主函数中，而是在主函数直接或间接调用的函数中）。

In the case of a yield, coroutine.resume also returns true, plus any values passed to coroutine.yield.
在让出的情况下，`coroutine.resume` 也返回 `true`，加上传递给 `coroutine.yield` 的任何值。

The next time you resume the same coroutine, it continues its execution from the point where it yielded, with the call to coroutine.yield returning any extra arguments passed to coroutine.resume.
下次你恢复同一个协程时，它从让出点继续执行，`coroutine.yield` 调用返回传递给 `coroutine.resume` 的任何额外参数。

Like coroutine.create, the coroutine.wrap function also creates a coroutine, but instead of returning the coroutine itself, it returns a function that, when called, resumes the coroutine.
与 `coroutine.create` 一样，`coroutine.wrap` 函数也创建协程，但它不返回协程本身，而是返回一个函数，当调用该函数时，会恢复协程。

Any arguments passed to this function go as extra arguments to coroutine.resume.
传递给此函数的任何参数都作为额外参数传递给 `coroutine.resume`。

coroutine.wrap returns all the values returned by coroutine.resume, except the first one (the boolean error code).
`coroutine.wrap` 返回 `coroutine.resume` 返回的所有值，除了第一个（布尔错误代码）。

Unlike coroutine.resume, the function created by coroutine.wrap propagates any error to the caller.
与 `coroutine.resume` 不同，`coroutine.wrap` 创建的函数会将任何错误传播给调用者。

In this case, the function also closes the coroutine (see coroutine.close).
在这种情况下，该函数还会关闭协程（见 `coroutine.close`）。

As an example of how coroutines work, consider the following code:
作为协程工作方式的示例，考虑以下代码：

```lua
function foo (a)
  print("foo", a)
  return coroutine.yield(2*a)
end

co = coroutine.create(function (a,b)
      print("co-body", a, b)
      local r = foo(a+1)
      print("co-body", r)
      local r, s = coroutine.yield(a+b, a-b)
      print("co-body", r, s)
      return b, "end"
end)

print("main", coroutine.resume(co, 1, 10))
print("main", coroutine.resume(co, "r"))
print("main", coroutine.resume(co, "x", "y"))
print("main", coroutine.resume(co, "x", "y"))
```

When you run it, it produces the following output:
运行它时，产生以下输出：

```
co-body 1       10
foo     2
main    true    4
co-body r
main    true    11      -9
co-body x       y
main    true    10      end
main    false   cannot resume dead coroutine
```

You can also create and manipulate coroutines through the C API: see functions lua_newthread, lua_resume, and lua_yield.
你还可以通过 C API 创建和操作协程：参见函数 `lua_newthread`、`lua_resume` 和 `lua_yield`。

---

# 3 – The Language（语言）

This section describes the lexis, the syntax, and the semantics of Lua. In other words, this section describes which tokens are valid, how they can be combined, and what their combinations mean.
本节描述 Lua 的词法、语法和语义。换句话说，本节描述哪些词法单元是有效的，它们如何组合，以及它们的组合意味着什么。

Language constructs will be explained using the usual extended BNF notation, in which {a} means 0 or more a's, and [a] means an optional a.
语言构造将使用通常的扩展 BNF 表示法来解释，其中 {a} 表示 0 个或多个 a，[a] 表示可选的 a。

Non-terminals are shown like *non-terminal*, keywords are shown like **kword**, and other terminal symbols are shown like `=`.
非终结符显示为 *non-terminal*，关键字显示为 **kword**，其他终结符显示为 `=`。

The complete syntax of Lua can be found in §9 at the end of this manual.
Lua 的完整语法可以在本手册末尾的 §9 找到。

## 3.1 – Lexical Conventions（词法约定）

Lua is a free-form language. It ignores spaces and comments between lexical elements (tokens), except as delimiters between two tokens.
Lua 是一门自由格式语言。它忽略词法单元（tokens）之间的空格和注释，只作为两个 token 之间的分隔符。

In source code, Lua recognizes as spaces the standard ASCII whitespace characters space, form feed, newline, carriage return, horizontal tab, and vertical tab.
在源代码中，Lua 将标准 ASCII 空白字符（空格、换页、换行、回车、水平制表符和垂直制表符）识别为空格。

**Names** (also called identifiers) in Lua can be any string of Latin letters, Arabic-Indic digits, and underscores, not beginning with a digit and not being a reserved word.
**名称**（也称为**标识符**，identifiers）可以是拉丁字母、阿拉伯-印度数字和下划线的任意字符串，不能以数字开头，也不能是保留字。

- **标识符规则**：与 C 类似，但允许 `$`（在某些 locale 中），见 `llex.c` 中 `lislalpha`。

Identifiers are used to name variables, table fields, and labels.
标识符用于命名变量、表字段和标签。

The following keywords are reserved and cannot be used as names:
以下关键字是保留的，不能用作名称：

```
and       break     do        else      elseif    end
false     for       function  goto      if        in
local     nil       not       or        repeat    return
then      true      until     while
```

- **保留字**：`llex.c` 中 `luaX_init` 将保留字存入字符串表并标记为保留。

Lua is a case-sensitive language: and is a reserved word, but And and AND are two different, valid names.
Lua 是区分大小写的语言：`and` 是保留字，但 `And` 和 `AND` 是两个不同的有效名称。

As a convention, programs should avoid creating names that start with an underscore followed by one or more uppercase letters (such as _VERSION).
按照惯例，程序应避免创建以下划线开头后跟一个或多个大写字母的名称（例如 `_VERSION`）。

The following strings denote other tokens:
以下字符串表示其他 token：

```
+     -     *     /     %     ^     #
&     ~     |     <<    >>    //
==    ~=    <=    >=    <     >     =
(     )     {     }     [     ]     ::
;     :     ,     .     ..    ...
```

A short literal string can be delimited by matching single or double quotes, and can contain the following C-like escape sequences:
**短字面字符串**（short literal string）可以用匹配的单引号或双引号分隔，可以包含以下类似 C 的转义序列：

- `\a` (bell, 响铃)
- `\b` (backspace, 退格)
- `\f` (form feed, 换页)
- `\n` (newline, 换行)
- `\r` (carriage return, 回车)
- `\t` (horizontal tab, 水平制表)
- `\v` (vertical tab, 垂直制表)
- `\\` (backslash, 反斜杠)
- `\"` (quotation mark [double quote], 双引号)
- `\'` (apostrophe [single quote], 单引号)

A backslash followed by a line break results in a newline in the string.
反斜杠后跟换行会在字符串中产生换行。

The escape sequence \z skips the following span of whitespace characters, including line breaks; it is particularly useful to break and indent a long literal string into multiple lines without adding the newlines and spaces into the string contents.
转义序列 `\z` 跳过随后的空白字符（包括换行）；这对于将长字面字符串拆分为多行而不将换行和空格加入字符串内容特别有用。

A short literal string cannot contain unescaped line breaks nor escapes not forming a valid escape sequence.
短字面字符串不能包含未转义的换行，也不能包含不构成有效转义序列的转义。

We can specify any byte in a short literal string, including embedded zeros, by its numeric value.
我们可以通过数值指定短字面字符串中的任何字节，包括嵌入的零。

This can be done with the escape sequence \xXX, where XX is a sequence of exactly two hexadecimal digits, or with the escape sequence \ddd, where ddd is a sequence of up to three decimal digits.
这可以通过转义序列 `\xXX` 完成，其中 `XX` 是恰好两个十六进制数字；或通过 `\ddd`，其中 `ddd` 是最多三个十进制数字。

(Note that if a decimal escape sequence is to be followed by a digit, it must be expressed using exactly three digits.)
（注意，如果十进制转义序列后面跟着数字，它必须恰好用三位数字表示。）

The UTF-8 encoding of a Unicode character can be inserted in a literal string with the escape sequence \u{XXX} (with mandatory enclosing braces), where XXX is a sequence of one or more hexadecimal digits representing the character code point.
Unicode 字符的 UTF-8 编码可以通过转义序列 `\u{XXX}`（必须有花括号）插入字面字符串，其中 `XXX` 是一个或多个十六进制数字，表示字符码点。

This code point can be any value less than 2^31.
此码点可以是任何小于 2^31 的值。

(Lua uses the original UTF-8 specification here, which is not restricted to valid Unicode code points.)
（Lua 这里使用原始的 UTF-8 规范，不限制为有效的 Unicode 码点。）

Literal strings can also be defined using a long format enclosed by long brackets.
字面字符串也可以用由**长括号**（long brackets）包围的长格式定义。

We define an opening long bracket of level n as an opening square bracket followed by n equal signs followed by another opening square bracket.
我们将**第 n 级开长括号**定义为开方括号后跟 n 个等号再跟另一个开方括号。

So, an opening long bracket of level 0 is written as [[, an opening long bracket of level 1 is written as [=[, and so on.
因此，第 0 级开长括号写作 `[[`，第 1 级写作 `[=[`，以此类推。

A closing long bracket is defined similarly; for instance, a closing long bracket of level 4 is written as ]====].
闭长括号定义类似；例如，第 4 级闭长括号写作 `]====]`。

A long literal starts with an opening long bracket of any level and ends at the first closing long bracket of the same level.
**长字面**（long literal）以任意级别的开长括号开始，结束于第一个相同级别的闭长括号。

It can contain any text except a closing bracket of the same level.
它可以包含任何文本，除了相同级别的闭括号。

Literals in this bracketed form can run for several lines, do not interpret any escape sequences, and ignore long brackets of any other level.
这种括号形式的字面量可以跨越多行，不解释任何转义序列，并忽略任何其他级别的长括号。

Any kind of end-of-line sequence (carriage return, newline, carriage return followed by newline, or newline followed by carriage return) is converted to a simple newline.
任何行尾序列（回车、换行、回车后跟换行、换行后跟回车）都被转换为简单的换行。

When the opening long bracket is immediately followed by a newline, the newline is not included in the string.
当开长括号紧跟换行时，该换行不包含在字符串中。

As an example, in a system using ASCII (in which 'a' is coded as 97, newline is coded as 10, and '1' is coded as 49), the five literal strings below denote the same string:
例如，在使用 ASCII 的系统中（`'a'` 编码为 97，换行编码为 10，`'1'` 编码为 49），以下五个字面字符串表示相同的字符串：

```lua
a = 'alo\n123"'
a = "alo\n123\""
a = '\97lo\10\04923"'
a = [[alo
123"]]
a = [==[
alo
123"]==]
```

Any byte in a literal string not explicitly affected by the previous rules represents itself.
字面字符串中任何未被前述规则明确影响的字节都表示自身。

However, Lua opens files for parsing in text mode, and the system's file functions may have problems with some control characters. So, it is safer to represent binary data as a quoted literal with explicit escape sequences for the non-text characters.
然而，Lua 以文本模式打开文件进行解析，系统的文件函数可能对某些控制字符有问题。因此，将二进制数据表示为带引号的字面量，并为非文本字符使用显式转义序列更安全。

A numeric constant (or numeral) can be written with an optional fractional part and an optional decimal exponent, marked by a letter e or E.
**数字常量**（numeric constant，或 numeral）可以写为可选小数部分和可选十进制指数，指数用字母 `e` 或 `E` 标记。

Lua also accepts hexadecimal constants, which start with 0x or 0X.
Lua 也接受十六进制常量，以 `0x` 或 `0X` 开头。

Hexadecimal constants also accept an optional fractional part plus an optional binary exponent, marked by a letter p or P and written in decimal.
十六进制常量也接受可选小数部分和可选二进制指数，指数用字母 `p` 或 `P` 标记，以十进制书写。

(For instance, 0x1.fp10 denotes 1984, which is 0x1f / 16 multiplied by 2^10.)
（例如，`0x1.fp10` 表示 1984，即 `0x1f / 16` 乘以 2^10。）

A numeric constant with a radix point or an exponent denotes a float; otherwise, if its value fits in an integer or it is a hexadecimal constant, it denotes an integer; otherwise (that is, a decimal integer numeral that overflows), it denotes a float.
具有小数点或指数的数字常量表示浮点数；否则，如果其值适合整数或是十六进制常量，它表示整数；否则（即十进制整数字面量溢出），它表示浮点数。

Hexadecimal numerals with neither a radix point nor an exponent always denote an integer value; if the value overflows, it wraps around to fit into a valid integer.
既无小数点也无指数的十六进制数字总是表示整数值；如果值溢出，它会回绕以适应有效整数。

Examples of valid integer constants are
有效整数常量的示例：

```
3   345   0xff   0xBEBADA
```

Examples of valid float constants are
有效浮点常量的示例：

```
3.0     3.1416     314.16e-2     0.31416E1     34e1
0x0.1E  0xA23p-4   0X1.921FB54442D18P+1
```

A comment starts with a double hyphen (--) anywhere outside a string.
**注释**（comment）在字符串外以双连字符（`--`）开头。

If the text immediately after -- is not an opening long bracket, the comment is a short comment, which runs until the end of the line.
如果 `--` 后的文本不是开长括号，注释是**短注释**（short comment），持续到行尾。

Otherwise, it is a long comment, which runs until the corresponding closing long bracket.
否则，它是**长注释**（long comment），持续到对应的闭长括号。

## 3.2 – Variables（变量）

Variables are places that store values. There are three kinds of variables in Lua: global variables, local variables, and table fields.
变量是存储值的地方。Lua 中有三种变量：全局变量、局部变量和表字段。

A single name can denote a global variable or a local variable (or a function's formal parameter, which is a particular kind of local variable):
单个名称可以表示全局变量或局部变量（或函数的形式参数，这是一种特殊的局部变量）：

```bnf
var ::= Name
```

`Name` 表示标识符（见 §3.1）。

Any variable name is assumed to be global unless explicitly declared as a local (see §3.3.7).
除非显式声明为局部变量（见 §3.3.7），否则任何变量名都被假定为全局的。

Local variables are lexically scoped: local variables can be freely accessed by functions defined inside their scope (see §3.5).
局部变量是**词法作用域**的：局部变量可以被定义在其作用域内的函数自由访问（见 §3.5）。

- **词法作用域**：局部变量的生命周期和可见性由代码块结构决定，编译时确定（见 `lparser.c`）。

Before the first assignment to a variable, its value is nil.
在第一次赋值之前，变量的值是 `nil`。

Square brackets are used to index a table:
方括号用于索引表：

```bnf
var ::= prefixexp '[' exp ']'
```

The meaning of accesses to table fields can be changed via metatables (see §2.4).
对表字段的访问的含义可以通过元表改变（见 §2.4）。

The syntax var.Name is just syntactic sugar for var["Name"]:
语法 `var.Name` 只是 `var["Name"]` 的语法糖：

```bnf
var ::= prefixexp '.' Name
```

An access to a global variable x is equivalent to _ENV.x.
对全局变量 `x` 的访问等价于 `_ENV.x`。

Due to the way that chunks are compiled, the variable _ENV itself is never global (see §2.2).
由于代码块的编译方式，变量 `_ENV` 本身永远不会是全局的（见 §2.2）。

## 3.3 – Statements（语句）

Lua supports an almost conventional set of statements, similar to those in other conventional languages. This set includes blocks, assignments, control structures, function calls, and variable declarations.
Lua 支持一套几乎常规的语句，与其他常规语言中的类似。这套语句包括块、赋值、控制结构、函数调用和变量声明。

### 3.3.1 – Blocks（块）

A block is a list of statements, which are executed sequentially:
**块**（block）是按顺序执行的语句列表：

```bnf
block ::= {stat}
```

Lua has empty statements that allow you to separate statements with semicolons, start a block with a semicolon or write two semicolons in sequence:
Lua 有**空语句**，允许你用分号分隔语句、用分号开始块或连续写两个分号：

```bnf
stat ::= ';'
```

Both function calls and assignments can start with an open parenthesis. This possibility leads to an ambiguity in Lua's grammar.
函数调用和赋值都可以以开括号开头。这导致了 Lua 语法中的歧义。

Consider the following fragment:
考虑以下片段：

```lua
a = b + c
(print or io.write)('done')
```

The grammar could see this fragment in two ways:
语法可以以两种方式看待这个片段：

```lua
a = b + c(print or io.write)('done')

a = b + c; (print or io.write)('done')
```

The current parser always sees such constructions in the first way, interpreting the open parenthesis as the start of the arguments to a call.
当前解析器总是以第一种方式看待这种构造，将开括号解释为调用参数的开始。

To avoid this ambiguity, it is a good practice to always precede with a semicolon statements that start with a parenthesis:
为避免这种歧义，最佳实践是总是在以括号开头的语句前加分号：

```lua
;(print or io.write)('done')
```

A block can be explicitly delimited to produce a single statement:
块可以显式界定以产生单个语句：

```bnf
stat ::= do block end
```

Explicit blocks are useful to control the scope of variable declarations.
显式块对于控制变量声明的作用域很有用。

Explicit blocks are also sometimes used to add a return statement in the middle of another block (see §3.3.4).
显式块有时也用于在另一个块中间添加 `return` 语句（见 §3.3.4）。

### 3.3.2 – Chunks（代码块）

The unit of compilation of Lua is called a chunk.
Lua 的编译单元称为**代码块**（chunk）。

Syntactically, a chunk is simply a block:
语法上，代码块就是一个块：

```bnf
chunk ::= block
```

Lua handles a chunk as the body of an anonymous function with a variable number of arguments (see §3.4.11).
Lua 将代码块处理为具有可变参数的匿名函数的体（见 §3.4.11）。

As such, chunks can define local variables, receive arguments, and return values.
因此，代码块可以定义局部变量、接收参数和返回值。

Moreover, such anonymous function is compiled as in the scope of an external local variable called _ENV (see §2.2). The resulting function always has _ENV as its only external variable, even if it does not use that variable.
此外，此类匿名函数在名为 `_ENV` 的外部局部变量的作用域内编译（见 §2.2）。得到的函数总是将 `_ENV` 作为其唯一的外部变量，即使它不使用该变量。

A chunk can be stored in a file or in a string inside the host program.
代码块可以存储在文件中或宿主程序内的字符串中。

To execute a chunk, Lua first loads it, precompiling the chunk's code into instructions for a virtual machine, and then Lua executes the compiled code with an interpreter for the virtual machine.
要执行代码块，Lua 首先**加载**它，将代码块的代码预编译为虚拟机的指令，然后 Lua 用虚拟机解释器执行编译后的代码。

- **加载过程**：`ldo.c` 中 `luaD_protectedparser` 调用 `lparser.c` 解析，生成字节码。

Chunks can also be precompiled into binary form; see the program luac and the function string.dump for details.
代码块也可以预编译为二进制形式；详见程序 `luac` 和函数 `string.dump`。

Programs in source and compiled forms are interchangeable; Lua automatically detects the file type and acts accordingly (see load).
源代码和编译形式的程序是可互换的；Lua 自动检测文件类型并相应处理（见 `load`）。

### 3.3.3 – Assignment（赋值）

Lua allows multiple assignments. Therefore, the syntax for assignment defines a list of variables on the left side and a list of expressions on the right side. The elements in both lists are separated by commas:
Lua 允许多重赋值。因此，赋值语法在左侧定义变量列表，在右侧定义表达式列表。两个列表中的元素用逗号分隔：

```bnf
stat ::= varlist '=' explist
varlist ::= var {',' var}
explist ::= exp {',' exp}
```

Expressions are discussed in §3.4.
表达式在 §3.4 中讨论。

Before the assignment, the list of values is adjusted to the length of the list of variables (see §3.4.13).
在赋值之前，值列表被**调整**为变量列表的长度（见 §3.4.13）。

If a variable is both assigned and read inside a multiple assignment, Lua ensures that all reads get the value of the variable before the assignment.
如果一个变量在多重赋值中既被赋值又被读取，Lua 确保所有读取都获得赋值前变量的值。

Thus the code
因此代码

```lua
i = 3
i, a[i] = i+1, 20
```

sets a[3] to 20, without affecting a[4] because the i in a[i] is evaluated (to 3) before it is assigned 4.
将 `a[3]` 设置为 20，不影响 `a[4]`，因为 `a[i]` 中的 `i` 在被赋值为 4 之前被求值（为 3）。

Similarly, the line
类似地，行

```lua
x, y = y, x
```

exchanges the values of x and y,
交换 `x` 和 `y` 的值，

and
以及

```lua
x, y, z = y, z, x
```

cyclically permutes the values of x, y, and z.
循环置换 `x`、`y` 和 `z` 的值。

Note that this guarantee covers only accesses syntactically inside the assignment statement. If a function or a metamethod called during the assignment changes the value of a variable, Lua gives no guarantees about the order of that access.
请注意，此保证仅涵盖赋值语句语法内部的访问。如果赋值期间调用的函数或元方法更改了变量的值，Lua 不保证该访问的顺序。

An assignment to a global name x = val is equivalent to the assignment _ENV.x = val (see §2.2).
对全局名称 `x = val` 的赋值等价于 `_ENV.x = val`（见 §2.2）。

The meaning of assignments to table fields and global variables (which are actually table fields, too) can be changed via metatables (see §2.4).
对表字段和全局变量（实际上也是表字段）的赋值的含义可以通过元表改变（见 §2.4）。

### 3.3.4 – Control Structures（控制结构）

The control structures if, while, and repeat have the usual meaning and familiar syntax:
控制结构 `if`、`while` 和 `repeat` 具有通常的含义和熟悉的语法：

```bnf
stat ::= while exp do block end
stat ::= repeat block until exp
stat ::= if exp then block {elseif exp then block} [else block] end
```

Lua also has a for statement, in two flavors (see §3.3.5).
Lua 还有 `for` 语句，有两种形式（见 §3.3.5）。

The condition expression of a control structure can return any value. Both false and nil test false. All values different from nil and false test true. In particular, the number 0 and the empty string also test true.
控制结构的条件表达式可以返回任何值。`false` 和 `nil` 测试为假。所有不同于 `nil` 和 `false` 的值测试为真。特别地，数字 0 和空字符串也测试为真。

In the repeat-until loop, the inner block does not end at the until keyword, but only after the condition. So, the condition can refer to local variables declared inside the loop block.
在 `repeat-until` 循环中，内部块不在 `until` 关键字处结束，而是在条件之后结束。因此，条件可以引用循环块内声明的局部变量。

The goto statement transfers the program control to a label. For syntactical reasons, labels in Lua are considered statements too:
`goto` 语句将程序控制转移到标签。由于语法原因，Lua 中的标签也被视为语句：

```bnf
stat ::= goto Name
stat ::= label
label ::= '::' Name '::'
```

A label is visible in the entire block where it is defined, except inside nested functions.
标签在其定义的整个块中可见，除了嵌套函数内部。

A goto can jump to any visible label as long as it does not enter into the scope of a local variable.
`goto` 可以跳转到任何可见的标签，只要不进入局部变量的作用域。

A label should not be declared where a label with the same name is visible, even if this other label has been declared in an enclosing block.
不应在具有相同名称的标签可见的地方声明标签，即使另一个标签已在封闭块中声明。

The break statement terminates the execution of a while, repeat, or for loop, skipping to the next statement after the loop:
`break` 语句终止 `while`、`repeat` 或 `for` 循环的执行，跳到循环后的下一个语句：

```bnf
stat ::= break
```

A break ends the innermost enclosing loop.
`break` 结束最内层的封闭循环。

The return statement is used to return values from a function or a chunk (which is handled as an anonymous function).
`return` 语句用于从函数或代码块（被视为匿名函数）返回值。

Functions can return more than one value, so the syntax for the return statement is
函数可以返回多个值，因此 `return` 语句的语法是

```bnf
stat ::= return [explist] [';']
```

The return statement can only be written as the last statement of a block. If it is necessary to return in the middle of a block, then an explicit inner block can be used, as in the idiom do return end, because now return is the last statement in its (inner) block.
`return` 语句只能写为块的最后一个语句。如果需要在块中间返回，则可以使用显式内部块，如惯用法 `do return end`，因为现在 `return` 是其（内部）块的最后一个语句。

### 3.3.5 – For Statement（for 语句）

The for statement has two forms: one numerical and one generic.
`for` 语句有两种形式：一种是数值型的，一种是泛型的。

#### The numerical for loop（数值型 for 循环）

The numerical for loop repeats a block of code while a control variable goes through an arithmetic progression. It has the following syntax:
数值型 `for` 循环在控制变量遍历等差数列时重复执行代码块。其语法如下：

```bnf
stat ::= for Name '=' exp ',' exp [',' exp] do block end
```

The given identifier (Name) defines the control variable, which is a new variable local to the loop body (block).
给定的标识符（`Name`）定义了控制变量，这是一个新的局部于循环体（`block`）的变量。

The loop starts by evaluating once the three control expressions. Their values are called respectively the initial value, the limit, and the step. If the step is absent, it defaults to 1.
循环开始时一次性求值三个控制表达式。它们的值分别称为**初始值**（initial value）、**限制**（limit）和**步长**（step）。如果步长缺失，默认为 1。

If both the initial value and the step are integers, the loop is done with integers; note that the limit may not be an integer. Otherwise, the three values are converted to floats and the loop is done with floats. Beware of floating-point accuracy in this case.
如果初始值和步长都是整数，循环用整数进行；注意限制可能不是整数。否则，三个值被转换为浮点数，循环用浮点数进行。在这种情况下 beware 浮点精度。

- **整数循环优化**：`lvm.c` 中 `luaV_execute` 对 `OP_FORPREP`/`OP_FORLOOP` 使用整数运算，溢出时结束循环。

After that initialization, the loop body is repeated with the value of the control variable going through an arithmetic progression, starting at the initial value, with a common difference given by the step.
初始化之后，循环体重复执行，控制变量的值按等差数列遍历，从初始值开始，公差由步长给出。

A negative step makes a decreasing sequence; a step equal to zero raises an error.
负步长形成递减序列；步长等于零会引发错误。

The loop continues while the value is less than or equal to the limit (greater than or equal to for a negative step).
当值小于或等于限制时循环继续（对于负步长，大于或等于）。

If the initial value is already greater than the limit (or less than, if the step is negative), the body is not executed.
如果初始值已经大于限制（或小于，如果步长为负），则不执行循环体。

For integer loops, the control variable never wraps around; instead, the loop ends in case of an overflow.
对于整数循环，控制变量永远不会回绕；相反，在溢出时循环结束。

You should not change the value of the control variable during the loop. If you need its value after the loop, assign it to another variable before exiting the loop.
你不应在循环期间更改控制变量的值。如果你需要在循环后使用其值，请在退出循环前将其赋值给另一个变量。

#### The generic for loop（泛型 for 循环）

The generic for statement works over functions, called iterators.
泛型 `for` 语句基于称为**迭代器**（iterators）的函数工作。

On each iteration, the iterator function is called to produce a new value, stopping when this new value is nil.
每次迭代时调用迭代器函数以产生一个新值，当此新值为 `nil` 时停止。

The generic for loop has the following syntax:
泛型 `for` 循环具有以下语法：

```bnf
stat ::= for namelist in explist do block end
namelist ::= Name {',' Name}
```

A for statement like
如下形式的 `for` 语句

```lua
for var_1, ..., var_n in explist do body end
```

works as follows.
工作方式如下。

The names var_i declare loop variables local to the loop body.
名称 `var_i` 声明局部于循环体的循环变量。

The first of these variables is the control variable.
这些变量中的第一个是**控制变量**。

The loop starts by evaluating explist to produce four values: an iterator function, a state, an initial value for the control variable, and a closing value.
循环开始时求值 `explist` 以产生四个值：**迭代器函数**、**状态**、控制变量的初始值和**关闭值**。

Then, at each iteration, Lua calls the iterator function with two arguments: the state and the control variable.
然后，在每次迭代时，Lua 用两个参数调用迭代器函数：状态和控制变量。

The results from this call are then assigned to the loop variables, following the rules of multiple assignments (see §3.3.3). If the control variable becomes nil, the loop terminates. Otherwise, the body is executed and the loop goes to the next iteration.
此调用的结果随后按照多重赋值的规则赋给循环变量（见 §3.3.3）。如果控制变量变为 `nil`，循环终止。否则，执行循环体并进入下一次迭代。

The closing value behaves like a to-be-closed variable (see §3.3.8), which can be used to release resources when the loop ends. Otherwise, it does not interfere with the loop.
关闭值的行为类似于待关闭变量（见 §3.3.8），可用于在循环结束时释放资源。否则，它不干扰循环。

You should not change the value of the control variable during the loop.
你不应在循环期间更改控制变量的值。

- **迭代器机制**：`pairs` 返回 `next`、`t`、 `nil`；`ipairs` 返回迭代器、表、 `0`。每次调用迭代器返回下一个键值对。

### 3.3.6 – Function Calls as Statements（作为语句的函数调用）

To allow possible side-effects, function calls can be executed as statements:
为了允许可能的副作用，函数调用可以作为语句执行：

```bnf
stat ::= functioncall
```

In this case, all returned values are thrown away.
在这种情况下，所有返回值都被丢弃。

Function calls are explained in §3.4.10.
函数调用在 §3.4.10 中解释。

### 3.3.7 – Local Declarations（局部声明）

Local variables can be declared anywhere inside a block. The declaration can include an initialization:
局部变量可以在块内的任何地方声明。声明可以包含初始化：

```bnf
stat ::= local attnamelist ['=' explist]
attnamelist ::= Name attrib {',' Name attrib}
```

If present, an initial assignment has the same semantics of a multiple assignment (see §3.3.3). Otherwise, all variables are initialized with nil.
如果存在，初始赋值具有与多重赋值相同的语义（见 §3.3.3）。否则，所有变量都用 `nil` 初始化。

Each variable name may be postfixed by an attribute (a name between angle brackets):
每个变量名后可以跟属性（尖括号中的名称）：

```bnf
attrib ::= ['<' Name '>']
```

There are two possible attributes: const, which declares a constant variable, that is, a variable that cannot be assigned to after its initialization; and close, which declares a to-be-closed variable (see §3.3.8).
有两种可能的属性：`const`，声明**常量变量**，即初始化后不能被赋值的变量；和 `close`，声明**待关闭变量**（见 §3.3.8）。

- **const 属性**：`lparser.c` 中标记变量不可变，后续赋值会编译错误。

A list of variables can contain at most one to-be-closed variable.
变量列表最多只能包含一个待关闭变量。

A chunk is also a block (see §3.3.2), and so local variables can be declared in a chunk outside any explicit block.
代码块也是一个块（见 §3.3.2），因此局部变量可以在代码块中任何显式块之外声明。

The visibility rules for local variables are explained in §3.5.
局部变量的可见性规则在 §3.5 中解释。

### 3.3.8 – To-be-closed Variables（待关闭变量）

A to-be-closed variable behaves like a constant local variable, except that its value is closed whenever the variable goes out of scope, including normal block termination, exiting its block by break/goto/return, or exiting by an error.
待关闭变量的行为类似于常量局部变量，区别在于每当变量超出作用域时其值被**关闭**，包括正常块终止、通过 `break`/`goto`/`return` 退出其块，或通过错误退出。

Here, to close a value means to call its __close metamethod.
这里，**关闭**一个值意味着调用其 `__close` 元方法。

When calling the metamethod, the value itself is passed as the first argument and the error object that caused the exit (if any) is passed as a second argument; if there was no error, the second argument is nil.
调用元方法时，值本身作为第一个参数传递，导致退出的错误对象（如果有）作为第二个参数传递；如果没有错误，第二个参数是 `nil`。

The value assigned to a to-be-closed variable must have a __close metamethod or be a false value.
赋给待关闭变量的值必须具有 `__close` 元方法或是假值。

(nil and false are ignored as to-be-closed values.)
（`nil` 和 `false` 作为待关闭值被忽略。）

If several to-be-closed variables go out of scope at the same event, they are closed in the reverse order that they were declared.
如果多个待关闭变量在同一事件中超出了作用域，它们按照声明的相反顺序被关闭。

If there is any error while running a closing method, that error is handled like an error in the regular code where the variable was defined. After an error, the other pending closing methods will still be called.
如果运行关闭方法时出现任何错误，该错误会像在定义变量的常规代码中的错误一样处理。错误后，其他待处理的关闭方法仍会被调用。

If a coroutine yields and is never resumed again, some variables may never go out of scope, and therefore they will never be closed.
如果协程让出且不再被恢复，某些变量可能永远不会超出作用域，因此永远不会被关闭。

(These variables are the ones created inside the coroutine and in scope at the point where the coroutine yielded.)
（这些变量是在协程内创建并在协程让出点处于作用域中的变量。）

Similarly, if a coroutine ends with an error, it does not unwind its stack, so it does not close any variable.
类似地，如果协程以错误结束，它不会展开其栈，因此不会关闭任何变量。

In both cases, you can either use finalizers or call coroutine.close to close the variables.
在这两种情况下，你可以使用终结器或调用 `coroutine.close` 来关闭变量。

However, if the coroutine was created through coroutine.wrap, then its corresponding function will close the coroutine in case of errors.
然而，如果协程是通过 `coroutine.wrap` 创建的，那么其对应的函数将在出错时关闭协程。

## 3.4 – Expressions（表达式）

The basic expressions in Lua are the following:
Lua 中的基本表达式如下：

```bnf
exp ::= prefixexp
exp ::= nil | false | true
exp ::= Numeral
exp ::= LiteralString
exp ::= functiondef
exp ::= tableconstructor
exp ::= '...'
exp ::= exp binop exp
exp ::= unop exp
prefixexp ::= var | functioncall | '(' exp ')'
```

Numerals and literal strings are explained in §3.1; variables are explained in §3.2; function definitions are explained in §3.4.11; function calls are explained in §3.4.10; table constructors are explained in §3.4.9.
数字和字面字符串在 §3.1 中解释；变量在 §3.2 中解释；函数定义在 §3.4.11 中解释；函数调用在 §3.4.10 中解释；表构造器在 §3.4.9 中解释。

Vararg expressions, denoted by three dots (...), can only be used when directly inside a variadic function; they are explained in §3.4.11.
变长参数表达式，由三个点（`...`）表示，只能在变长函数内部直接使用；它们在 §3.4.11 中解释。

Binary operators comprise arithmetic operators (see §3.4.1), bitwise operators (see §3.4.2), relational operators (see §3.4.4), logical operators (see §3.4.5), and the concatenation operator (see §3.4.6).
二元运算符包括算术运算符（见 §3.4.1）、按位运算符（见 §3.4.2）、关系运算符（见 §3.4.4）、逻辑运算符（见 §3.4.5）和连接运算符（见 §3.4.6）。

Unary operators comprise the unary minus (see §3.4.1), the unary bitwise NOT (see §3.4.2), the unary logical not (see §3.4.5), and the unary length operator (see §3.4.7).
一元运算符包括一元减（见 §3.4.1）、一元按位非（见 §3.4.2）、一元逻辑非（见 §3.4.5）和一元长度运算符（见 §3.4.7）。

### 3.4.1 – Arithmetic Operators（算术运算符）

Lua supports the following arithmetic operators:
Lua 支持以下算术运算符：

- `+` ：加法
- `-` ：减法
- `*` ：乘法
- `/` ：浮点除法
- `//` ：向下取整除法
- `%` ：取模
- `^` ：幂运算
- `-` ：一元减

With the exception of exponentiation and float division, the arithmetic operators work as follows: If both operands are integers, the operation is performed over integers and the result is an integer. Otherwise, if both operands are numbers, then they are converted to floats, the operation is performed following the machine's rules for floating-point arithmetic (usually the IEEE 754 standard), and the result is a float.
除了幂运算和浮点除法外，算术运算符的工作方式如下：如果两个操作数都是整数，则运算在整数上进行，结果是整数。否则，如果两个操作数都是数字，则它们被转换为浮点数，按照机器的浮点运算规则（通常是 IEEE 754 标准）执行运算，结果是浮点数。

(The string library coerces strings to numbers in arithmetic operations; see §3.4.3 for details.)
（字符串库在算术运算中将字符串强制转换为数字；详见 §3.4.3。）

Exponentiation and float division (/) always convert their operands to floats and the result is always a float.
幂运算和浮点除法（`/`）总是将其操作数转换为浮点数，结果总是浮点数。

Exponentiation uses the ANSI pow, so that it works for non-integer exponents too.
幂运算使用 ANSI `pow`，因此它也适用于非整数指数。

- **pow 函数**：`lmathlib.c` 中 `math_pow` 封装 `pow`，`lvm.c` 中幂运算调用 `luaV_pow`。

Floor division (//) is a division that rounds the quotient towards minus infinity, resulting in the floor of the division of its operands.
向下取整除法（`//`）是一种向负无穷大舍入商的除法，结果是其操作数除法的向下取整。

Modulo is defined as the remainder of a division that rounds the quotient towards minus infinity (floor division).
取模定义为向负无穷大舍除商（向下取整除法）的除法的余数。

In case of overflows in integer arithmetic, all operations wrap around.
在整数算术溢出的情况下，所有操作都会回绕。

### 3.4.2 – Bitwise Operators（按位运算符）

Lua supports the following bitwise operators:
Lua 支持以下按位运算符：

- `&` ：按位与
- `|` ：按位或
- `~` ：按位异或
- `>>` ：右移
- `<<` ：左移
- `~` ：一元按位非

All bitwise operations convert its operands to integers (see §3.4.3), operate on all bits of those integers, and result in an integer.
所有按位运算将其操作数转换为整数（见 §3.4.3），对这些整数的所有位进行操作，结果为整数。

Both right and left shifts fill the vacant bits with zeros.
右移和左移都用零填充空位。

Negative displacements shift to the other direction; displacements with absolute values equal to or higher than the number of bits in an integer result in zero (as all bits are shifted out).
负位移向相反方向移动；绝对值等于或高于整数位数的位移结果为零（因为所有位都被移出）。

- **按位运算实现**：`lvm.c` 中 `luaV_shiftl` 处理位移，`luaO_arith` 处理其他按位运算。

### 3.4.3 – Coercions and Conversions（强制转换与转换）

Lua provides some automatic conversions between some types and representations at run time.
Lua 在运行时提供一些类型和表示之间的自动转换。

Bitwise operators always convert float operands to integers.
按位运算符总是将浮点操作数转换为整数。

Exponentiation and float division always convert integer operands to floats.
幂运算和浮点除法总是将整数操作数转换为浮点数。

All other arithmetic operations applied to mixed numbers (integers and floats) convert the integer operand to a float.
应用于混合数字（整数和浮点数）的所有其他算术运算将整数操作数转换为浮点数。

The C API also converts both integers to floats and floats to integers, as needed.
C API 也根据需要将整数转换为浮点数，将浮点数转换为整数。

Moreover, string concatenation accepts numbers as arguments, besides strings.
此外，字符串连接接受数字作为参数，除了字符串。

In a conversion from integer to float, if the integer value has an exact representation as a float, that is the result. Otherwise, the conversion gets the nearest higher or the nearest lower representable value. This kind of conversion never fails.
在从整数到浮点数的转换中，如果整数值具有精确的浮点数表示，则结果就是该值。否则，转换获得最近的可表示的较高或较低值。这种转换永远不会失败。

The conversion from float to integer checks whether the float has an exact representation as an integer (that is, the float has an integral value and it is in the range of integer representation). If it does, that representation is the result. Otherwise, the conversion fails.
从浮点数到整数的转换检查浮点数是否具有精确的整数表示（即浮点数具有整数值，且在整数表示范围内）。如果是，则该表示就是结果。否则，转换失败。

Several places in Lua coerce strings to numbers when necessary. In particular, the string library sets metamethods that try to coerce strings to numbers in all arithmetic operations.
Lua 中的几个地方在需要时将字符串强制转换为数字。特别地，字符串库设置元方法，尝试在所有算术运算中将字符串强制转换为数字。

If the conversion fails, the library calls the metamethod of the other operand (if present) or it raises an error.
如果转换失败，库调用另一个操作数的元方法（如果存在）或引发错误。

Note that bitwise operators do not do this coercion.
注意，按位运算符不进行这种强制转换。

It is always a good practice not to rely on the implicit coercions from strings to numbers, as they are not always applied; in particular, "1"==1 is false and "1"<1 raises an error (see §3.4.4).
不依赖从字符串到数字的隐式强制转换总是一个好习惯，因为它们并不总是适用；特别地，`"1"==1` 为假，`"1"<1` 会引发错误（见 §3.4.4）。

These coercions exist mainly for compatibility and may be removed in future versions of the language.
这些强制转换主要为了兼容性而存在，可能会在未来版本的语音中移除。

A string is converted to an integer or a float following its syntax and the rules of the Lua lexer.
字符串根据其语法和 Lua 词法分析器的规则转换为整数或浮点数。

The string may have also leading and trailing whitespaces and a sign.
字符串还可以有前导和尾随空白以及符号。

All conversions from strings to numbers accept both a dot and the current locale mark as the radix character.
所有从字符串到数字的转换都接受点和当前区域设置标记作为基数字符。

(The Lua lexer, however, accepts only a dot.)
（然而，Lua 词法分析器只接受点。）

If the string is not a valid numeral, the conversion fails.
如果字符串不是有效的数字，转换失败。

If necessary, the result of this first step is then converted to a specific number subtype following the previous rules for conversions between floats and integers.
如果需要，此第一步的结果随后按照浮点数和整数之间转换的先前规则转换为特定的数字子类型。

The conversion from numbers to strings uses a non-specified human-readable format.
从数字到字符串的转换使用未指定的可读格式。

To convert numbers to strings in any specific way, use the function string.format.
要以任何特定方式将数字转换为字符串，请使用函数 `string.format`。

### 3.4.4 – Relational Operators（关系运算符）

Lua supports the following relational operators:
Lua 支持以下关系运算符：

- `==` ：等于
- `~=` ：不等于
- `<` ：小于
- `>` ：大于
- `<=` ：小于等于
- `>=` ：大于等于

These operators always result in false or true.
这些运算符总是产生 `false` 或 `true`。

Equality (==) first compares the type of its operands. If the types are different, then the result is false. Otherwise, the values of the operands are compared.
等于（`==`）首先比较其操作数的类型。如果类型不同，则结果为 `false`。否则，比较操作数的值。

Strings are equal if they have the same byte content.
如果字符串具有相同的字节内容，则它们相等。

Numbers are equal if they denote the same mathematical value.
如果数字表示相同的数学值，则它们相等。

Tables, userdata, and threads are compared by reference: two objects are considered equal only if they are the same object.
表、userdata 和线程通过引用进行比较：只有当两个对象是同一个对象时，它们才被认为是相等的。

Every time you create a new object (a table, a userdata, or a thread), this new object is different from any previously existing object.
每次创建新对象（表、userdata 或线程）时，这个新对象与任何先前存在的对象都不同。

A function is always equal to itself.
函数总是等于自身。

Functions with any detectable difference (different behavior, different definition) are always different.
具有任何可检测差异（不同行为、不同定义）的函数总是不同的。

Functions created at different times but with no detectable differences may be classified as equal or not (depending on internal caching details).
在不同时间创建但没有可检测差异的函数可能被分类为相等或不相等（取决于内部缓存细节）。

You can change the way that Lua compares tables and userdata by using the __eq metamethod (see §2.4).
你可以使用 `__eq` 元方法改变 Lua 比较表和 userdata 的方式（见 §2.4）。

Equality comparisons do not convert strings to numbers or vice versa. Thus, "0"==0 evaluates to false, and t[0] and t["0"] denote different entries in a table.
等于比较不会将字符串转换为数字或反之。因此，`"0"==0` 求值为 `false`，`t[0]` 和 `t["0"]` 表示表中的不同条目。

The operator ~= is exactly the negation of equality (==).
运算符 `~=` 正好是等于（`==`）的否定。

The order operators work as follows. If both arguments are numbers, then they are compared according to their mathematical values, regardless of their subtypes.
顺序运算符的工作方式如下。如果两个参数都是数字，则根据它们的数学值进行比较，无论其子类型如何。

Otherwise, if both arguments are strings, then their values are compared according to the current locale.
否则，如果两个参数都是字符串，则根据当前区域设置比较它们的值。

Otherwise, Lua tries to call the __lt or the __le metamethod (see §2.4).
否则，Lua 尝试调用 `__lt` 或 `__le` 元方法（见 §2.4）。

A comparison a > b is translated to b < a and a >= b is translated to b <= a.
比较 `a > b` 被转换为 `b < a`，`a >= b` 被转换为 `b <= a`。

Following the IEEE 754 standard, the special value NaN is considered neither less than, nor equal to, nor greater than any value, including itself.
根据 IEEE 754 标准，特殊值 NaN 被认为不小于、不等于也不大于任何值，包括它自己。

### 3.4.5 – Logical Operators（逻辑运算符）

The logical operators in Lua are and, or, and not.
Lua 中的逻辑运算符是 `and`、`or` 和 `not`。

Like the control structures (see §3.3.4), all logical operators consider both false and nil as false and anything else as true.
像控制结构（见 §3.3.4）一样，所有逻辑运算符都将 `false` 和 `nil` 视为假，其他任何东西视为真。

The negation operator not always returns false or true.
否定运算符 `not` 总是返回 `false` 或 `true`。

The conjunction operator and returns its first argument if this value is false or nil; otherwise, and returns its second argument.
合取运算符 `and` 如果第一个参数是 `false` 或 `nil`，则返回第一个参数；否则，`and` 返回第二个参数。

The disjunction operator or returns its first argument if this value is different from nil and false; otherwise, or returns its second argument.
析取运算符 `or` 如果第一个参数不同于 `nil` 和 `false`，则返回第一个参数；否则，`or` 返回第二个参数。

Both and and or use short-circuit evaluation; that is, the second operand is evaluated only if necessary.
`and` 和 `or` 都使用**短路求值**；也就是说，只有在必要时才求值第二个操作数。

Here are some examples:
以下是一些示例：

```lua
10 or 20            --> 10
10 or error()       --> 10
nil or "a"          --> "a"
nil and 10          --> nil
false and error()   --> false
false and nil       --> false
false or nil        --> nil
10 and 20           --> 20
```

- **短路求值**：`lcode.c` 中生成跳转指令，避免不必要的求值。

### 3.4.6 – Concatenation（连接）

The string concatenation operator in Lua is denoted by two dots (..).
Lua 中的字符串连接运算符由两个点（`..`）表示。

If both operands are strings or numbers, then the numbers are converted to strings in a non-specified format (see §3.4.3).
如果两个操作数都是字符串或数字，则数字以未指定的格式转换为字符串（见 §3.4.3）。

Otherwise, the __concat metamethod is called (see §2.4).
否则，调用 `__concat` 元方法（见 §2.4）。

- **字符串连接实现**：`lvm.c` 中 `luaV_concat` 处理连接，会创建新的 `TString`。

### 3.4.7 – The Length Operator（长度运算符）

The length operator is denoted by the unary prefix operator #.
长度运算符由一元前缀运算符 `#` 表示。

The length of a string is its number of bytes.
字符串的长度是其字节数。

(That is the usual meaning of string length when each character is one byte.)
（这是每个字符为一个字节时字符串长度的通常含义。）

The length operator applied on a table returns a border in that table.
应用于表的长度运算符返回该表中的一个**边界**（border）。

A border in a table t is any non-negative integer that satisfies the following condition:
表 `t` 中的边界是满足以下条件的任何非负整数：

```lua
(border == 0 or t[border] ~= nil) and
(t[border + 1] == nil or border == math.maxinteger)
```

In words, a border is any positive integer index present in the table that is followed by an absent index, plus two limit cases: zero, when index 1 is absent; and the maximum value for an integer, when that index is present.
换句话说，边界是表中存在的任何正整数索引，后面跟着一个不存在的索引，加上两个极限情况：零，当索引 1 不存在时；以及整数的最大值，当该索引存在时。

Note that keys that are not positive integers do not interfere with borders.
注意，不是正整数的键不会干扰边界。

A table with exactly one border is called a sequence.
恰好有一个边界的表称为**序列**（sequence）。

For instance, the table {10, 20, 30, 40, 50} is a sequence, as it has only one border (5).
例如，表 `{10, 20, 30, 40, 50}` 是一个序列，因为它只有一个边界（5）。

The table {10, 20, 30, nil, 50} has two borders (3 and 5), and therefore it is not a sequence.
表 `{10, 20, 30, nil, 50}` 有两个边界（3 和 5），因此它不是序列。

(The nil at index 4 is called a hole.)
（索引 4 处的 `nil` 称为**洞**（hole）。）

The table {nil, 20, 30, nil, nil, 60, nil} has three borders (0, 3, and 6), so it is not a sequence, too.
表 `{nil, 20, 30, nil, nil, 60, nil}` 有三个边界（0、3 和 6），所以它也不是序列。

The table {} is a sequence with border 0.
表 `{}` 是具有边界 0 的序列。

When t is a sequence, #t returns its only border, which corresponds to the intuitive notion of the length of the sequence.
当 `t` 是序列时，`#t` 返回其唯一边界，这对应于序列长度的直观概念。

When t is not a sequence, #t can return any of its borders.
当 `t` 不是序列时，`#t` 可以返回其任何边界。

(The exact one depends on details of the internal representation of the table, which in turn can depend on how the table was populated and the memory addresses of its non-numeric keys.)
（确切的边界取决于表内部表示的细节，而这又取决于表是如何填充的以及其非数字键的内存地址。）

The computation of the length of a table has a guaranteed worst time of O(log n), where n is the largest integer key in the table.
表长度的计算有保证的最坏时间为 O(log n)，其中 n 是表中最大的整数键。

- **长度计算**：`ltable.c` 中 `luaH_getn` 使用二分查找在数组部分查找边界。

A program can modify the behavior of the length operator for any value but strings through the __len metamethod (see §2.4).
程序可以通过 `__len` 元方法修改长度运算符对除字符串以外的任何值的行为（见 §2.4）。

### 3.4.8 – Precedence（优先级）

Operator precedence in Lua follows the table below, from lower to higher priority:
Lua 中的运算符优先级遵循下表，从低到高：

```
or
and
<     >     <=    >=    ~=    ==
|
~
&
<<    >>
..
+     -
*     /     //    %
unary operators (not   #     -     ~)
^
```

As usual, you can use parentheses to change the precedences of an expression.
像往常一样，你可以使用括号来改变表达式的优先级。


The concatenation (..) and exponentiation (^) operators are right associative. All other binary operators are left associative.
连接（`..`）和幂运算（`^`）运算符是右结合的。所有其他二元运算符是左结合的。

### 3.4.9 – Table Constructors（表构造器）

Table constructors are expressions that create tables. Every time a constructor is evaluated, a new table is created.
**表构造器**（table constructors）是创建表的表达式。每次求值构造器时，都会创建一个新表。

A constructor can be used to create an empty table or to create a table and initialize some of its fields.
构造器可用于创建空表，或创建表并初始化其某些字段。

The general syntax for constructors is
构造器的一般语法为

```bnf
tableconstructor ::= '{' [fieldlist] '}'
fieldlist ::= field {fieldsep field} [fieldsep]
field ::= '[' exp ']' '=' exp | Name '=' exp | exp
fieldsep ::= ',' | ';'
```

Each field of the form [exp1] = exp2 adds to the new table an entry with key exp1 and value exp2.
`[exp1] = exp2` 形式的每个字段向新表添加一个键为 `exp1`、值为 `exp2` 的条目。

A field of the form name = exp is equivalent to ["name"] = exp.
`name = exp` 形式的字段等价于 `["name"] = exp`。

Fields of the form exp are equivalent to [i] = exp, where i are consecutive integers starting with 1; fields in the other formats do not affect this counting.
`exp` 形式的字段等价于 `[i] = exp`，其中 `i` 是从 1 开始的连续整数；其他格式的字段不影响此计数。

For example,
例如，

```lua
a = { [f(1)] = g; "x", "y"; x = 1, f(x), [30] = 23; 45 }
```

is equivalent to
等价于

```lua
do
  local t = {}
  t[f(1)] = g
  t[1] = "x"         -- 1st exp
  t[2] = "y"         -- 2nd exp
  t.x = 1            -- t["x"] = 1
  t[3] = f(x)        -- 3rd exp
  t[30] = 23
  t[4] = 45          -- 4th exp
  a = t
end
```

The order of the assignments in a constructor is undefined. (This order would be relevant only when there are repeated keys.)
构造器中赋值的顺序是未定义的。（此顺序仅在存在重复键时才相关。）

If the last field in the list has the form exp and the expression is a multires expression, then all values returned by this expression enter the list consecutively (see §3.4.13).
如果列表中的最后一个字段具有 `exp` 形式且该表达式是多结果表达式，则此表达式返回的所有值连续进入列表（见 §3.4.13）。

The field list can have an optional trailing separator, as a convenience for machine-generated code.
字段列表可以有可选的尾随分隔符，以方便机器生成的代码。

### 3.4.10 – Function Calls（函数调用）

A function call in Lua has the following syntax:
Lua 中的**函数调用**具有以下语法：

```bnf
functioncall ::= prefixexp args
```

In a function call, first prefixexp and args are evaluated. If the value of prefixexp has type function, then this function is called with the given arguments. Otherwise, if present, the prefixexp __call metamethod is called: its first argument is the value of prefixexp, followed by the original call arguments (see §2.4).
在函数调用中，首先求值 `prefixexp` 和 `args`。如果 `prefixexp` 的值类型为 `function`，则使用给定参数调用此函数。否则，如果存在，则调用 `prefixexp` 的 `__call` 元方法：其第一个参数是 `prefixexp` 的值，后跟原始调用参数（见 §2.4）。

The form
形式

```bnf
functioncall ::= prefixexp ':' Name args
```

can be used to emulate methods. A call v:name(args) is syntactic sugar for v.name(v,args), except that v is evaluated only once.
可用于模拟方法。调用 `v:name(args)` 是 `v.name(v,args)` 的语法糖，不同之处在于 `v` 只被求值一次。

Arguments have the following syntax:
参数具有以下语法：

```bnf
args ::= '(' [explist] ')' | tableconstructor | LiteralString
```

All argument expressions are evaluated before the call.
所有参数表达式在调用前求值。

A call of the form f{fields} is syntactic sugar for f({fields}); that is, the argument list is a single new table.
`f{fields}` 形式的调用是 `f({fields})` 的语法糖；也就是说，参数列表是一个单一的新表。

A call of the form f'string' (or f"string" or f[[string]]) is syntactic sugar for f('string'); that is, the argument列表 is a single literal string.
`f'string'`（或 `f"string"` 或 `f[[string]]`）形式的调用是 `f('string')` 的语法糖；也就是说，参数列表是一个单一的字面字符串。

A call of the form return functioncall not in the scope of a to-be-closed variable is called a tail call.
不在待关闭变量作用域内的 `return functioncall` 形式的调用称为**尾调用**（tail call）。

- **尾调用**：`lvm.c` 中 `OP_TAILCALL` 复用当前栈帧，不增加调用深度。

Lua implements proper tail calls (or proper tail recursion): In a tail call, the called function reuses the stack entry of the calling function. Therefore, there is no limit on the number of nested tail calls that a program can execute.
Lua 实现了**适当尾调用**（proper tail calls）（或**适当尾递归**，proper tail recursion）：在尾调用中，被调用函数复用调用函数的栈条目。因此，程序可以执行的嵌套尾调用数量没有限制。

However, a tail call erases any debug information about the calling function.
然而，尾调用会擦除有关调用函数的任何调试信息。

Note that a tail call only happens with a particular syntax, where the return has one single function call as argument, and it is outside the scope of any to-be-closed variable.
请注意，尾调用只发生在特定语法中，其中 `return` 只有一个函数调用作为参数，并且它在任何待关闭变量的作用域之外。

This syntax makes the calling function return exactly the returns of the called function, without any intervening action.
此语法使调用函数准确地返回被调用函数的返回值，没有任何中间操作。

So, none of the following examples are tail calls:
因此，以下示例都不是尾调用：

```lua
return (f(x))        -- results adjusted to 1
return 2 * f(x)      -- result multiplied by 2
return x, f(x)       -- additional results
f(x); return         -- results discarded
return x or f(x)     -- results adjusted to 1
```

### 3.4.11 – Function Definitions（函数定义）

The syntax for function definition is
函数定义的语法为

```bnf
functiondef ::= function funcbody
funcbody ::= '(' [parlist] ')' block end
```

The following syntactic sugar simplifies function definitions:
以下语法糖简化了函数定义：

```bnf
stat ::= function funcname funcbody
stat ::= local function Name funcbody
funcname ::= Name {'.' Name} [':' Name]
```

The statement
语句

```lua
function f () body end
```

translates to
翻译为

```lua
f = function () body end
```

The statement
语句

```lua
function t.a.b.c.f () body end
```

translates to
翻译为

```lua
t.a.b.c.f = function () body end
```

The statement
语句

```lua
local function f () body end
```

translates to
翻译为

```lua
local f; f = function () body end
```

not to
而不是

```lua
local f = function () body end
```

(This only makes a difference when the body of the function contains references to f.)
（这仅在函数体包含对 `f` 的引用时才有区别。）

- **局部函数递归**：`local function f()` 允许 `f` 在函数体内引用自身，因为 `f` 已在局部作用域中声明。

A function definition is an executable expression, whose value has type function.
函数定义是一个可执行表达式，其值类型为 `function`。

When Lua precompiles a chunk, all its function bodies are precompiled too, but they are not created yet.
当 Lua 预编译代码块时，其所有函数体也会被预编译，但尚未创建。

Then, whenever Lua executes the function definition, the function is instantiated (or closed).
然后，每当 Lua 执行函数定义时，函数被**实例化**（instantiated）（或**闭合**，closed）。

This function instance, or closure, is the final value of the expression.
此函数实例，或**闭包**（closure），是表达式的最终值。

- **闭包**：`lfunc.c` 中 `luaF_newCclosure` / `luaF_newLclosure` 创建闭包对象，包含指向 upvalues 的指针。

Parameters act as local variables that are initialized with the argument values:
参数充当用参数值初始化的局部变量：

```bnf
parlist ::= namelist [',' '...'] | '...'
```

When a Lua function is called, it adjusts its list of arguments to the length of its list of parameters (see §3.4.13), unless the function is a variadic function, which is indicated by three dots (...) at the end of its parameter list.
调用 Lua 函数时，它将参数列表调整为其参数列表的长度（见 §3.4.13），除非该函数是**变长函数**（variadic function），其参数列表末尾以三个点（`...`）表示。

A variadic function does not adjust its argument list; instead, it collects all extra arguments and supplies them to the function through a vararg expression, which is also written as three dots.
变长函数不调整其参数列表；相反，它收集所有额外参数并通过**变长参数表达式**（vararg expression）提供给函数，该表达式也写作三个点。

The value of this expression is a list of all actual extra arguments, similar to a function with multiple results (see §3.4.13).
此表达式的值是所有实际额外参数的列表，类似于具有多个结果的函数（见 §3.4.13）。

As an example, consider the following definitions:
例如，考虑以下定义：

```lua
function f(a, b) end
function g(a, b, ...) end
function r() return 1,2,3 end
```

Then, we have the following mapping from arguments to parameters and to the vararg expression:
然后，我们有从参数到参数和变长参数表达式的以下映射：

```
CALL             PARAMETERS

f(3)             a=3, b=nil
f(3, 4)          a=3, b=4
f(3, 4, 5)       a=3, b=4
f(r(), 10)       a=1, b=10
f(r())           a=1, b=2

g(3)             a=3, b=nil, ... -->  (nothing)
g(3, 4)          a=3, b=4,   ... -->  (nothing)
g(3, 4, 5, 8)    a=3, b=4,   ... -->  5  8
g(5, r())        a=5, b=1,   ... -->  2  3
```

Results are returned using the return statement (see §3.3.4).
结果使用 `return` 语句返回（见 §3.3.4）。

If control reaches the end of a function without encountering a return statement, then the function returns with no results.
如果控制到达函数末尾而没有遇到 `return` 语句，则函数不返回任何结果。

There is a system-dependent limit on the number of values that a function may return. This limit is guaranteed to be greater than 1000.
函数可返回的值的数量有系统相关的限制。此限制保证大于 1000。

The colon syntax is used to emulate methods, adding an implicit extra parameter self to the function.
冒号语法用于模拟方法，向函数添加隐式额外参数 `self`。

Thus, the statement
因此，语句

```lua
function t.a.b.c:f (params) body end
```

is syntactic sugar for
是以下语句的语法糖

```lua
t.a.b.c.f = function (self, params) body end
```

### 3.4.12 – Lists of expressions, multiple results, and adjustment（表达式列表、多结果和调整）

Both function calls and vararg expressions can result in multiple values. These expressions are called multires expressions.
函数调用和变长参数表达式都可以产生多个值。这些表达式称为**多结果表达式**（multires expressions）。

When a multires expression is used as the last element of a list of expressions, all results from the expression are added to the list of values produced by the list of expressions.
当多结果表达式用作表达式列表的最后一个元素时，表达式的所有结果都会添加到表达式列表产生的值列表中。

Note that a single expression in a place that expects a list of expressions is the last expression in that (singleton) list.
注意，在期望表达式列表的地方，单个表达式是该（单例）列表中的最后一个表达式。

These are the places where Lua expects a list of expressions:
以下是 Lua 期望表达式列表的地方：

- A `return` statement, for instance `return e1, e2, e3` (see §3.3.4).
- A table constructor, for instance `{e1, e2, e3}` (see §3.4.9).
- The arguments of a function call, for instance `foo(e1, e2, e3)` (see §3.4.10).
- A multiple assignment, for instance `a, b, c = e1, e2, e3` (see §3.3.3).
- A local declaration, for instance `local a, b, c = e1, e2, e3` (see §3.3.7).
- The initial values in a generic `for` loop, for instance `for k in e1, e2, e3 do ... end` (see §3.3.5).

In the last four cases, the list of values from the list of expressions must be adjusted to a specific length: the number of parameters in a call to a non-variadic function (see §3.4.11), the number of variables in a multiple assignment or a local declaration, and exactly four values for a generic `for` loop.
在最后四种情况下，表达式列表的值列表必须**调整**为特定长度：调用非变长函数时的参数数量（见 §3.4.11）、多重赋值或局部声明中的变量数量，以及泛型 `for` 循环的恰好四个值。

The adjustment follows these rules: If there are more values than needed, the extra values are thrown away; if there are fewer values than needed, the list is extended with nil's.
调整遵循以下规则：如果值多于需要，则丢弃多余的值；如果值少于需要，则列表用 `nil` 扩展。

When the list of expressions ends with a multires expression, all results from that expression enter the list of values before the adjustment.
当表达式列表以多结果表达式结束时，该表达式的所有结果在调整之前进入值列表。

When a multires expression is used in a list of expressions without being the last element, or in a place where the syntax expects a single expression, Lua adjusts the result list of that expression to one element.
当多结果表达式在表达式列表中用作非最后一个元素，或在语法期望单个表达式的地方使用时，Lua 将该表达式的结果列表调整为单个元素。

As a particular case, the syntax expects a single expression inside a parenthesized expression; therefore, adding parentheses around a multires expression forces it to produce exactly one result.
作为特例，语法期望括号表达式内的单个表达式；因此，在多结果表达式周围添加括号会强制它恰好产生一个结果。

We seldom need to use a vararg expression in a place where the syntax expects a single expression. (Usually it is simpler to add a regular parameter before the variadic part and use that parameter.)
我们很少需要在语法期望单个表达式的地方使用变长参数表达式。（通常，在变长部分之前添加常规参数并使用该参数更简单。）

When there is such a need, we recommend assigning the vararg expression to a single variable and using that variable in its place.
当有这种需要时，我们建议将变长参数表达式赋给单个变量并在其位置使用该变量。

Here are some examples of uses of multires expressions. In all cases, when the construction needs "the n-th result" and there is no such result, it uses a nil.
以下是多结果表达式用法的示例。在所有情况下，当构造需要 "第 n 个结果" 且没有这样的结果时，它使用 `nil`。

```lua
print(x, f())      -- prints x and all results from f().
print(x, (f()))    -- prints x and the first result from f().
print(f(), x)      -- prints the first result from f() and x.
print(1 + f())     -- prints 1 added to the first result from f().
local x = ...      -- x gets the first vararg argument.
x,y = ...          -- x gets the first vararg argument,
                   -- y gets the second vararg argument.
x,y,z = w, f()     -- x gets w, y gets the first result from f(),
                   -- z gets the second result from f().
x,y,z = f()        -- x gets the first result from f(),
                   -- y gets the second result from f(),
                   -- z gets the third result from f().
x,y,z = f(), g()   -- x gets the first result from f(),
                   -- y gets the first result from g(),
                   -- z gets the second result from g().
x,y,z = (f())      -- x gets the first result from f(), y and z get nil.
return f()         -- returns all results from f().
return x, ...      -- returns x and all received vararg arguments.
return x,y,f()     -- returns x, y, and all results from f().
{f()}              -- creates a list with all results from f().
{...}              -- creates a list with all vararg arguments.
{f(), 5}           -- creates a list with the first result from f() and 5.
```

## 3.5 – Visibility Rules（可见性规则）

Lua is a lexically scoped language.
Lua 是一门词法作用域语言。

The scope of a local variable begins at the first statement after its declaration and lasts until the last non-void statement of the innermost block that includes the declaration.
局部变量的作用域从其声明后的第一个语句开始，持续到包含该声明的最内层块的最后一个非空语句。

(Void statements are labels and empty statements.)
（空语句是标签和空语句。）

Consider the following example:
考虑以下示例：

```lua
x = 10                -- global variable
do                    -- new block
  local x = x         -- new 'x', with value 10
  print(x)            --> 10
  x = x+1
  do                  -- another block
    local x = x+1     -- another 'x'
    print(x)          --> 12
  end
  print(x)            --> 11
end
print(x)              --> 10  (the global one)
```

Notice that, in a declaration like local x = x, the new x being declared is not in scope yet, and so the second x refers to the outside variable.
请注意，在像 `local x = x` 这样的声明中，正在声明的新 `x` 尚未在作用域内，因此第二个 `x` 引用外部变量。

Because of the lexical scoping rules, local variables can be freely accessed by functions defined inside their scope.
由于词法作用域规则，局部变量可以被定义在其作用域内的函数自由访问。

A local variable used by an inner function is called an upvalue (or external local variable, or simply external variable) inside the inner function.
内部函数使用的局部变量在内部函数中称为**上值**（upvalue）（或外部局部变量，或简称外部变量）。

- **upvalue**：`lfunc.c` 中通过 `UpVal` 结构实现，闭包通过指针引用外部局部变量。

Notice that each execution of a local statement defines new local variables.
请注意，`local` 语句的每次执行都会定义新的局部变量。

Consider the following example:
考虑以下示例：

```lua
a = {}
local x = 20
for i = 1, 10 do
  local y = 0
  a[i] = function () y = y + 1; return x + y end
end
```

The loop creates ten closures (that is, ten instances of the anonymous function). Each of these closures uses a different y variable, while all of them share the same x.
循环创建十个闭包（即匿名函数的十个实例）。这些闭包中的每一个使用不同的 `y` 变量，而它们都共享相同的 `x`。

---

# 4 – The Application Program Interface（应用程序接口）

This section describes the C API for Lua, that is, the set of C functions available to the host program to communicate with Lua.
本节描述 Lua 的 **C API**，即宿主程序可用于与 Lua 通信的 **C 函数**集。

- **C API**：Lua 的核心暴露接口，定义在 `lua.h` 中，实现在 `lapi.c` 中。

All API functions and related types and constants are declared in the header file lua.h.
所有 API 函数以及相关类型和常量都在头文件 `lua.h` 中声明。

Even when we use the term "function", any facility in the API may be provided as a macro instead.
即使我们使用术语 "函数"，API 中的任何工具也可能以宏的形式提供。

Except where stated otherwise, all such macros use each of their arguments exactly once (except for the first argument, which is always a Lua state), and so do not generate any hidden side-effects.
除非另有说明，所有此类宏对每个参数恰好使用一次（除了第一个参数，它始终是 Lua 状态），因此不会产生任何隐藏的副作用。

As in most C libraries, the Lua API functions do not check their arguments for validity or consistency.
与大多数 **C 库**一样，Lua API 函数不检查其参数的有效性或一致性。

However, you can change this behavior by compiling Lua with the macro LUA_USE_APICHECK defined.
但是，你可以通过定义宏 `LUA_USE_APICHECK` 编译 Lua 来更改此行为。

The Lua library is fully reentrant: it has no global variables.
Lua 库是完全可重入的：它没有全局变量。

It keeps all information it needs in a dynamic structure, called the Lua state.
它将所需的所有信息保存在一个称为 **Lua 状态**（Lua state）的动态结构中。

Each Lua state has one or more threads, which correspond to independent, cooperative lines of execution.
每个 Lua 状态有一个或多个线程，对应于独立的、协作的执行线。

The type lua_State (despite its name) refers to a thread.
类型 `lua_State`（尽管名称如此）指的是线程。

(Indirectly, through the thread, it also refers to the Lua state associated to the thread.)
（间接地，通过线程，它也指与线程关联的 Lua 状态。）

A pointer to a thread must be passed as the first argument to every function in the library, except to lua_newstate, which creates a Lua state from scratch and returns a pointer to the main thread in the new state.
指向线程的指针必须作为第一个参数传递给库中的每个函数，除了 `lua_newstate`，它从头创建 Lua 状态并返回指向新状态中主线程的指针。

- **实现**（implementation）：`lua_State` 在 `lstate.h` 中定义为 per-thread 结构，包含栈顶 `top`、当前调用信息 `ci`、栈底 `stack` 等；全局状态 `global_State` 由所有线程共享，可通过 `G(L)` 访问。

## 4.1 – The Stack（栈）

Lua uses a virtual stack to pass values to and from C.
Lua 使用**虚拟栈**（virtual stack）向 C 传递值和从 C 接收值。

- **虚拟栈**：每个 C 函数调用获得独立栈，参数和返回值通过栈交换（见 `lapi.c`）。

Each element in this stack represents a Lua value (nil, number, string, etc.).
此栈中的每个元素表示一个 Lua 值（`nil`、数字、字符串等）。

Functions in the API can access this stack through the Lua state parameter that they receive.
API 中的函数可以通过接收的 Lua 状态参数访问此栈。

Whenever Lua calls C, the called function gets a new stack, which is independent of previous stacks and of stacks of C functions that are still active.
每当 Lua 调用 C 时，被调用函数获得一个新栈，该栈独立于先前的栈和仍然活动的 **C 函数**的栈。

This stack initially contains any arguments to the C function and it is where the C function can store temporary Lua values and must push its results to be returned to the caller (see lua_CFunction).
此栈最初包含 **C 函数**的任何参数，也是 **C 函数**可以存储临时 Lua 值并必须推送其结果以返回给调用者的地方（见 `lua_CFunction`）。

- **实现**（implementation）：栈在 `lstate.c:stack_init` 中分配，初始大小为 `BASIC_STACK_SIZE + EXTRA_STACK`；每个槽位是 `StackValue`（即 `TValue`），`L->top.p` 始终指向第一个空闲槽位。

For convenience, most query operations in the API do not follow a strict stack discipline. Instead, they can refer to any element in the stack by using an index:
为了方便起见，API 中的大多数查询操作不遵循严格的栈规则。相反，它们可以通过使用**索引**（index）引用栈中的任何元素：

A positive index represents an absolute stack position, starting at 1 as the bottom of the stack; a negative index represents an offset relative to the top of the stack.
正索引表示绝对栈位置，从 1 开始作为栈底；负索引表示相对于栈顶的偏移。

More specifically, if the stack has n elements, then index 1 represents the first element (that is, the element that was pushed onto the stack first) and index n represents the last element; index -1 also represents the last element (that is, the element at the top) and index -n represents the first element.
更具体地说，如果栈有 n 个元素，则索引 1 表示第一个元素（即最先被推入栈的元素），索引 n 表示最后一个元素；索引 -1 也表示最后一个元素（即栈顶的元素），索引 -n 表示第一个元素。

### 4.1.1 – Stack Size（栈大小）

When you interact with the Lua API, you are responsible for ensuring consistency. In particular, you are responsible for controlling stack overflow.
当你与 Lua API 交互时，你负责确保一致性。特别地，你负责控制栈溢出。

When you call any API function, you must ensure the stack has enough room to accommodate the results.
当你调用任何 API 函数时，必须确保栈有足够的空间容纳结果。

There is one exception to the above rule: When you call a Lua function without a fixed number of results (see lua_call), Lua ensures that the stack has enough space for all results.
上述规则有一个例外：当你调用没有固定结果数量的 Lua 函数时（见 `lua_call`），Lua 确保栈有足够的空间容纳所有结果。

However, it does not ensure any extra space. So, before pushing anything on the stack after such a call you should use lua_checkstack.
但是，它不确保任何额外空间。因此，在此类调用后向栈推送任何内容之前，你应该使用 `lua_checkstack`。

Whenever Lua calls C, it ensures that the stack has space for at least LUA_MINSTACK extra elements; that is, you can safely push up to LUA_MINSTACK values into it.
每当 Lua 调用 C 时，它确保栈至少有 `LUA_MINSTACK` 个额外元素的空间；也就是说，你可以安全地向其中推送最多 `LUA_MINSTACK` 个值。

LUA_MINSTACK is defined as 20, so that usually you do not have to worry about stack space unless your code has loops pushing elements onto the stack.
`LUA_MINSTACK` 定义为 20，因此通常你不必担心栈空间，除非你的代码有将元素推入栈的循环。

Whenever necessary, you can use the function lua_checkstack to ensure that the stack has enough space for pushing new elements.
每当必要时，你可以使用函数 `lua_checkstack` 确保栈有足够的空间用于推送新元素。

### 4.1.2 – Valid and Acceptable Indices（有效和可接受索引）

Any function in the API that receives stack indices works only with valid indices or acceptable indices.
API 中接收栈索引的任何函数仅适用于**有效索引**（valid indices）或**可接受索引**（acceptable indices）。

A valid index is an index that refers to a position that stores a modifiable Lua value. It comprises stack indices between 1 and the stack top (1 <= abs(index) <= top) plus pseudo-indices, which represent some positions that are accessible to C code but that are not in the stack.
**有效索引**是指引用存储可修改 Lua 值的位置的索引。它包括栈顶之间的栈索引（`1 <= abs(index) <= top`）加上**伪索引**（pseudo-indices），伪索引表示 **C 代码**可访问但不在栈中的某些位置。

- **伪索引**：`LUA_REGISTRYINDEX` 等，通过 `lua_upvalueindex(n)` 访问上值。

Pseudo-indices are used to access the registry (see §4.5) and the upvalues of a C function (see §4.2).
伪索引用于访问注册表（见 §4.5）和 **C 函数**的上值（见 §4.2）。

Functions that do not need a specific mutable position, but only a value (e.g., query functions), can be called with acceptable indices.
不需要特定可变位置而只需要值的函数（例如查询函数）可以使用**可接受索引**调用。

An acceptable index can be any valid index, but it also can be any positive index after the stack top within the space allocated for the stack, that is, indices up to the stack size.
**可接受索引**可以是任何有效索引，但它也可以是栈顶之后栈分配空间内的任何正索引，即直到栈大小的索引。

(Note that 0 is never an acceptable index.)
（注意，0 从来不是可接受索引。）

Indices to upvalues (see §4.2) greater than the real number of upvalues in the current C function are also acceptable (but invalid).
大于当前 **C 函数**中实际的上值数量的上值索引（见 §4.2）也是可接受的（但无效）。

Except when noted otherwise, functions in the API work with acceptable indices.
除非另有说明，API 中的函数使用可接受索引工作。

Acceptable indices serve to avoid extra tests against the stack top when querying the stack.
可接受索引用于在查询栈时避免对栈顶的额外测试。

For instance, a C function can query its third argument without the need to check whether there is a third argument, that is, without the need to check whether 3 is a valid index.
例如，**C 函数**可以查询其第三个参数，而无需检查是否有第三个参数，即无需检查 3 是否为有效索引。

For functions that can be called with acceptable indices, any non-valid index is treated as if it contains a value of a virtual type LUA_TNONE, which behaves like a nil value.
对于可以使用可接受索引调用的函数，任何非有效索引都被视为包含虚拟类型 `LUA_TNONE` 的值，其行为类似于 `nil` 值。

### 4.1.3 – Pointers to strings（字符串指针）

Several functions in the API return pointers (const char*) to Lua strings in the stack.
API 中的几个函数返回指向栈中 Lua 字符串的指针（`const char*`）。

In general, Lua's garbage collection can free or move internal memory and then invalidate pointers to internal strings.
通常，Lua 的垃圾回收可以释放或移动内部内存，从而使指向内部字符串的指针失效。

To allow a safe use of these pointers, the API guarantees that any pointer to a string in a stack index is valid while the string value at that index is not removed from the stack.
为了允许安全使用这些指针，API 保证只要该索引处的字符串值未从栈中移除，指向栈索引中字符串的任何指针就是有效的。

(It can be moved to another index, though.)
（但是，它可以移动到另一个索引。）

When the index is a pseudo-index (referring to an upvalue), the pointer is valid while the corresponding call is active and the corresponding upvalue is not modified.
当索引是伪索引（引用上值）时，只要相应的调用处于活动状态且相应的上值未被修改，指针就是有效的。

Some functions in the debug interface also return pointers to strings, namely lua_getlocal, lua_getupvalue, lua_setlocal, and lua_setupvalue.
调试接口中的一些函数也返回指向字符串的指针，即 `lua_getlocal`、`lua_getupvalue`、`lua_setlocal` 和 `lua_setupvalue`。

For these functions, the pointer is guaranteed to be valid while the caller function is active and the given closure (if one was given) is in the stack.
对于这些函数，只要调用者函数处于活动状态且给定的闭包（如果给定）在栈中，指针就保证有效。

Except for these guarantees, the garbage collector is free to invalidate any pointer to internal strings.
除了这些保证之外，垃圾回收器可以随意使任何指向内部字符串的指针失效。

- **实现**（implementation）：`TString` 对象内嵌字符数组（短字符串直接内嵌，长字符串通过 `extra` 指向外部块），因此 `getstr` 返回的指针在对象未被 GC 回收前始终有效；栈移动仅复制 `TValue` 引用，不改变字符串对象本身。

## 4.2 – C Closures（C 闭包）

When a C function is created, it is possible to associate some values with it, thus creating a C closure (see lua_pushcclosure); these values are called upvalues and are accessible to the function whenever it is called.
创建 **C 函数**时，可以将某些值与其关联，从而创建 **C 闭包**（见 `lua_pushcclosure`）；这些值称为**上值**（upvalues），函数在每次被调用时都可以访问它们。

Whenever a C function is called, its upvalues are located at specific pseudo-indices. These pseudo-indices are produced by the macro lua_upvalueindex.
每当调用 **C 函数**时，其上值位于特定的伪索引处。这些伪索引由宏 `lua_upvalueindex` 生成。

The first upvalue associated with a function is at index lua_upvalueindex(1), and so on.
与函数关联的第一个上值在索引 `lua_upvalueindex(1)` 处，依此类推。

Any access to lua_upvalueindex(n), where n is greater than the number of upvalues of the current function (but not greater than 256, which is one plus the maximum number of upvalues in a closure), produces an acceptable but invalid index.
对 `lua_upvalueindex(n)` 的任何访问，其中 n 大于当前函数的上值数量（但不大于 256，即闭包中最大上值数加一），会产生可接受但无效的索引。

A C closure can also change the values of its corresponding upvalues.
**C 闭包**还可以更改其相应上值的值。

- **实现**（implementation）：C 闭包在 `lfunc.c:luaF_newCclosure` 中创建，类型为 `CClosure`，其 `upvalue` 数组直接内嵌在对象尾部；`lua_pushcclosure` 从栈中弹出值并复制到该数组。

## 4.3 – Registry（注册表）

Lua provides a registry, a predefined table that can be used by any C code to store whatever Lua values it needs to store.
Lua 提供了**注册表**（registry），一个预定义的表，任何 **C 代码**都可以使用它来存储需要存储的任何 Lua 值。

The registry table is always accessible at pseudo-index LUA_REGISTRYINDEX.
注册表表始终可在伪索引 `LUA_REGISTRYINDEX` 处访问。

Any C library can store data into this table, but it must take care to choose keys that are different from those used by other libraries, to avoid collisions.
任何 **C 库**都可以将数据存储到此表中，但必须注意选择与**其他库**使用的键不同的键，以避免冲突。

Typically, you should use as key a string containing your library name, or a light userdata with the address of a C object in your code, or any Lua object created by your code.
通常，你应该使用包含库名称的字符串作为键，或使用指向代码中 **C 对象**地址的轻量 userdata，或代码创建的任何 Lua 对象。

As with variable names, string keys starting with an underscore followed by uppercase letters are reserved for Lua.
与变量名称一样，以下划线开头后跟大写字母的字符串键保留给 Lua。

The integer keys in the registry are used by the reference mechanism (see luaL_ref) and by some predefined values.
注册表中的整数键由引用机制（见 `luaL_ref`）和一些预定义值使用。

Therefore, integer keys in the registry must not be used for other purposes.
因此，注册表中的整数键不得用于其他目的。

When you create a new Lua state, its registry comes with some predefined values. These predefined values are indexed with integer keys defined as constants in lua.h.
创建新的 Lua 状态时，其注册表带有一些预定义值。这些预定义值用 `lua.h` 中定义为常量的整数键索引。

- **实现**（implementation）：注册表是 `global_State` 中的 `l_registry` 字段（`TValue`），本质上是一个表；`LUA_REGISTRYINDEX` 伪索引通过 `index2value` 映射到 `&G(L)->l_registry`。

The following constants are defined:
定义了以下常量：

- **LUA_RIDX_MAINTHREAD**：在此索引处，注册表具有状态的主线程。（主线程是与状态一起创建的线程。）
- **LUA_RIDX_GLOBALS**：在此索引处，注册表具有全局环境。

## 4.4 – Error Handling in C（C 中的错误处理）

Internally, Lua uses the C longjmp facility to handle errors. (Lua will use exceptions if you compile it as C++; search for LUAI_THROW in the source code for details.)
在内部，Lua 使用 C `longjmp` 工具来处理错误。（如果你将 Lua 编译为 C++，Lua 将使用异常；在源代码中搜索 `LUAI_THROW` 了解详情。）

- **longjmp**：`ldo.c` 中 `luaD_throw` 使用 `longjmp` 跳转到最近的恢复点。

When Lua faces any error, such as a memory allocation error or a type error, it raises an error; that is, it does a long jump.
当 Lua 遇到任何错误时，例如内存分配错误或类型错误，它会**引发**错误；也就是说，它执行长跳转。

A protected environment uses setjmp to set a recovery point; any error jumps to the most recent active recovery point.
**受保护环境**使用 `setjmp` 设置恢复点；任何错误都会跳转到最近的活动恢复点。

Inside a C function you can raise an error explicitly by calling lua_error.
在 **C 函数**中，你可以通过调用 `lua_error` 显式引发错误。

Most functions in the API can raise an error, for instance due to a memory allocation error.
API 中的大多数函数都可能引发错误，例如由于内存分配错误。

The documentation for each function indicates whether it can raise errors.
每个函数的文档指示它是否可以引发错误。

If an error happens outside any protected environment, Lua calls a panic function (see lua_atpanic) and then calls abort, thus exiting the host application.
如果错误发生在任何受保护环境之外，Lua 会调用**恐慌函数**（panic function）（见 `lua_atpanic`），然后调用 `abort`，从而退出宿主应用程序。

Your panic function can avoid this exit by never returning (e.g., doing a long jump to your own recovery point outside Lua).
你的恐慌函数可以通过永不返回来避免此退出（例如，执行长跳转到你自己的 Lua 外部恢复点）。

The panic function, as its name implies, is a mechanism of last resort. Programs should avoid it.
恐慌函数，顾名思义，是最后的手段。程序应避免使用它。

- **实现**（implementation）：`ldo.c:luaD_throw` 是错误传播核心；若线程有 `errorJmp`（`lua_longjmp` 链）则 `longjmp` 到恢复点，否则沿主线程链传播，最终无处理者时调用 `panic` 并 `abort`。

As a general rule, when a C function is called by Lua with a Lua state, it can do whatever it wants on that Lua state, as it should be already protected.
作为一般规则，当 **C 函数**被 Lua 用 Lua 状态调用时，它可以在该 Lua 状态上执行任何操作，因为它应该已经受到保护。

However, when C code operates on other Lua states (e.g., a Lua-state argument to the function, a Lua state stored in the registry, or the result of lua_newthread), it should use them only in API calls that cannot raise errors.
然而，当 **C 代码**操作其他 Lua 状态（例如，函数的 Lua 状态参数、存储在注册表中的 Lua 状态或 `lua_newthread` 的结果）时，它应该只在不会引发错误的 API 调用中使用它们。

The panic function runs as if it were a message handler (see §2.3); in particular, the error object is on the top of the stack.
恐慌函数的运行方式就像消息处理程序（见 §2.3）一样；特别地，错误对象在栈顶。

However, there is no guarantee about stack space.
但是，栈空间没有保证。

To push anything on the stack, the panic function must first check the available space (see §4.1.1).
要在栈上推送任何内容，恐慌函数必须首先检查可用空间（见 §4.1.1）。

### 4.4.1 – Status Codes（状态码）

Several functions that report errors in the API use the following status codes to indicate different kinds of errors or other conditions:
API 中报告错误的几个函数使用以下状态码来指示不同类型的错误或其他条件：

- **LUA_OK** (0)：无错误。
- **LUA_ERRRUN**：运行时错误。
- **LUA_ERRMEM**：内存分配错误。对于此类错误，Lua 不调用消息处理程序。
- **LUA_ERRERR**：运行消息处理程序时出错。
- **LUA_ERRSYNTAX**：预编译期间的语法错误。
- **LUA_YIELD**：线程（协程）让出。
- **LUA_ERRFILE**：与文件相关的错误；例如，无法打开或读取文件。

These constants are defined in the header file lua.h.
这些常量在头文件 `lua.h` 中定义。

## 4.5 – Handling Yields in C（在 C 中处理让出）

Internally, Lua uses the C longjmp facility to yield a coroutine.
在内部，Lua 使用 C `longjmp` 工具来让出协程。

Therefore, if a C function foo calls an API function and this API function yields (directly or indirectly by calling another function that yields), Lua cannot return to foo any more, because the longjmp removes its frame from the C stack.
因此，如果 **C 函数** `foo` 调用 API 函数并且此 API 函数让出（直接或通过调用另一个让出的函数间接让出），Lua 无法再返回到 `foo`，因为 `longjmp` 从 **C 栈**中移除了其帧。

To avoid this kind of problem, Lua raises an error whenever it tries to yield across an API call, except for three functions: lua_yieldk, lua_callk, and lua_pcallk.
为避免此类问题，每当 Lua 尝试跨 API 调用让出时，Lua 都会引发错误，除了三个函数：`lua_yieldk`、`lua_callk` 和 `lua_pcallk`。

All those functions receive a continuation function (as a parameter named k) to continue execution after a yield.
所有这些函数都接收一个**延续函数**（continuation function）（作为名为 `k` 的参数），以在让出后继续执行。

We need to set some terminology to explain continuations.
我们需要设置一些术语来解释延续。

We have a C function called from Lua which we will call the original function.
我们有一个从 Lua 调用的 **C 函数**，我们称之为**原始函数**（original function）。

This original function then calls one of those three functions in the C API, which we will call the callee function, that then yields the current thread.
此原始函数随后调用 C API 中的这三个函数之一，我们称之为**被调用函数**（callee function），然后让出当前线程。

This can happen when the callee function is lua_yieldk, or when the callee function is either lua_callk or lua_pcallk and the function called by them yields.
当被调用函数是 `lua_yieldk` 时，或当被调用函数是 `lua_callk` 或 `lua_pcallk` 且它们调用的函数让出时，就会发生这种情况。

Suppose the running thread yields while executing the callee function. After the thread resumes, it eventually will finish running the callee function. However, the callee function cannot return to the original function, because its frame in the C stack was destroyed by the yield.
假设运行线程在执行被调用函数时让出。线程恢复后，它最终会完成运行被调用函数。然而，被调用函数无法返回到原始函数，因为其在 **C 栈**中的帧已被让出破坏。

Instead, Lua calls a continuation function, which was given as an argument to the callee function. As the name implies, the continuation function should continue the task of the original function.
相反，Lua 调用**延续函数**，该函数作为参数传递给被调用函数。顾名思义，延续函数应该继续原始函数的任务。

As an illustration, consider the following function:
作为说明，考虑以下函数：

```c
int original_function (lua_State *L) {
  ...     /* code 1 */
  status = lua_pcall(L, n, m, h);  /* calls Lua */
  ...     /* code 2 */
}
```

Now we want to allow the Lua code being run by lua_pcall to yield.
现在我们希望允许 `lua_pcall` 运行的 Lua 代码让出。

First, we can rewrite our function like here:
首先，我们可以像这样重写我们的函数：

```c
int k (lua_State *L, int status, lua_KContext ctx) {
  ...  /* code 2 */
}

int original_function (lua_State *L) {
  ...     /* code 1 */
  return k(L, lua_pcall(L, n, m, h), ctx);
}
```

In the above code, the new function k is a continuation function (with type lua_KFunction), which should do all the work that the original function was doing after calling lua_pcall.
在上面的代码中，新函数 `k` 是**延续函数**（类型为 `lua_KFunction`），它应该完成原始函数在调用 `lua_pcall` 后所做的所有工作。

Now, we must inform Lua that it must call k if the Lua code being executed by lua_pcall gets interrupted in some way (errors or yielding), so we rewrite the code as here, replacing lua_pcall by lua_pcallk:
现在，我们必须通知 Lua，如果 `lua_pcall` 正在执行的 Lua 代码以某种方式被中断（错误或让出），它必须调用 `k`，因此我们将代码重写为这里，用 `lua_pcallk` 替换 `lua_pcall`：

```c
int original_function (lua_State *L) {
  ...     /* code 1 */
  return k(L, lua_pcallk(L, n, m, h, ctx2, k), ctx1);
}
```

Note the external, explicit call to the continuation: Lua will call the continuation only if needed, that is, in case of errors or resuming after a yield.
注意对外部延续的显式调用：Lua 只会在需要时调用延续，即在错误或让出后恢复时。

If the called function returns normally without ever yielding, lua_pcallk (and lua_callk) will also return normally.
如果被调用函数正常返回而没有让出，`lua_pcallk`（和 `lua_callk`）也会正常返回。

(Of course, instead of calling the continuation in that case, you can do the equivalent work directly inside the original function.)
（当然，在这种情况下，你可以直接在原始函数内执行等效工作，而不是调用延续。）

Besides the Lua state, the continuation function has two other parameters: the final status of the call and the context value (ctx) that was passed originally to lua_pcallk.
除了 Lua 状态之外，延续函数还有两个其他参数：调用的最终状态和最初传递给 `lua_pcallk` 的上下文值（`ctx`）。

Lua does not use this context value; it only passes this value from the original function to the continuation function.
Lua 不使用此上下文值；它只将此值从原始函数传递给延续函数。

For lua_pcallk, the status is the same value that would be returned by lua_pcallk, except that it is LUA_YIELD when being executed after a yield (instead of LUA_OK).
对于 `lua_pcallk`，状态是与 `lua_pcallk` 返回的值相同的值，不同之处在于在让出后执行时它是 `LUA_YIELD`（而不是 `LUA_OK`）。

For lua_yieldk and lua_callk, the status is always LUA_YIELD when Lua calls the continuation.
对于 `lua_yieldk` 和 `lua_callk`，当 Lua 调用延续时，状态始终为 `LUA_YIELD`。

(For these two functions, Lua will not call the continuation in case of errors, because they do not handle errors.)
（对于这两个函数，Lua 不会在出错时调用延续，因为它们不处理错误。）

Similarly, when using lua_callk, you should call the continuation function with LUA_OK as the status.
类似地，使用 `lua_callk` 时，你应该以 `LUA_OK` 作为状态调用延续函数。

(For lua_yieldk, there is not much point in calling directly the continuation function, because lua_yieldk usually does not return.)
（对于 `lua_yieldk`，直接调用延续函数没有太大意义，因为 `lua_yieldk` 通常不会返回。）

Lua treats the continuation function as if it were the original function.
Lua 将延续函数视为原始函数。

The continuation function receives the same Lua stack from the original function, in the same state it would be if the callee function had returned.
延续函数从原始函数接收相同的 Lua 栈，其状态与被调用函数已返回时相同。

(For instance, after a lua_callk the function and its arguments are removed from the stack and replaced by the results from the call.)
（例如，在 `lua_callk` 之后，函数及其参数从栈中移除并由调用的结果替换。）

It also has the same upvalues.
它也有相同的上值。

Whatever it returns is handled by Lua as if it were the return of the original function.
它返回的任何内容都由 Lua 处理，就像它是原始函数的返回一样。

- **实现**（implementation）：延续机制的核心在 `ldo.c` 的 `resume` 和 `luaD_call` 中；`CallInfo` 的 `u.c.k` 和 `u.c.ctx` 保存延续函数与上下文，让出后 `longjmp` 返回到 `lua_resume`，再由 C 调用者显式调用 `k`。

## 4.6 – Functions and Types（函数和类型）

Here we list all functions and types from the C API in alphabetical order.
这里我们按字母顺序列出 **C API** 中的所有函数和类型。

Each function has an indicator like this: [-o, +p, x]
每个函数都有一个如下指示器：`[-o, +p, x]`

The first field, o, is how many elements the function pops from the stack.
第一个字段 `o` 是函数从栈中弹出的元素数量。

The second field, p, is how many elements the function pushes onto the stack.
第二个字段 `p` 是函数推入栈中的元素数量。

(Any function always pushes its results after popping its arguments.)
（任何函数总是在弹出参数后推送其结果。）

A field in the form x|y means the function can push (or pop) x or y elements, depending on the situation; an interrogation mark ? means that we cannot know how many elements the function pops/pushes by looking only at its arguments.
`x|y` 形式的字段表示函数可以推送（或弹出）`x` 或 `y` 个元素，具体取决于情况；问号 `?` 表示我们无法仅通过查看其参数就知道函数弹出/推送多少元素。

The third field, x, tells whether the function may raise errors:
第三个字段 `x` 指示函数是否可能引发错误：

- `-` 表示函数永远不会引发任何错误；
- `m` 表示函数可能只引发内存不足错误；
- `v` 表示函数可能引发文本中解释的错误；
- `e` 表示函数可以直接或通过元方法运行任意 Lua 代码，因此可能引发任何错误。

### lua_absindex

`int lua_absindex (lua_State *L, int idx);`

[-0, +0, -]

Converts the acceptable index idx into an equivalent absolute index (that is, one that does not depend on the stack size).
将**可接受索引** `idx` 转换为等效的**绝对索引**（即不依赖于栈大小的索引）。

- **绝对索引**：始终从 1 开始计数，不受栈操作影响。

### lua_Alloc

`typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);`

The type of the memory-allocation function used by Lua states.
Lua 状态使用的**内存分配函数**的类型。

The allocator function must provide a functionality similar to realloc, but not exactly the same.
分配器函数必须提供类似于 `realloc` 的功能，但不完全相同。

Its arguments are ud, an opaque pointer passed to lua_newstate; ptr, a pointer to the block being allocated/reallocated/freed; osize, the original size of the block or some code about what is being allocated; and nsize, the new size of the block.
其参数为：`ud`，传递给 `lua_newstate` 的不透明指针；`ptr`，指向正在分配/重新分配/释放的块的指针；`osize`，块的原始大小或有关正在分配的内容的某些代码；以及 `nsize`，块的新大小。

When ptr is not NULL, osize is the size of the block pointed by ptr, that is, the size given when it was allocated or reallocated.
当 `ptr` 不为 `NULL` 时，`osize` 是 `ptr` 指向的块的大小，即分配或重新分配时给定的大小。

When ptr is NULL, osize encodes the kind of object that Lua is allocating.
当 `ptr` 为 `NULL` 时，`osize` 编码 Lua 正在分配的对象类型。

osize is any of LUA_TSTRING, LUA_TTABLE, LUA_TFUNCTION, LUA_TUSERDATA, or LUA_TTHREAD when (and only when) Lua is creating a new object of that type.
`osize` 是 `LUA_TSTRING`、`LUA_TTABLE`、`LUA_TFUNCTION`、`LUA_TUSERDATA` 或 `LUA_TTHREAD` 中的任何一个，当（且仅当）Lua 正在创建该类型的新对象时。

When osize is some other value, Lua is allocating memory for something else.
当 `osize` 是其他值时，Lua 正在为其他内容分配内存。

Lua assumes the following behavior from the allocator function:
Lua 假设分配器函数具有以下行为：

When nsize is zero, the allocator must behave like free and then return NULL.
当 `nsize` 为零时，分配器必须表现得像 `free`，然后返回 `NULL`。

When nsize is not zero, the allocator must behave like realloc. In particular, the allocator returns NULL if and only if it cannot fulfill the request.
当 `nsize` 不为零时，分配器必须表现得像 `realloc`。特别地，当且仅当分配器无法满足请求时，它才返回 `NULL`。

Here is a simple implementation for the allocator function. It is used in the auxiliary library by luaL_newstate.
以下是分配器函数的简单实现。它在辅助库中由 `luaL_newstate` 使用。

```c
static void *l_alloc (void *ud, void *ptr, size_t osize,
                                           size_t nsize) {
  (void)ud;  (void)osize;  /* not used */
  if (nsize == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, nsize);
}
```

Note that ISO C ensures that free(NULL) has no effect and that realloc(NULL,size) is equivalent to malloc(size).
请注意，ISO C 确保 `free(NULL)` 无效，并且 `realloc(NULL,size)` 等同于 `malloc(size)`。

### lua_arith

`void lua_arith (lua_State *L, int op);`

[-(2|1), +1, e]

Performs an arithmetic or bitwise operation over the two values (or one, in the case of negations) at the top of the stack, with the value on the top being the second operand, pops these values, and pushes the result of the operation.
对栈顶的两个值（或一个值，在取反的情况下）执行算术或按位运算，栈顶的值是第二个操作数，弹出这些值，并推送运算结果。

The function follows the semantics of the corresponding Lua operator (that is, it may call metamethods).
该函数遵循相应 Lua 运算符的语义（也就是说，它可能会调用元方法）。

The value of op must be one of the following constants:
`op` 的值必须是以下常量之一：

- **LUA_OPADD**：执行加法（`+`）
- **LUA_OPSUB**：执行减法（`-`）
- **LUA_OPMUL**：执行乘法（`*`）
- **LUA_OPDIV**：执行浮点除法（`/`）
- **LUA_OPIDIV**：执行向下取整除法（`//`）
- **LUA_OPMOD**：执行取模（`%`）
- **LUA_OPPOW**：执行幂运算（`^`）
- **LUA_OPUNM**：执行数学取反（一元 `-`）
- **LUA_OPBNOT**：执行按位非（`~`）
- **LUA_OPBAND**：执行按位与（`&`）
- **LUA_OPBOR**：执行按位或（`|`）
- **LUA_OPBXOR**：执行按位异或（`~`）
- **LUA_OPSHL**：执行左移（`<<`）
- **LUA_OPSHR**：执行右移（`>>`）


- **实现**（implementation）：`lapi.c:lua_arith` 对二元操作检查栈顶有两个操作数，一元操作则复制栈顶作为第二个假操作数；最终调用 `luaO_arith`（或 `luaV_mod` 等）计算结果并替换栈顶。

### lua_atpanic

`lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf);`

[-0, +0, -]

Sets a new panic function and returns the old one (see §4.4).
设置新的恐慌函数并返回旧的恐慌函数（见 §4.4）。

- **实现**（implementation）：`lapi.c:lua_atpanic` 直接读写 `global_State->panic` 指针，不涉及栈操作；当无保护环境中发生错误时，`luaD_throw` 会在无恢复点后调用此函数。

### lua_call

`void lua_call (lua_State *L, int nargs, int nresults);`

[-(nargs+1), +nresults, e]

Calls a function. Like regular Lua calls, lua_call respects the __call metamethod. So, here the word "function" means any callable value.
调用函数。与普通 Lua 调用一样，`lua_call` 遵守 `__call` 元方法。因此，这里的"函数"一词指任何可调用值。

To do a call you must use the following protocol: first, the function to be called is pushed onto the stack; then, the arguments to the call are pushed in direct order; that is, the first argument is pushed first. Finally you call lua_call; nargs is the number of arguments that you pushed onto the stack.
进行调用时，必须使用以下协议：首先，将要调用的函数推入栈；然后，按直接顺序推入调用参数；也就是说，第一个参数最先推入。最后调用 `lua_call`；`nargs` 是你推入栈中的参数数量。

When the function returns, all arguments and the function value are popped and the call results are pushed onto the stack. The number of results is adjusted to nresults, unless nresults is LUA_MULTRET.
当函数返回时，所有参数和函数值都被弹出，调用结果被推入栈中。结果数量调整为 `nresults`，除非 `nresults` 是 `LUA_MULTRET`。

In this case, all results from the function are pushed; Lua takes care that the returned values fit into the stack space, but it does not ensure any extra space in the stack.
在这种情况下，函数的所有结果都被推入；Lua 确保返回值适合栈空间，但不确保栈中的任何额外空间。

The function results are pushed onto the stack in direct order (the first result is pushed first), so that after the call the last result is on the top of the stack.
函数结果按直接顺序推入栈（第一个结果最先推入），因此调用后最后一个结果在栈顶。

Any error while calling and running the function is propagated upwards (with a longjmp).
调用和运行函数期间的任何错误都会向上传播（通过 `longjmp`）。

The following example shows how the host program can do the equivalent to this Lua code:
以下示例展示宿主程序如何执行与以下 Lua 代码等效的操作：

```lua
a = f("how", t.x, 14)
```

Here it is in C:
以下是 C 代码：

```c
lua_getglobal(L, "f");                  /* function to be called */
lua_pushliteral(L, "how");                       /* 1st argument */
lua_getglobal(L, "t");                    /* table to be indexed */
lua_getfield(L, -1, "x");        /* push result of t.x (2nd arg) */
lua_remove(L, -2);                  /* remove 't' from the stack */
lua_pushinteger(L, 14);                          /* 3rd argument */
lua_call(L, 3, 1);     /* call 'f' with 3 arguments and 1 result */
lua_setglobal(L, "a");                         /* set global 'a' */
```

Note that the code above is balanced: at its end, the stack is back to its original configuration. This is considered good programming practice.
请注意，上面的代码是**平衡的**：在其末尾，栈恢复到原始配置。这被认为是良好的编程实践。

- **实现**（implementation）：`lapi.c:lua_call` 本质是 `lua_callk` 且 `k==NULL` 的包装；它定位函数在栈中的位置后调用 `luaD_callnoyield`，再通过 `adjustresults` 调整结果数量。

### lua_callk

`void lua_callk (lua_State *L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k);`

[-(nargs + 1), +nresults, e]

This function behaves exactly like lua_call, but allows the called function to yield (see §4.5).
此函数的行为与 `lua_call` 完全相同，但允许被调用函数让出（见 §4.5）。

- **实现**（implementation）：`lapi.c:lua_callk` 若提供了延续 `k` 且线程可让出，则将 `k` 和 `ctx` 保存到当前 `CallInfo` 的 `u.c` 字段，然后调用 `luaD_call`；否则走 `luaD_callnoyield`。

### lua_CFunction

`typedef int (*lua_CFunction) (lua_State *L);`

Type for C functions.
**C 函数**的类型。

In order to communicate properly with Lua, a C function must use the following protocol, which defines the way parameters and results are passed: a C function receives its arguments from Lua in its stack in direct order (the first argument is pushed first).
为了与 Lua 正确通信，**C 函数**必须使用以下协议，该协议定义了参数和结果的传递方式：**C 函数**从其栈中以直接顺序从 Lua 接收参数（第一个参数最先推入）。

So, when the function starts, lua_gettop(L) returns the number of arguments received by the function. The first argument (if any) is at index 1 and its last argument is at index lua_gettop(L).
因此，当函数开始时，`lua_gettop(L)` 返回函数接收的参数数量。第一个参数（如果有）在索引 1 处，其最后一个参数在索引 `lua_gettop(L)` 处。

To return values to Lua, a C function just pushes them onto the stack, in direct order (the first result is pushed first), and returns in C the number of results.
要向 Lua 返回值，**C 函数**只需将它们推入栈，按直接顺序（第一个结果最先推入），并在 C 中返回结果的数量。

Any other value in the stack below the results will be properly discarded by Lua.
结果下方栈中的任何其他值都将被 Lua 正确丢弃。

Like a Lua function, a C function called by Lua can also return many results.
与 Lua 函数一样，被 Lua 调用的 **C 函数**也可以返回多个结果。

As an example, the following function receives a variable number of numeric arguments and returns their average and their sum:
例如，以下函数接收可变数量的数值参数并返回它们的平均值和总和：

```c
static int foo (lua_State *L) {
  int n = lua_gettop(L);    /* number of arguments */
  lua_Number sum = 0.0;
  int i;
  for (i = 1; i <= n; i++) {
    if (!lua_isnumber(L, i)) {
      lua_pushliteral(L, "incorrect argument");
      lua_error(L);
    }
    sum += lua_tonumber(L, i);
  }
  lua_pushnumber(L, sum/n);        /* first result */
  lua_pushnumber(L, sum);         /* second result */
  return 2;                   /* number of results */
}
```

### lua_checkstack

`int lua_checkstack (lua_State *L, int n);`

[-0, +0, -]

Ensures that the stack has space for at least n extra elements, that is, that you can safely push up to n values into it.
确保栈至少有 `n` 个额外元素的空间，也就是说，你可以安全地向其中推送最多 `n` 个值。

It returns false if it cannot fulfill the request, either because it would cause the stack to be greater than a fixed maximum size (typically at least several thousand elements) or because it cannot allocate memory for the extra space.
如果无法满足请求，则返回 false，要么是因为它将导致栈超过固定最大大小（通常至少几千个元素），要么是因为无法为额外空间分配内存。

This function never shrinks the stack; if the stack already has space for the extra elements, it is left unchanged.
此函数从不缩小栈；如果栈已经有额外元素的空间，则保持不变。

- **实现**（implementation）：`lapi.c:lua_checkstack` 检查 `L->stack_last.p - L->top.p > n`，不足时调用 `luaD_growstack` 扩展栈，并调整当前 `CallInfo` 的栈顶。

### lua_close

`void lua_close (lua_State *L);`

[-0, +0, -]

Close all active to-be-closed variables in the main thread, release all objects in the given Lua state (calling the corresponding garbage-collection metamethods, if any), and frees all dynamic memory used by this state.
关闭主线程中所有活动的待关闭变量，释放给定 Lua 状态中的所有对象（如果有，调用相应的垃圾回收元方法），并释放此状态使用的所有动态内存。

On several platforms, you may not need to call this function, because all resources are naturally released when the host program ends.
在多个平台上，你可能不需要调用此函数，因为当宿主程序结束时，所有资源都会自然释放。

On the other hand, long-running programs that create multiple states, such as daemons or web servers, will probably need to close states as soon as they are not needed.
另一方面，创建多个状态的长时间运行程序（如守护进程或 Web 服务器）可能需要在不需要时立即关闭状态。

- **实现**（implementation）：`lstate.c:lua_close` 切换到主线程后调用 `close_state`，释放所有对象、关闭待关闭变量，并最终调用分配器释放 `LG` 结构所占内存。

### lua_closeslot

`void lua_closeslot (lua_State *L, int index);`

[-0, +0, e]

Close the to-be-closed slot at the given index and set its value to nil. The index must be the last index previously marked to be closed (see lua_toclose) that is still active (that is, not closed yet).
关闭给定索引处的待关闭槽并将其值设置为 `nil`。索引必须是先前标记为待关闭的最后一个索引（见 `lua_toclose`），且仍然处于活动状态（即尚未关闭）。

A __close metamethod cannot yield when called through this function.
通过此函数调用时，`__close` 元方法不能让出。

(This function was introduced in release 5.4.3.)
（此函数在 5.4.3 版本中引入。）

### lua_closethread

`int lua_closethread (lua_State *L, lua_State *from);`

[-0, +?, -]

Resets a thread, cleaning its call stack and closing all pending to-be-closed variables. Returns a status code: LUA_OK for no errors in the thread (either the original error that stopped the thread or errors in closing methods), or an error status otherwise. In case of error, leaves the error object on the top of the stack.
重置线程，清理其调用栈并关闭所有待处理的待关闭变量。返回状态码：`LUA_OK` 表示线程中没有错误（无论是停止线程的原始错误还是关闭方法中的错误），否则返回错误状态。在出错的情况下，将错误对象留在栈顶。

The parameter from represents the coroutine that is resetting L. If there is no such coroutine, this parameter can be NULL.
参数 `from` 表示正在重置 `L` 的协程。如果没有这样的协程，此参数可以为 `NULL`。

(This function was introduced in release 5.4.6.)
（此函数在 5.4.6 版本中引入。）

### lua_compare

`int lua_compare (lua_State *L, int index1, int index2, int op);`

[-0, +0, e]

Compares two Lua values. Returns 1 if the value at index index1 satisfies op when compared with the value at index index2, following the semantics of the corresponding Lua operator (that is, it may call metamethods). Otherwise returns 0. Also returns 0 if any of the indices is not valid.
比较两个 Lua 值。如果索引 `index1` 处的值与索引 `index2` 处的值相比满足 `op`，则返回 1，遵循相应 Lua 运算符的语义（也就是说，它可能会调用元方法）。否则返回 0。如果任何索引无效，也返回 0。

The value of op must be one of the following constants:
`op` 的值必须是以下常量之一：

- **LUA_OPEQ**：比较是否相等（`==`）
- **LUA_OPLT**：比较是否小于（`<`）
- **LUA_OPLE**：比较是否小于或等于（`<=`）

### lua_concat

`void lua_concat (lua_State *L, int n);`

[-n, +1, e]

Concatenates the n values at the top of the stack, pops them, and leaves the result on the top. If n is 1, the result is the single value on the stack (that is, the function does nothing); if n is 0, the result is the empty string.
连接栈顶的 `n` 个值，弹出它们，并将结果留在栈顶。如果 `n` 为 1，则结果是栈中的单个值（也就是说，函数什么都不做）；如果 `n` 为 0，则结果为空字符串。

Concatenation is performed following the usual semantics of Lua (see §3.4.6).
连接遵循 Lua 的通常语义执行（见 §3.4.6）。

- **实现**（implementation）：`lapi.c:lua_concat` 调用 `luaV_concat` 执行实际连接（可能触发 `__concat` 元方法）；`n==0` 时压入空字符串；最后触发 `luaC_checkGC` 检查是否需要垃圾回收。

### lua_copy

`void lua_copy (lua_State *L, int fromidx, int toidx);`

[-0, +0, -]

Copies the element at index fromidx into the valid index toidx, replacing the value at that position. Values at other positions are not affected.
将索引 `fromidx` 处的元素复制到有效索引 `toidx`，替换该位置的值。其他位置的值不受影响。

### lua_createtable

`void lua_createtable (lua_State *L, int narr, int nrec);`

[-0, +1, m]

Creates a new empty table and pushes it onto the stack. Parameter narr is a hint for how many elements the table will have as a sequence; parameter nrec is a hint for how many other elements the table will have.
创建一个新的空表并将其推入栈。参数 `narr` 是表将作为序列拥有的元素数量的提示；参数 `nrec` 是表将拥有的其他元素数量的提示。

Lua may use these hints to preallocate memory for the new table. This preallocation may help performance when you know in advance how many elements the table will have. Otherwise you can use the function lua_newtable.
Lua 可以使用这些提示为新表预分配内存。当你预先知道表将有多少元素时，这种预分配可能有助于性能。否则你可以使用函数 `lua_newtable`。

### lua_dump

`int lua_dump (lua_State *L, lua_Writer writer, void *data, int strip);`

[-0, +0, -]

Dumps a function as a binary chunk. Receives a Lua function on the top of the stack and produces a binary chunk that, if loaded again, results in a function equivalent to the one dumped. As it produces parts of the chunk, lua_dump calls function writer with the given data to write them.
将函数转储为二进制块。接收栈顶的 Lua 函数并生成一个二进制块，如果再次加载，会产生与转储的函数等效的函数。在生成块的各个部分时，`lua_dump` 使用给定的 `data` 调用 `writer` 函数来写入它们。

If strip is true, the binary representation may not include all debug information about the function, to save space.
如果 `strip` 为 true，二进制表示可能不包含有关函数的所有调试信息，以节省空间。

The value returned is the error code returned by the last call to the writer; 0 means no errors.
返回的值是最后一次调用 writer 返回的错误码；0 表示没有错误。

This function does not pop the Lua function from the stack.
此函数不会从栈中弹出 Lua 函数。

### lua_error

`int lua_error (lua_State *L);`

[-1, +0, v]

Raises a Lua error, using the value on the top of the stack as the error object. This function does a long jump, and therefore never returns (see luaL_error).
引发 Lua 错误，使用栈顶的值作为错误对象。此函数执行长跳转，因此永远不会返回（见 `luaL_error`）。

- **实现**（implementation）：`lapi.c:lua_error` 检查错误对象是否为预分配的内存错误消息，是则调用 `luaM_error`，否则调用 `luaG_errormsg` 通过 `luaD_throw` 执行 `longjmp`。

### lua_gc

`int lua_gc (lua_State *L, int what, ...);`

[-0, +0, -]

Controls the garbage collector.
控制垃圾回收器。

This function performs several tasks, according to the value of the parameter what.
此函数根据参数 `what` 的值执行多项任务。

- **LUA_GCCOLLECT**：执行完整的垃圾回收周期。
- **LUA_GCSTOP**：停止垃圾回收器。
- **LUA_GCRESTART**：重新启动垃圾回收器。
- **LUA_GCCOUNT**：返回 Lua 当前使用的内存量（以 Kbytes 为单位）。
- **LUA_GCCOUNTB**：返回 Lua 当前使用的内存字节数除以 1024 的余数。
- **LUA_GCSTEP (int stepsize)**：执行垃圾回收的增量步骤，对应于分配 `stepsize` Kbytes。
- **LUA_GCISRUNNING**：返回一个布尔值，指示回收器是否正在运行（即未停止）。
- **LUA_GCINC (int pause, int stepmul, stepsize)**：将回收器更改为增量模式，使用给定参数（见 §2.5.1）。返回先前的模式（`LUA_GCGEN` 或 `LUA_GCINC`）。
- **LUA_GCGEN (int minormul, int majormul)**：将回收器更改为分代模式，使用给定参数（见 §2.5.2）。返回先前的模式（`LUA_GCGEN` 或 `LUA_GCINC`）。

For more details about these options, see collectgarbage.
有关这些选项的更多详细信息，请参见 `collectgarbage`。

This function should not be called by a finalizer.
终结器不应调用此函数。

- **实现**（implementation）：`lapi.c:lua_gc` 是一个大的 `switch`，根据 `what` 调用 `luaC_fullgc`、`luaC_step`、`luaC_changemode` 等；内存统计直接读取 `global_State` 中的 `totalbytes` 和 `GCdebt`。

### lua_getallocf

`lua_Alloc lua_getallocf (lua_State *L, void **ud);`

[-0, +0, -]

Returns the memory-allocation function of a given state. If ud is not NULL, Lua stores in *ud the opaque pointer given when the memory-allocator function was set.
返回给定状态的**内存分配函数**。如果 `ud` 不为 `NULL`，Lua 在 `*ud` 中存储设置内存分配器函数时给定的不透明指针。

### lua_getfield

`int lua_getfield (lua_State *L, int index, const char *k);`

[-0, +1, e]

Pushes onto the stack the value t[k], where t is the value at the given index. As in Lua, this function may trigger a metamethod for the "index" event (see §2.4). Returns the type of the pushed value.
将值 `t[k]` 推入栈，其中 `t` 是给定索引处的值。与 Lua 中一样，此函数可能会触发 "index" 事件的元方法（见 §2.4）。返回推送值的类型。

- **实现**（implementation）：`lapi.c:lua_getfield` 将字符串键内部化为 `TString` 后，走与 `lua_gettable` 类似的 `luaV_fastget` / `luaV_finishget` 路径访问表字段。

### lua_getextraspace

`void *lua_getextraspace (lua_State *L);`

[-0, +0, -]

Returns a pointer to a raw memory area associated with the given Lua state. The application can use this area for any purpose; Lua does not use it for anything.
返回指向与给定 Lua 状态关联的原始内存区域的指针。应用程序可以将此区域用于任何目的；Lua 不将其用于任何用途。

Each new thread has this area initialized with a copy of the area of the main thread.
每个新线程都使用主线程区域的副本初始化此区域。

By default, this area has the size of a pointer to void, but you can recompile Lua with a different size for this area. (See LUA_EXTRASPACE in luaconf.h.)
默认情况下，此区域的大小为指向 `void` 的指针的大小，但你可以使用不同的大小重新编译 Lua。（请参阅 `luaconf.h` 中的 `LUA_EXTRASPACE`。）

### lua_getglobal

`int lua_getglobal (lua_State *L, const char *name);`

[-0, +1, e]

Pushes onto the stack the value of the global name. Returns the type of that value.
将全局变量 `name` 的值推入栈。返回该值的类型。

- **实现**（implementation）：`lapi.c:lua_getglobal` 获取全局表 `_G` 后，调用 `auxgetstr` 以 `TString` 为键查表，走 `luaV_fastget` / `luaV_finishget` 路径。

### lua_geti

`int lua_geti (lua_State *L, int index, lua_Integer i);`

[-0, +1, e]

Pushes onto the stack the value t[i], where t is the value at the given index. As in Lua, this function may trigger a metamethod for the "index" event (see §2.4). Returns the type of the pushed value.
将值 `t[i]` 推入栈，其中 `t` 是给定索引处的值。与 Lua 中一样，此函数可能会触发 "index" 事件的元方法（见 §2.4）。返回推送值的类型。

### lua_getmetatable

`int lua_getmetatable (lua_State *L, int index);`

[-0, +0|1, -]

If the value at the given index has a metatable, the function pushes that metatable onto the stack and returns 1. Otherwise, the function returns 0 and pushes nothing on the stack.
如果给定索引处的值具有元表，则函数将该元表推入栈并返回 1。否则，函数返回 0 且不在栈上推送任何内容。

- **实现**（implementation）：`lapi.c:lua_getmetatable` 对表和 userdata 直接取对象内部的 `metatable` 指针；对其他类型取 `G(L)->mt[ttype(obj)]`（全局按类型元表）。

### lua_gettable

`int lua_gettable (lua_State *L, int index);`

[-1, +1, e]

Pushes onto the stack the value t[k], where t is the value at the given index and k is the value on the top of the stack.
将值 `t[k]` 推入栈，其中 `t` 是给定索引处的值，`k` 是栈顶的值。

This function pops the key from the stack, pushing the resulting value in its place. As in Lua, this function may trigger a metamethod for the "index" event (see §2.4). Returns the type of the pushed value.
此函数从栈中弹出键，将结果值推入其位置。与 Lua 中一样，此函数可能会触发 "index" 事件的元方法（见 §2.4）。返回推送值的类型。

- **实现**（implementation）：`lapi.c:lua_gettable` 先尝试 `luaV_fastget` 快速路径（直接查表），失败时走 `luaV_finishget` 慢路径，可能调用 `__index` 元方法，结果替换栈顶的键。

### lua_gettop

`int lua_gettop (lua_State *L);`

[-0, +0, -]

Returns the index of the top element in the stack. Because indices start at 1, this result is equal to the number of elements in the stack; in particular, 0 means an empty stack.
返回栈中顶部元素的索引。因为索引从 1 开始，此结果等于栈中的元素数量；特别地，0 表示空栈。

- **实现**（implementation）：`lapi.c:lua_gettop` 直接计算 `L->top.p - (L->ci->func.p + 1)`，即栈顶与当前函数栈底之间的偏移。

### lua_getiuservalue

`int lua_getiuservalue (lua_State *L, int index, int n);`

[-0, +1, -]

Pushes onto the stack the n-th user value associated with the full userdata at the given index and returns the type of the pushed value. If the userdata does not have that value, pushes nil and returns LUA_TNONE.
将给定索引处完整 userdata 的第 `n` 个用户值推入栈，并返回推送值的类型。如果 userdata 没有该值，则推入 `nil` 并返回 `LUA_TNONE`。

- **实现**（implementation）：`lapi.c:lua_getiuservalue` 检查 `n` 是否在 `[1, nuvalue]` 范围内，命中则从 `uvalue(o)->uv[n-1].uv` 复制 `TValue` 到栈顶，否则压入 `nil`。

### lua_insert

`void lua_insert (lua_State *L, int index);`

[-1, +1, -]

Moves the top element into the given valid index, shifting up the elements above this index to open space. This function cannot be called with a pseudo-index, because a pseudo-index is not an actual stack position.
将顶部元素移动到给定的有效索引处，将此索引上方的元素向上移动以打开空间。此函数不能使用伪索引调用，因为伪索引不是实际的栈位置。

### lua_Integer

`typedef ... lua_Integer;`

The type of integers in Lua.
Lua 中整数的类型。

By default this type is long long, (usually a 64-bit two-complement integer), but that can be changed to long or int (usually a 32-bit two-complement integer). (See LUA_INT_TYPE in luaconf.h.)
默认情况下，此类型为 `long long`（通常是 64 位二进制补码整数），但可以更改为 `long` 或 `int`（通常是 32 位二进制补码整数）。（请参阅 `luaconf.h` 中的 `LUA_INT_TYPE`。）

Lua also defines the constants LUA_MININTEGER and LUA_MAXINTEGER, with the minimum and the maximum values that fit in this type.
Lua 还定义了常量 `LUA_MININTEGER` 和 `LUA_MAXINTEGER`，分别是适合此类型的最小值和最大值。

### lua_isboolean

`int lua_isboolean (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a boolean, and 0 otherwise.
如果给定索引处的值是布尔值，则返回 1，否则返回 0。

### lua_iscfunction

`int lua_iscfunction (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a C function, and 0 otherwise.
如果给定索引处的值是 **C 函数**，则返回 1，否则返回 0。

### lua_isfunction

`int lua_isfunction (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a function (either C or Lua), and 0 otherwise.
如果给定索引处的值是函数（**C** 或 Lua），则返回 1，否则返回 0。

### lua_isinteger

`int lua_isinteger (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is an integer (that is, the value is a number and is represented as an integer), and 0 otherwise.
如果给定索引处的值是整数（也就是说，该值是数字并表示为整数），则返回 1，否则返回 0。

- **实现**（implementation）：`lapi.c:lua_isinteger` 直接检查 `TValue` 的类型标签是否为 `LUA_VNUMINT`，不做任何转换。

### lua_islightuserdata

`int lua_islightuserdata (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a light userdata, and 0 otherwise.
如果给定索引处的值是轻量 userdata，则返回 1，否则返回 0。

### lua_isnil

`int lua_isnil (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is nil, and 0 otherwise.
如果给定索引处的值是 `nil`，则返回 1，否则返回 0。

### lua_isnone

`int lua_isnone (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the given index is not valid, and 0 otherwise.
如果给定索引无效，则返回 1，否则返回 0。

### lua_isnoneornil

`int lua_isnoneornil (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the given index is not valid or if the value at this index is nil, and 0 otherwise.
如果给定索引无效或此索引处的值是 `nil`，则返回 1，否则返回 0。

### lua_isnumber

`int lua_isnumber (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a number or a string convertible to a number, and 0 otherwise.
如果给定索引处的值是数字或可转换为数字的字符串，则返回 1，否则返回 0。

- **实现**（implementation）：`lapi.c:lua_isnumber` 通过 `tonumber` 宏尝试转换：整数和浮点直接成功，字符串调用 `luaO_str2num` 词法解析，转换成功则返回 1。

### lua_isstring

`int lua_isstring (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a string or a number (which is always convertible to a string), and 0 otherwise.
如果给定索引处的值是字符串或数字（始终可转换为字符串），则返回 1，否则返回 0。

- **实现**（implementation）：`lapi.c:lua_isstring` 直接检查类型标签是否为 `LUA_VSHRSTR` / `LUA_VLNGSTR`，或对数字类型（`cvt2str`）也返回 1，因为数字总可转字符串。

### lua_istable

`int lua_istable (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a table, and 0 otherwise.
如果给定索引处的值是表，则返回 1，否则返回 0。

### lua_isthread

`int lua_isthread (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a thread, and 0 otherwise.
如果给定索引处的值是线程，则返回 1，否则返回 0。

### lua_isuserdata

`int lua_isuserdata (lua_State *L, int index);`

[-0, +0, -]

Returns 1 if the value at the given index is a userdata (either full or light), and 0 otherwise.
如果给定索引处的值是 userdata（完整或轻量），则返回 1，否则返回 0。

### lua_isyieldable

`int lua_isyieldable (lua_State *L);`

[-0, +0, -]

Returns 1 if the given coroutine can yield, and 0 otherwise.
如果给定协程可以让出，则返回 1，否则返回 0。

### lua_KContext

`typedef ... lua_KContext;`

The type for continuation-function contexts. It must be a numeric type. This type is defined as intptr_t when intptr_t is available, so that it can store pointers too. Otherwise, it is defined as ptrdiff_t.
延续函数上下文的类型。它必须是数值类型。当 `intptr_t` 可用时，此类型定义为 `intptr_t`，以便它也可以存储指针。否则，它定义为 `ptrdiff_t`。

### lua_KFunction

`typedef int (*lua_KFunction) (lua_State *L, int status, lua_KContext ctx);`

Type for continuation functions (see §4.5).
延续函数的类型（见 §4.5）。

### lua_len

`void lua_len (lua_State *L, int index);`

[-0, +1, e]

Returns the length of the value at the given index. It is equivalent to the # operator in Lua (see §3.4.7) and may trigger a metamethod for the "length" event (see §2.4). The result is pushed on the stack.
返回给定索引处值的长度。它等价于 Lua 中的 `#` 运算符（见 §3.4.7），并可能触发 "length" 事件的元方法（见 §2.4）。结果被推入栈中。

### lua_load

`int lua_load (lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode);`

[-0, +1, -]

Loads a Lua chunk without running it. If there are no errors, lua_load pushes the compiled chunk as a Lua function on top of the stack. Otherwise, it pushes an error message.
加载 Lua 代码块而不运行它。如果没有错误，`lua_load` 将编译后的代码块作为 Lua 函数推入栈顶。否则，它推入错误消息。

The lua_load function uses a user-supplied reader function to read the chunk (see lua_Reader). The data argument is an opaque value passed to the reader function.
`lua_load` 函数使用用户提供的读取器函数来读取代码块（见 `lua_Reader`）。`data` 参数是传递给读取器函数的不透明值。

The chunkname argument gives a name to the chunk, which is used for error messages and in debug information (see §4.7).
`chunkname` 参数为代码块命名，用于错误消息和调试信息（见 §4.7）。

lua_load automatically detects whether the chunk is text or binary and loads it accordingly (see program luac). The string mode works as in function load, with the addition that a NULL value is equivalent to the string "bt".
`lua_load` 自动检测代码块是文本还是二进制并相应地加载它（见程序 `luac`）。字符串 `mode` 的工作方式与 `load` 函数相同，但附加条件是 `NULL` 值等同于字符串 `"bt"`。

lua_load uses the stack internally, so the reader function must always leave the stack unmodified when returning.
`lua_load` 在内部使用栈，因此读取器函数在返回时必须始终不修改栈。

lua_load can return LUA_OK, LUA_ERRSYNTAX, or LUA_ERRMEM. The function may also return other values corresponding to errors raised by the read function (see §4.4.1).
`lua_load` 可以返回 `LUA_OK`、`LUA_ERRSYNTAX` 或 `LUA_ERRMEM`。该函数还可能返回与读取函数引发的错误相对应的其他值（见 §4.4.1）。

If the resulting function has upvalues, its first upvalue is set to the value of the global environment stored at index LUA_RIDX_GLOBALS in the registry (see §4.3). When loading main chunks, this upvalue will be the _ENV variable (see §2.2). Other upvalues are initialized with nil.
如果结果函数有上值，则其第一个上值设置为注册表中索引 `LUA_RIDX_GLOBALS` 处存储的全局环境的值（见 §4.3）。加载主代码块时，此上值将是 `_ENV` 变量（见 §2.2）。其他上值用 `nil` 初始化。

### lua_newstate

`lua_State *lua_newstate (lua_Alloc f, void *ud);`

[-0, +0, -]

Creates a new independent state and returns its main thread. Returns NULL if it cannot create the state (due to lack of memory). The argument f is the allocator function; Lua will do all memory allocation for this state through this function (see lua_Alloc). The second argument, ud, is an opaque pointer that Lua passes to the allocator in every call.
创建一个新的独立状态并返回其主线程。如果无法创建状态（由于内存不足），则返回 `NULL`。参数 `f` 是分配器函数；Lua 将通过此函数完成此状态的所有内存分配（见 `lua_Alloc`）。第二个参数 `ud` 是一个不透明指针，Lua 在每次调用时将其传递给分配器。

- **实现**（implementation）：`lstate.c:lua_newstate` 使用用户提供的分配器分配 `LG` 结构（包含 `lua_State` 和 `global_State`），初始化 GC、字符串表、注册表等，然后在保护模式下调用 `f_luaopen` 完成初始化。

### lua_newtable

`void lua_newtable (lua_State *L);`

[-0, +1, m]

Creates a new empty table and pushes it onto the stack. It is equivalent to lua_createtable(L, 0, 0).
创建一个新的空表并将其推入栈。它等价于 `lua_createtable(L, 0, 0)`。

### lua_newthread

`lua_State *lua_newthread (lua_State *L);`

[-0, +1, m]

Creates a new thread, pushes it on the stack, and returns a pointer to a lua_State that represents this new thread. The new thread returned by this function shares with the original thread its global environment, but has an independent execution stack.
创建一个新线程，将其推入栈，并返回指向表示此新线程的 `lua_State` 的指针。此函数返回的新线程与原始线程共享其全局环境，但具有独立的执行栈。

Threads are subject to garbage collection, like any Lua object.
线程像任何 Lua 对象一样受垃圾回收影响。

### lua_newuserdatauv

`void *lua_newuserdatauv (lua_State *L, size_t size, int nuvalue);`

[-0, +1, m]

This function creates and pushes on the stack a new full userdata, with nuvalue associated Lua values, called user values, plus an associated block of raw memory with size bytes. (The user values can be set and read with the functions lua_setiuservalue and lua_getiuservalue.)
此函数创建并将一个新的完整 userdata 推入栈，具有 `nuvalue` 个关联的 Lua 值，称为**用户值**（user values），加上一个大小为 `size` 字节的关联原始内存块。（用户值可以使用函数 `lua_setiuservalue` 和 `lua_getiuservalue` 设置和读取。）

The function returns the address of the block of memory. Lua ensures that this address is valid as long as the corresponding userdata is alive (see §2.5). Moreover, if the userdata is marked for finalization (see §2.5.3), its address is valid at least until the call to its finalizer.
该函数返回内存块的地址。Lua 确保只要相应的 userdata 处于活动状态，此地址就有效（见 §2.5）。此外，如果 userdata 被标记为终结（见 §2.5.3），则其地址至少有效直到调用其终结器。

- **实现**（implementation）：`lapi.c:lua_newuserdatauv` 调用 `luaS_newudata` 分配 `Udata` 对象及关联的 `nuvalue` 个 `TValue` 空间，压栈后返回 `getudatamem(u)` 指向的原始内存块地址。

### lua_next

`int lua_next (lua_State *L, int index);`

[-1, +2|0, v]

Pops a key from the stack, and pushes a key-value pair from the table at the given index, the "next" pair after the given key. If there are no more elements in the table, then lua_next returns 0 and pushes nothing.
从栈中弹出一个键，并从给定索引处的表中推入一个键值对，即给定键之后的 "下一个" 对。如果表中没有更多元素，则 `lua_next` 返回 0 且不推送任何内容。

A typical table traversal looks like this:
典型的表遍历如下所示：

```c
/* table is in the stack at index 't' */
lua_pushnil(L);  /* first key */
while (lua_next(L, t) != 0) {
  /* uses 'key' (at index -2) and 'value' (at index -1) */
  printf("%s - %s\n",
         lua_typename(L, lua_type(L, -2)),
         lua_typename(L, lua_type(L, -1)));
  /* removes 'value'; keeps 'key' for next iteration */
  lua_pop(L, 1);
}
```

While traversing a table, avoid calling lua_tolstring directly on a key, unless you know that the key is actually a string. Recall that lua_tolstring may change the value at the given index; this confuses the next call to lua_next.
遍历表时，避免直接在键上调用 `lua_tolstring`，除非你知道该键实际上是字符串。请注意，`lua_tolstring` 可能会更改给定索引处的值；这会混淆对 `lua_next` 的下一次调用。

This function may raise an error if the given key is neither nil nor present in the table. See function next for the caveats of modifying the table during its traversal.
如果给定键既不是 `nil` 也不在表中，则此函数可能会引发错误。有关在遍历期间修改表的注意事项，请参见 `next` 函数。

- **实现**（implementation）：`lapi.c:lua_next` 调用 `luaH_next` 遍历表的数组部分和哈希部分；成功时压入键值对并返回 1，失败时弹出键并返回 0。

### lua_Number

`typedef ... lua_Number;`

The type of floats in Lua.
Lua 中浮点数的类型。

By default this type is double, but that can be changed to a single float or a long double. (See LUA_FLOAT_TYPE in luaconf.h.)
默认情况下，此类型为 `double`，但可以更改为单精度浮点数或 `long double`。（请参阅 `luaconf.h` 中的 `LUA_FLOAT_TYPE`。）

### lua_numbertointeger

`int lua_numbertointeger (lua_Number n, lua_Integer *p);`

Tries to convert a Lua float to a Lua integer; the float n must have an integral value. If that value is within the range of Lua integers, it is converted to an integer and assigned to *p. The macro results in a boolean indicating whether the conversion was successful. (Note that this range test can be tricky to do correctly without this macro, due to rounding.)
尝试将 Lua 浮点数转换为 Lua 整数；浮点数 `n` 必须具有整数值。如果该值在 Lua 整数范围内，则将其转换为整数并赋给 `*p`。宏结果是表示转换是否成功的布尔值。（请注意，由于舍入，如果没有此宏，此范围测试可能很难正确执行。）

This macro may evaluate its arguments more than once.
此宏可能会多次计算其参数。

### lua_pcall

`int lua_pcall (lua_State *L, int nargs, int nresults, int msgh);`

[-(nargs + 1), +(nresults|1), -]

Calls a function (or a callable object) in protected mode.
在受保护模式下调用函数（或可调用对象）。

Both nargs and nresults have the same meaning as in lua_call. If there are no errors during the call, lua_pcall behaves exactly like lua_call. However, if there is any error, lua_pcall catches it, pushes a single value on the stack (the error object), and returns an error code. Like lua_call, lua_pcall always removes the function and its arguments from the stack.
`nargs` 和 `nresults` 的含义与 `lua_call` 中相同。如果调用期间没有错误，`lua_pcall` 的行为与 `lua_call` 完全相同。但是，如果有任何错误，`lua_pcall` 会捕获它，在栈上推送单个值（错误对象），并返回错误码。与 `lua_call` 一样，`lua_pcall` 始终从栈中移除函数及其参数。

If msgh is 0, then the error object returned on the stack is exactly the original error object.
如果 `msgh` 为 0，则栈上返回的错误对象正是原始错误对象。

Otherwise, msgh is the stack index of a message handler. (This index cannot be a pseudo-index.) In case of runtime errors, this handler will be called with the error object and its return value will be the object returned on the stack by lua_pcall.
否则，`msgh` 是**消息处理程序**的栈索引。（此索引不能是伪索引。）在运行时错误的情况下，将使用错误对象调用此处理程序，其返回值将是 `lua_pcall` 在栈上返回的对象。

Typically, the message handler is used to add more debug information to the error object, such as a stack traceback. Such information cannot be gathered after the return of lua_pcall, since by then the stack has unwound.
通常，消息处理程序用于向错误对象添加更多调试信息，例如栈回溯。此类信息无法在 `lua_pcall` 返回后收集，因为到那时栈已经展开。

The lua_pcall function returns one of the following status codes: LUA_OK, LUA_ERRRUN, LUA_ERRMEM, or LUA_ERRERR.
`lua_pcall` 函数返回以下状态码之一：`LUA_OK`、`LUA_ERRRUN`、`LUA_ERRMEM` 或 `LUA_ERRERR`。

- **实现**（implementation）：`lapi.c:lua_pcall` 本质是 `lua_pcallk` 且 `k==NULL` 的包装；在无延续时通过 `luaD_pcall` 建立 `setjmp` 恢复点，调用 `f_call` 执行 `luaD_callnoyield`。

### lua_pcallk

`int lua_pcallk (lua_State *L, int nargs, int nresults, int msgh, lua_KContext ctx, lua_KFunction k);`

[-(nargs + 1), +(nresults|1), -]

This function behaves exactly like lua_pcall, except that it allows the called function to yield (see §4.5).
此函数的行为与 `lua_pcall` 完全相同，不同之处在于它允许被调用函数让出（见 §4.5）。

- **实现**（implementation）：`lapi.c:lua_pcallk` 在提供延续且线程可让出时，将 `k`、`ctx` 和错误处理函数索引保存到 `CallInfo`，设置 `CIST_YPCALL` 标志后调用 `luaD_call`；否则回退到传统的 `luaD_pcall` 保护调用。

### lua_pop

`void lua_pop (lua_State *L, int n);`

[-n, +0, e]

Pops n elements from the stack. It is implemented as a macro over lua_settop.
从栈中弹出 `n` 个元素。它实现为 `lua_settop` 上的宏。

- **实现**（implementation）：`lua_pop` 是宏，定义为 `lua_settop(L, -(n)-1)`，直接调整栈顶指针实现弹出。

### lua_pushboolean

`void lua_pushboolean (lua_State *L, int b);`

[-0, +1, -]

Pushes a boolean value with value b onto the stack.
将值为 `b` 的布尔值推入栈。

- **实现**（implementation）：`lapi.c:lua_pushboolean` 根据 `b` 的真假，在栈顶设置 `LUA_VTRUE` 或 `LUA_VFALSE` 类型标签，然后递增栈顶。

### lua_pushcclosure

`void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n);`

[-n, +1, m]

Pushes a new C closure onto the stack. This function receives a pointer to a C function and pushes onto the stack a Lua value of type function that, when called, invokes the corresponding C function. The parameter n tells how many upvalues this function will have (see §4.2).
将新的 **C 闭包**推入栈。此函数接收指向 **C 函数**的指针，并将类型为 `function` 的 Lua 值推入栈，调用时将调用相应的 **C 函数**。参数 `n` 指示此函数将有多少个上值（见 §4.2）。

Any function to be callable by Lua must follow the correct protocol to receive its parameters and return its results (see lua_CFunction).
任何可被 Lua 调用的函数都必须遵循正确的协议来接收其参数并返回其结果（见 `lua_CFunction`）。

When a C function is created, it is possible to associate some values with it, the so called upvalues; these upvalues are then accessible to the function whenever it is called. This association is called a C closure (see §4.2). To create a C closure, first the initial values for its upvalues must be pushed onto the stack. (When there are multiple upvalues, the first value is pushed first.) Then lua_pushcclosure is called to create and push the C function onto the stack, with the argument n telling how many values will be associated with the function. lua_pushcclosure also pops these values from the stack.
创建 **C 函数**时，可以将某些值与其关联，即所谓的**上值**；函数在每次被调用时都可以访问这些上值。这种关联称为 **C 闭包**（见 §4.2）。要创建 **C 闭包**，首先必须将其上值的初始值推入栈。（当有多个上值时，第一个值最先推入。）然后调用 `lua_pushcclosure` 来创建并将 **C 函数**推入栈，参数 `n` 指示将与函数关联多少个值。`lua_pushcclosure` 还会从栈中弹出这些值。

The maximum value for n is 255.
`n` 的最大值为 255。

When n is zero, this function creates a light C function, which is just a pointer to the C function. In that case, it never raises a memory error.
当 `n` 为零时，此函数创建一个**轻量 C 函数**（light C function），它只是指向 **C 函数**的指针。在这种情况下，它永远不会引发内存错误。

- **实现**（implementation）：`lapi.c:lua_pushcclosure` 在 `n==0` 时直接压入轻量函数指针；否则调用 `luaF_newCclosure` 创建 `CClosure` 对象，从栈中弹出 `n` 个上值并复制到闭包的 `upvalue` 数组中。

### lua_pushcfunction

`void lua_pushcfunction (lua_State *L, lua_CFunction f);`

[-0, +1, -]

Pushes a C function onto the stack. This function is equivalent to lua_pushcclosure with no upvalues.
将 **C 函数**推入栈。此函数等价于没有上值的 `lua_pushcclosure`。

### lua_pushfstring

`const char *lua_pushfstring (lua_State *L, const char *fmt, ...);`

[-0, +1, v]

Pushes onto the stack a formatted string and returns a pointer to this string (see §4.1.3). It is similar to the sprintf, but has two important differences.
将格式化字符串推入栈并返回指向此字符串的指针（见 §4.1.3）。它类似于 `sprintf`，但有两个重要区别。

First, you do not have to allocate space for the result; the result is a Lua string and Lua takes care of memory allocation (and deallocation, through garbage collection).
首先，你不必为结果分配空间；结果是 Lua 字符串，Lua 负责内存分配（以及通过垃圾回收进行释放）。

Second, the conversion specifiers are quite restricted. There are no flags, widths, or precisions. The conversion specifiers can only be %% (inserts the character %), %s (inserts a zero-terminated string, with no size restrictions), %f (inserts a lua_Number), %I (inserts a lua_Integer), %p (inserts a pointer), %d (inserts an int), %c (inserts an int as a one-byte character), and %U (inserts a long int as a UTF-8 byte sequence).
其次，转换说明符非常受限。没有标志、宽度或精度。转换说明符只能是 `%%`（插入字符 `%`）、`%s`（插入以零结尾的字符串，没有大小限制）、`%f`（插入 `lua_Number`）、`%I`（插入 `lua_Integer`）、`%p`（插入指针）、`%d`（插入 `int`）、`%c`（插入 `int` 作为单字节字符）和 `%U`（插入 `long int` 作为 UTF-8 字节序列）。

This function may raise errors due to memory overflow or an invalid conversion specifier.
此函数可能由于内存溢出或无效的转换说明符而引发错误。

### lua_pushglobaltable

`void lua_pushglobaltable (lua_State *L);`

[-0, +1, -]

Pushes the global environment onto the stack.
将全局环境推入栈。

### lua_pushinteger

`void lua_pushinteger (lua_State *L, lua_Integer n);`

[-0, +1, -]

Pushes an integer with value n onto the stack.
将值为 `n` 的整数推入栈。

- **实现**（implementation）：`lapi.c:lua_pushinteger` 通过 `setivalue` 将 `lua_Integer` 写入栈顶的 `TValue`，设置类型标签为 `LUA_VNUMINT`，然后递增栈顶。

### lua_pushlightuserdata

`void lua_pushlightuserdata (lua_State *L, void *p);`

[-0, +1, -]

Pushes a light userdata onto the stack.
将轻量 userdata 推入栈。

Userdata represent C values in Lua. A light userdata represents a pointer, a void*. It is a value (like a number): you do not create it, it has no individual metatable, and it is not collected (as it was never created). A light userdata is equal to "any" light userdata with the same C address.
Userdata 在 Lua 中表示 **C 值**。**轻量 userdata** 表示指针，即 `void*`。它是一个值（像数字一样）：你不创建它，它没有单独的元表，也不会被收集（因为它从未被创建）。轻量 userdata 与具有相同 **C 地址**的任何轻量 userdata 相等。

### lua_pushliteral

`const char *lua_pushliteral (lua_State *L, const char *s);`

[-0, +1, m]

This macro is equivalent to lua_pushstring, but should be used only when s is a literal string. (Lua may optimize this case.)
此宏等价于 `lua_pushstring`，但仅在 `s` 是字面字符串时使用。（Lua 可以优化这种情况。）

### lua_pushlstring

`const char *lua_pushlstring (lua_State *L, const char *s, size_t len);`

[-0, +1, m]

Pushes the string pointed to by s with size len onto the stack. Lua will make or reuse an internal copy of the given string, so the memory at s can be freed or reused immediately after the function returns. The string can contain any binary data, including embedded zeros.
将 `s` 指向的大小为 `len` 的字符串推入栈。Lua 将创建或重用给定字符串的内部副本，因此 `s` 处的内存可以在函数返回后立即释放或重用。字符串可以包含任何二进制数据，包括嵌入的零。

Returns a pointer to the internal copy of the string (see §4.1.3).
返回指向字符串内部副本的指针（见 §4.1.3）。

### lua_pushnil

`void lua_pushnil (lua_State *L);`

[-0, +1, -]

Pushes a nil value onto the stack.
将 `nil` 值推入栈。

- **实现**（implementation）：`lapi.c:lua_pushnil` 在栈顶设置 `LUA_VNIL` 类型标签（即 `setnilvalue`），然后递增栈顶指针。

### lua_pushnumber

`void lua_pushnumber (lua_State *L, lua_Number n);`

[-0, +1, -]

Pushes a float with value n onto the stack.
将值为 `n` 的浮点数推入栈。

- **实现**（implementation）：`lapi.c:lua_pushnumber` 通过 `setfltvalue` 将 `lua_Number` 写入栈顶的 `TValue`，设置类型标签为 `LUA_VNUMFLT`，然后递增栈顶。

### lua_pushstring

`const char *lua_pushstring (lua_State *L, const char *s);`

[-0, +1, m]

Pushes the zero-terminated string pointed to by s onto the stack. Lua will make or reuse an internal copy of the given string, so the memory at s can be freed or reused immediately after the function returns.
将 `s` 指向的以零结尾的字符串推入栈。Lua 将创建或重用给定字符串的内部副本，因此 `s` 处的内存可以在函数返回后立即释放或重用。

Returns a pointer to the internal copy of the string (see §4.1.3).
返回指向字符串内部副本的指针（见 §4.1.3）。

If s is NULL, pushes nil and returns NULL.
如果 `s` 为 `NULL`，则推入 `nil` 并返回 `NULL`。

- **实现**（implementation）：`lapi.c:lua_pushstring` 调用 `luaS_new` 对字符串做内部化（intern），得到 `TString*` 后压栈；`NULL` 时压入 `nil`。返回的指针指向 Lua 内部管理的字符串数据。

### lua_pushthread

`int lua_pushthread (lua_State *L);`

[-0, +1, -]

Pushes the thread represented by L onto the stack. Returns 1 if this thread is the main thread of its state.
将 `L` 表示的线程推入栈。如果此线程是其状态的主线程，则返回 1。

### lua_pushvalue

`void lua_pushvalue (lua_State *L, int index);`

[-0, +1, -]

Pushes a copy of the element at the given index onto the stack.
将给定索引处元素的副本推入栈。

- **实现**（implementation）：`lapi.c:lua_pushvalue` 通过 `index2value` 将索引转为 `TValue*` 指针，再用 `setobj2s` 复制到栈顶并递增栈顶。

### lua_pushvfstring

`const char *lua_pushvfstring (lua_State *L, const char *fmt, va_list argp);`

[-0, +1, v]

Equivalent to lua_pushfstring, except that it receives a va_list instead of a variable number of arguments.
等价于 `lua_pushfstring`，不同之处在于它接收 `va_list` 而不是可变数量的参数。

### lua_rawequal

`int lua_rawequal (lua_State *L, int index1, int index2);`

[-0, +0, -]

Returns 1 if the two values in indices index1 and index2 are primitively equal (that is, equal without calling the __eq metamethod). Otherwise returns 0. Also returns 0 if any of the indices are not valid.
如果索引 `index1` 和 `index2` 中的两个值原始相等（也就是说，不调用 `__eq` 元方法而相等），则返回 1。否则返回 0。如果任何索引无效，也返回 0。

### lua_rawget

`int lua_rawget (lua_State *L, int index);`

[-1, +1, -]

Similar to lua_gettable, but does a raw access (i.e., without metamethods). The value at index must be a table.
类似于 `lua_gettable`，但执行原始访问（即不使用元方法）。索引处的值必须是表。

- **实现**（implementation）：`lapi.c:lua_rawget` 直接从栈顶弹出键，调用 `luaH_get` 在 `Table` 的哈希表/数组中查找，结果通过 `finishrawget` 压入栈顶。

### lua_rawgeti

`int lua_rawgeti (lua_State *L, int index, lua_Integer n);`

[-0, +1, -]

Pushes onto the stack the value t[n], where t is the table at the given index. The access is raw, that is, it does not use the __index metavalue. Returns the type of the pushed value.
将值 `t[n]` 推入栈，其中 `t` 是给定索引处的表。访问是原始的，也就是说，它不使用 `__index` 元值。返回推送值的类型。

### lua_rawgetp

`int lua_rawgetp (lua_State *L, int index, const void *p);`

[-0, +1, -]

Pushes onto the stack the value t[k], where t is the table at the given index and k is the pointer p represented as a light userdata. The access is raw; that is, it does not use the __index metavalue. Returns the type of the pushed value.
将值 `t[k]` 推入栈，其中 `t` 是给定索引处的表，`k` 是表示为轻量 userdata 的指针 `p`。访问是原始的；也就是说，它不使用 `__index` 元值。返回推送值的类型。

### lua_rawlen

`lua_Unsigned lua_rawlen (lua_State *L, int index);`

[-0, +0, -]

Returns the raw "length" of the value at the given index: for strings, this is the string length; for tables, this is the result of the length operator (#) with no metamethods; for userdata, this is the size of the block of memory allocated for the userdata. For other values, this call returns 0.
返回给定索引处值的原始"长度"：对于字符串，这是字符串长度；对于表，这是没有元方法的长度运算符（`#`）的结果；对于 userdata，这是为 userdata 分配的内存块的大小。对于其他值，此调用返回 0。

### lua_rawset

`void lua_rawset (lua_State *L, int index);`

[-2, +0, m]

Similar to lua_settable, but does a raw assignment (i.e., without metamethods). The value at index must be a table.
类似于 `lua_settable`，但执行原始赋值（即不使用元方法）。索引处的值必须是表。

- **实现**（implementation）：`lapi.c:lua_rawset` 直接调用 `luaH_set` 写入哈希表，然后执行 `invalidateTMcache` 和 `luaC_barrierback` 以保持 GC 屏障一致性，最后弹出键和值。

### lua_rawseti

`void lua_rawseti (lua_State *L, int index, lua_Integer i);`

[-1, +0, m]

Does the equivalent of t[i] = v, where t is the table at the given index and v is the value on the top of the stack. This function pops the value from the stack. The assignment is raw, that is, it does not use the __newindex metavalue.
执行 `t[i] = v` 的等效操作，其中 `t` 是给定索引处的表，`v` 是栈顶的值。此函数从栈中弹出值。赋值是原始的，也就是说，它不使用 `__newindex` 元值。

### lua_rawsetp

`void lua_rawsetp (lua_State *L, int index, const void *p);`

[-1, +0, m]

Does the equivalent of t[p] = v, where t is the table at the given index, p is encoded as a light userdata, and v is the value on the top of the stack. This function pops the value from the stack. The assignment is raw, that is, it does not use the __newindex metavalue.
执行 `t[p] = v` 的等效操作，其中 `t` 是给定索引处的表，`p` 编码为轻量 userdata，`v` 是栈顶的值。此函数从栈中弹出值。赋值是原始的，也就是说，它不使用 `__newindex` 元值。

### lua_Reader

`typedef const char * (*lua_Reader) (lua_State *L, void *data, size_t *size);`

The reader function used by lua_load. Every time lua_load needs another piece of the chunk, it calls the reader, passing along its data parameter. The reader must return a pointer to a block of memory with a new piece of the chunk and set size to the block size. The block must exist until the reader function is called again. To signal the end of the chunk, the reader must return NULL or set size to zero. The reader function may return pieces of any size greater than zero.
`lua_load` 使用的读取器函数。每次 `lua_load` 需要代码块的另一部分时，它都会调用读取器，传递其 `data` 参数。读取器必须返回指向包含代码块新部分的内存块的指针，并将 `size` 设置为块大小。该块必须存在，直到再次调用读取器函数。要发出代码块结束的信号，读取器必须返回 `NULL` 或将 `size` 设置为零。读取器函数可以返回任何大于零的大小的部分。

### lua_register

`void lua_register (lua_State *L, const char *name, lua_CFunction f);`

[-0, +0, e]

Sets the C function f as the new value of global name. It is defined as a macro:
将 **C 函数** `f` 设置为全局变量 `name` 的新值。它定义为宏：

```c
#define lua_register(L,n,f) \
       (lua_pushcfunction(L, f), lua_setglobal(L, n))
```

### lua_remove

`void lua_remove (lua_State *L, int index);`

[-1, +0, -]

Removes the element at the given valid index, shifting down the elements above this index to fill the gap. This function cannot be called with a pseudo-index, because a pseudo-index is not an actual stack position.
移除给定有效索引处的元素，将此索引上方的元素向下移动以填补间隙。此函数不能使用伪索引调用，因为伪索引不是实际的栈位置。

### lua_replace

`void lua_replace (lua_State *L, int index);`

[-1, +0, -]

Moves the top element into the given valid index without shifting any element (therefore replacing the value at that given index), and then pops the top element.
将顶部元素移动到给定的有效索引处，而不移动任何元素（因此替换该给定索引处的值），然后弹出顶部元素。

### lua_resetthread

`int lua_resetthread (lua_State *L);`

[-0, +?, -]

This function is deprecated; it is equivalent to lua_closethread with from being NULL.
此函数已弃用；它等价于 `lua_closethread` 且 `from` 为 `NULL`。

### lua_resume

`int lua_resume (lua_State *L, lua_State *from, int nargs, int *nresults);`

[-?, +?, -]

Starts and resumes a coroutine in the given thread L.
在给定线程 `L` 中启动和恢复协程。

To start a coroutine, you push the main function plus any arguments onto the empty stack of the thread, then you call lua_resume, with nargs being the number of arguments. This call returns when the coroutine suspends or finishes its execution. When it returns, *nresults is updated and the top of the stack contains the *nresults values passed to lua_yield or returned by the body function. lua_resume returns LUA_YIELD if the coroutine yields, LUA_OK if the coroutine finishes its execution without errors, or an error code in case of errors (see §4.4.1). In case of errors, the error object is on the top of the stack.
要启动协程，请将主函数加上任何参数推入线程的空栈，然后调用 `lua_resume`，`nargs` 为参数数量。当协程挂起或完成其执行时，此调用返回。返回时，`*nresults` 被更新，栈顶包含传递给 `lua_yield` 或由主体函数返回的 `*nresults` 个值。如果协程让出，`lua_resume` 返回 `LUA_YIELD`；如果协程无错误完成其执行，则返回 `LUA_OK`；在出错的情况下返回错误码（见 §4.4.1）。在出错的情况下，错误对象在栈顶。

To resume a coroutine, you remove the *nresults yielded values from its stack, push the values to be passed as results from yield, and then call lua_resume.
要恢复协程，请从其栈中移除 `*nresults` 个让出的值，推入要作为 yield 结果传递的值，然后调用 `lua_resume`。

The parameter from represents the coroutine that is resuming L. If there is no such coroutine, this parameter can be NULL.
参数 `from` 表示正在恢复 `L` 的协程。如果没有这样的协程，此参数可以为 `NULL`。

- **实现**（implementation）：`ldo.c:lua_resume` 检查线程状态（`LUA_OK` 或 `LUA_YIELD`），复制 `nCcalls` 后通过 `luaD_rawrunprotected` 调用 `resume`；返回时根据状态计算 `*nresults`（让出结果数或函数返回值数量）。

### lua_rotate

`void lua_rotate (lua_State *L, int idx, int n);`

[-0, +0, -]

Rotates the stack elements between the valid index idx and the top of the stack. The elements are rotated n positions in the direction of the top, for a positive n, or -n positions in the direction of the bottom, for a negative n. The absolute value of n must not be greater than the size of the slice being rotated. This function cannot be called with a pseudo-index, because a pseudo-index is not an actual stack position.
旋转有效索引 `idx` 和栈顶之间的栈元素。对于正 `n`，元素向栈顶方向旋转 `n` 个位置；对于负 `n`，元素向栈底方向旋转 `-n` 个位置。`n` 的绝对值不得大于被旋转切片的大小。此函数不能使用伪索引调用，因为伪索引不是实际的栈位置。

### lua_setallocf

`void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);`

[-0, +0, -]

Changes the allocator function of a given state to f with user data ud.
将给定状态的分配器函数更改为 `f`，用户数据为 `ud`。

### lua_setfield

`void lua_setfield (lua_State *L, int index, const char *k);`

[-1, +0, e]

Does the equivalent to t[k] = v, where t is the value at the given index and v is the value on the top of the stack. This function pops the value from the stack. As in Lua, this function may trigger a metamethod for the "newindex" event (see §2.4).
执行 `t[k] = v` 的等效操作，其中 `t` 是给定索引处的值，`v` 是栈顶的值。此函数从栈中弹出值。与 Lua 中一样，此函数可能会触发 "newindex" 事件的元方法（见 §2.4）。

- **实现**（implementation）：`lapi.c:lua_setfield` 将字符串键内部化为 `TString` 后压入栈顶，再走与 `lua_settable` 类似的 `luaV_fastget` / `luaV_finishset` 路径完成赋值。

### lua_setglobal

`void lua_setglobal (lua_State *L, const char *name);`

[-1, +0, e]

Pops a value from the stack and sets it as the new value of global name.
从栈中弹出一个值，并将其设置为全局变量 `name` 的新值。

- **实现**（implementation）：`lapi.c:lua_setglobal` 获取全局表（`_G`）后，通过 `auxsetstr` 以 `TString` 为键完成赋值，逻辑与 `lua_setfield` 相同。

### lua_seti

`void lua_seti (lua_State *L, int index, lua_Integer n);`

[-1, +0, e]

Does the equivalent to t[n] = v, where t is the value at the given index and v is the value on the top of the stack. This function pops the value from the stack. As in Lua, this function may trigger a metamethod for the "newindex" event (see §2.4).
执行 `t[n] = v` 的等效操作，其中 `t` 是给定索引处的值，`v` 是栈顶的值。此函数从栈中弹出值。与 Lua 中一样，此函数可能会触发 "newindex" 事件的元方法（见 §2.4）。

### lua_setiuservalue

`int lua_setiuservalue (lua_State *L, int index, int n);`

[-1, +0, -]

Pops a value from the stack and sets it as the new n-th user value associated to the full userdata at the given index. Returns 0 if the userdata does not have that value.
从栈中弹出一个值，并将其设置为给定索引处完整 userdata 的新第 `n` 个用户值。如果 userdata 没有该值，则返回 0。

- **实现**（implementation）：`lapi.c:lua_setiuservalue` 检查 `n` 是否在 `[1, nuvalue]` 范围内，命中则通过 `setobj` 将栈顶值复制到 `uvalue(o)->uv[n-1].uv`，并执行 `luaC_barrierback` GC 屏障。

### lua_setmetatable

`int lua_setmetatable (lua_State *L, int index);`

[-1, +0, -]

Pops a table or nil from the stack and sets that value as the new metatable for the value at the given index. (nil means no metatable.)
从栈中弹出一个表或 `nil`，并将该值设置为给定索引处值的新元表。（`nil` 表示没有元表。）

(For historical reasons, this function returns an int, which now is always 1.)
（由于历史原因，此函数返回 `int`，现在始终为 1。）

- **实现**（implementation）：`lapi.c:lua_setmetatable` 根据对象类型分别设置元表指针，并执行 `luaC_objbarrier` 和 `luaC_checkfinalizer` 以处理 GC 屏障和终结器标记。

### lua_settable

`void lua_settable (lua_State *L, int index);`

[-2, +0, e]

Does the equivalent to t[k] = v, where t is the value at the given index, v is the value on the top of the stack, and k is the value just below the top. This function pops both the key and the value from the stack. As in Lua, this function may trigger a metamethod for the "newindex" event (see §2.4).
执行 `t[k] = v` 的等效操作，其中 `t` 是给定索引处的值，`v` 是栈顶的值，`k` 是栈顶正下方的值。此函数从栈中弹出键和值。与 Lua 中一样，此函数可能会触发 "newindex" 事件的元方法（见 §2.4）。

- **实现**（implementation）：`lapi.c:lua_settable` 先尝试 `luaV_fastget` 查找目标位置，命中则走 `luaV_finishfastset` 快速写入，否则走 `luaV_finishset` 慢路径，可能调用 `__newindex` 元方法。

### lua_settop

`void lua_settop (lua_State *L, int index);`

[-?, +?, e]

Accepts any index, or 0, and sets the stack top to this index. If the new top is greater than the old one, then the new elements are filled with nil. If index is 0, then all stack elements are removed.
接受任何索引，或 0，并将栈顶设置为此索引。如果新栈顶大于旧栈顶，则新元素用 `nil` 填充。如果索引为 0，则移除所有栈元素。

This function can run arbitrary code when removing an index marked as to-be-closed from the stack.
当从栈中移除标记为待关闭的索引时，此函数可以运行任意代码。

- **实现**（implementation）：`lapi.c:lua_settop` 根据索引正负计算新的栈顶位置；扩展时填充 `nil`，收缩时若触及待关闭槽则调用 `luaF_close` 执行 `__close` 元方法。

### lua_setwarnf

`void lua_setwarnf (lua_State *L, lua_WarnFunction f, void *ud);`

[-0, +0, -]

Sets the warning function to be used by Lua to emit warnings (see lua_WarnFunction). The ud parameter sets the value ud passed to the warning function.
设置 Lua 用于发出警告的警告函数（见 `lua_WarnFunction`）。`ud` 参数设置传递给警告函数的 `ud` 值。

### lua_State

`typedef struct lua_State lua_State;`

An opaque structure that points to a thread and indirectly (through the thread) to the whole state of a Lua interpreter. The Lua library is fully reentrant: it has no global variables. All information about a state is accessible through this structure.
一个不透明的结构，指向线程并间接地（通过线程）指向 Lua 解释器的整个状态。Lua 库是完全可重入的：它没有全局变量。有关状态的所有信息都可以通过此结构访问。

A pointer to this structure must be passed as the first argument to every function in the library, except to lua_newstate, which creates a Lua state from scratch.
指向此结构的指针必须作为第一个参数传递给库中的每个函数，除了 `lua_newstate`，它从头开始创建 Lua 状态。

### lua_status

`int lua_status (lua_State *L);`

[-0, +0, -]

Returns the status of the thread L.
返回线程 `L` 的状态。

The status can be LUA_OK for a normal thread, an error code if the thread finished the execution of a lua_resume with an error, or LUA_YIELD if the thread is suspended.
正常线程的状态可以是 `LUA_OK`；如果线程以错误完成 `lua_resume` 的执行，则为错误码；如果线程被挂起，则为 `LUA_YIELD`。

You can call functions only in threads with status LUA_OK. You can resume threads with status LUA_OK (to start a new coroutine) or LUA_YIELD (to resume a coroutine).
你只能在状态为 `LUA_OK` 的线程中调用函数。你可以恢复状态为 `LUA_OK`（启动新协程）或 `LUA_YIELD`（恢复协程）的线程。

### lua_stringtonumber

`size_t lua_stringtonumber (lua_State *L, const char *s);`

[-0, +1, -]

Converts the zero-terminated string s to a number, pushes that number into the stack, and returns the total size of the string, that is, its length plus one. The conversion can result in an integer or a float, according to the lexical conventions of Lua (see §3.1). The string may have leading and trailing whitespaces and a sign. If the string is not a valid numeral, returns 0 and pushes nothing.
将以零结尾的字符串 `s` 转换为数字，将该数字推入栈，并返回字符串的总大小，即其长度加一。根据 Lua 的词法约定（见 §3.1），转换可以产生整数或浮点数。字符串可能有前导和尾随空白以及符号。如果字符串不是有效的数字，则返回 0 且不推送任何内容。

(Note that the result can be used as a boolean, true if the conversion succeeds.)
（请注意，结果可以用作布尔值，如果转换成功则为 true。）

### lua_toboolean

`int lua_toboolean (lua_State *L, int index);`

[-0, +0, -]

Converts the Lua value at the given index to a C boolean value (0 or 1). Like all tests in Lua, lua_toboolean returns true for any Lua value different from false and nil; otherwise it returns false.
将给定索引处的 Lua 值转换为 **C 布尔值**（0 或 1）。与 Lua 中的所有测试一样，`lua_toboolean` 对于任何不同于 `false` 和 `nil` 的 Lua 值返回 true；否则返回 false。

(If you want to accept only actual boolean values, use lua_isboolean to test the value's type.)
（如果你只想接受实际的布尔值，请使用 `lua_isboolean` 来测试值的类型。）

### lua_tocfunction

`lua_CFunction lua_tocfunction (lua_State *L, int index);`

[-0, +0, -]

Converts a value at the given index to a C function. That value must be a C function; otherwise, returns NULL.
将给定索引处的值转换为 **C 函数**。该值必须是 **C 函数**；否则，返回 `NULL`。

### lua_toclose

`void lua_toclose (lua_State *L, int index);`

[-0, +0, v]

Marks the given index in the stack as a to-be-closed slot (see §3.3.8). Like a to-be-closed variable in Lua, the value at that slot in the stack will be closed when it goes out of scope. Here, in the context of a C function, to go out of scope means that the running function returns to Lua, or there is an error, or the slot is removed from the stack through lua_settop or lua_pop, or there is a call to lua_closeslot.
将栈中的给定索引标记为待关闭槽（见 §3.3.8）。与 Lua 中的待关闭变量一样，栈中该槽的值在超出作用域时将被关闭。在这里，在 **C 函数**的上下文中，超出作用域意味着运行函数返回到 Lua，或者出现错误，或者通过 `lua_settop` 或 `lua_pop` 从栈中移除槽，或者调用 `lua_closeslot`。

A slot marked as to-be-closed should not be removed from the stack by any other function in the API except lua_settop or lua_pop, unless previously deactivated by lua_closeslot.
标记为待关闭的槽不应被 API 中的任何其他函数从栈中移除，除非先前由 `lua_closeslot` 停用。

This function raises an error if the value at the given slot neither has a __close metamethod nor is a false value.
如果给定槽处的值既没有 `__close` 元方法也不是假值，则此函数会引发错误。

This function should not be called for an index that is equal to or below an active to-be-closed slot.
不应为等于或低于活动待关闭槽的索引调用此函数。

Note that, both in case of errors and of a regular return, by the time the __close metamethod runs, the C stack was already unwound, so that any automatic C variable declared in the calling function (e.g., a buffer) will be out of scope.
请注意，无论是在错误还是正常返回的情况下，当 `__close` 元方法运行时，**C 栈**已经展开，因此调用函数中声明的任何自动 **C 变量**（例如缓冲区）都将超出作用域。

### lua_tointeger

`lua_Integer lua_tointeger (lua_State *L, int index);`

[-0, +0, -]

Equivalent to lua_tointegerx with isnum equal to NULL.
等价于 `lua_tointegerx` 且 `isnum` 等于 `NULL`。

### lua_tointegerx

`lua_Integer lua_tointegerx (lua_State *L, int index, int *isnum);`

[-0, +0, -]

Converts the Lua value at the given index to the signed integral type lua_Integer. The Lua value must be an integer, or a number or string convertible to an integer (see §3.4.3); otherwise, lua_tointegerx returns 0.
将给定索引处的 Lua 值转换为有符号整数类型 `lua_Integer`。Lua 值必须是整数，或可转换为整数的数字或字符串（见 §3.4.3）；否则，`lua_tointegerx` 返回 0。

If isnum is not NULL, its referent is assigned a boolean value that indicates whether the operation succeeded.
如果 `isnum` 不为 `NULL`，则其引用被赋值为表示操作是否成功的布尔值。

- **实现**（implementation）：`lapi.c:lua_tointegerx` 通过 `tointeger` 宏尝试将 `TValue` 转为 `lua_Integer`（支持从浮点和字符串转换），并通过 `isnum` 输出是否成功。

### lua_tolstring

`const char *lua_tolstring (lua_State *L, int index, size_t *len);`

[-0, +0, m]

Converts the Lua value at the given index to a C string. If len is not NULL, it sets *len with the string length. The Lua value must be a string or a number; otherwise, the function returns NULL.
将给定索引处的 Lua 值转换为 **C 字符串**。如果 `len` 不为 `NULL`，则用字符串长度设置 `*len`。Lua 值必须是字符串或数字；否则，函数返回 `NULL`。

If the value is a number, then lua_tolstring also changes the actual value in the stack to a string. (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
如果值是数字，则 `lua_tolstring` 还会将栈中的实际值更改为字符串。（当在表遍历期间将 `lua_tolstring` 应用于键时，这种更改会混淆 `lua_next`。）

lua_tolstring returns a pointer to a string inside the Lua state (see §4.1.3). This string always has a zero (\0) after its last character (as in C), but can contain other zeros in its body.
`lua_tolstring` 返回指向 Lua 状态内部字符串的指针（见 §4.1.3）。此字符串在其最后一个字符之后始终有一个零（`\0`）（与 C 中一样），但其主体中可以包含其他零。

This function can raise memory errors only when converting a number to a string (as then it may create a new string).
此函数仅在将数字转换为字符串时可能引发内存错误（因为那时它可能会创建新字符串）。

- **实现**（implementation）：`lapi.c:lua_tolstring` 若值不是字符串则先检查是否可转换（`cvt2str`），可转换时调用 `luaO_tostring` 创建新字符串并替换栈中原值；最终返回 `getstr(tsvalue(o))` 指向的内部字符串数据。

### lua_tonumber

`lua_Number lua_tonumber (lua_State *L, int index);`

[-0, +0, -]

Equivalent to lua_tonumberx with isnum equal to NULL.
等价于 `lua_tonumberx` 且 `isnum` 等于 `NULL`。

### lua_tonumberx

`lua_Number lua_tonumberx (lua_State *L, int index, int *isnum);`

[-0, +0, -]

Converts the Lua value at the given index to the C type lua_Number (see lua_Number). The Lua value must be a number or a string convertible to a number (see §3.4.3); otherwise, lua_tonumberx returns 0.
将给定索引处的 Lua 值转换为 **C 类型** `lua_Number`（见 `lua_Number`）。Lua 值必须是数字或可转换为数字的字符串（见 §3.4.3）；否则，`lua_tonumberx` 返回 0。

If isnum is not NULL, its referent is assigned a boolean value that indicates whether the operation succeeded.
如果 `isnum` 不为 `NULL`，则其引用被赋值为表示操作是否成功的布尔值。

- **实现**（implementation）：`lapi.c:lua_tonumberx` 通过 `tonumber` 宏尝试将 `TValue` 转为 `lua_Number`（整数会先转为浮点），并通过 `isnum` 输出是否成功。

### lua_topointer

`const void *lua_topointer (lua_State *L, int index);`

[-0, +0, -]

Converts the value at the given index to a generic C pointer (void*). The value can be a userdata, a table, a thread, a string, or a function; otherwise, lua_topointer returns NULL. Different objects will give different pointers. There is no way to convert the pointer back to its original value.
将给定索引处的值转换为通用 **C 指针**（`void*`）。值可以是 userdata、表、线程、字符串或函数；否则，`lua_topointer` 返回 `NULL`。不同的对象将给出不同的指针。无法将指针转换回其原始值。

Typically this function is used only for hashing and debug information.
通常此函数仅用于哈希和调试信息。

### lua_tostring

`const char *lua_tostring (lua_State *L, int index);`

[-0, +0, m]

Equivalent to lua_tolstring with len equal to NULL.
等价于 `lua_tolstring` 且 `len` 等于 `NULL`。

### lua_tothread

`lua_State *lua_tothread (lua_State *L, int index);`

[-0, +0, -]

Converts the value at the given index to a Lua thread (represented as lua_State*). This value must be a thread; otherwise, the function returns NULL.
将给定索引处的值转换为 Lua 线程（表示为 `lua_State*`）。此值必须是线程；否则，函数返回 `NULL`。

### lua_touserdata

`void *lua_touserdata (lua_State *L, int index);`

[-0, +0, -]

If the value at the given index is a full userdata, returns its memory-block address. If the value is a light userdata, returns its value (a pointer). Otherwise, returns NULL.
如果给定索引处的值是完整 userdata，则返回其内存块地址。如果值是轻量 userdata，则返回其值（指针）。否则，返回 `NULL`。

### lua_type

`int lua_type (lua_State *L, int index);`

[-0, +0, -]

Returns the type of the value in the given valid index, or LUA_TNONE for a non-valid but acceptable index. The types returned by lua_type are coded by the following constants defined in lua.h:
返回给定有效索引中值的类型，对于无效但可接受的索引返回 `LUA_TNONE`。`lua_type` 返回的类型由 `lua.h` 中定义的以下常量编码：

- `LUA_TNIL`
- `LUA_TNUMBER`
- `LUA_TBOOLEAN`
- `LUA_TSTRING`
- `LUA_TTABLE`
- `LUA_TFUNCTION`
- `LUA_TUSERDATA`
- `LUA_TTHREAD`
- `LUA_TLIGHTUSERDATA`

- **实现**（implementation）：`lapi.c:lua_type` 通过 `index2value` 获取 `TValue*`，有效时返回 `ttype(o)`（取低 4 位的基础类型），无效时返回 `LUA_TNONE`。

### lua_typename

`const char *lua_typename (lua_State *L, int tp);`

[-0, +0, -]

Returns the name of the type encoded by the value tp, which must be one the values returned by lua_type.
返回由值 `tp` 编码的类型的名称，`tp` 必须是 `lua_type` 返回的值之一。

- **实现**（implementation）：`lapi.c:lua_typename` 直接查表 `luaT_typename`，将类型常量映射为字符串（如 `"nil"`、`"number"`、`"string"` 等）。

### lua_Unsigned

`typedef ... lua_Unsigned;`

The unsigned version of lua_Integer.
`lua_Integer` 的无符号版本。

### lua_upvalueindex

`int lua_upvalueindex (int i);`

[-0, +0, -]

Returns the pseudo-index that represents the i-th upvalue of the running function (see §4.2). i must be in the range [1,256].
返回表示运行函数的第 `i` 个上值的伪索引（见 §4.2）。`i` 必须在范围 `[1,256]` 内。

- **实现**（implementation）：`lua_upvalueindex` 是宏，定义为 `(LUA_REGISTRYINDEX - (i))`，利用负索引越界区域表示上值位置；在 `index2value` 中对小于 `LUA_REGISTRYINDEX` 的索引解析为 `CClosure->upvalue` 数组。

### lua_version

`lua_Number lua_version (lua_State *L);`

[-0, +0, -]

Returns the version number of this core.
返回此核心的版本号。

### lua_WarnFunction

`typedef void (*lua_WarnFunction) (void *ud, const char *msg, int tocont);`

The type of warning functions, called by Lua to emit warnings. The first parameter is an opaque pointer set by lua_setwarnf. The second parameter is the warning message. The third parameter is a boolean that indicates whether the message is to be continued by the message in the next call.
**警告函数**的类型，由 Lua 调用以发出警告。第一个参数是由 `lua_setwarnf` 设置的不透明指针。第二个参数是警告消息。第三个参数是一个布尔值，指示消息是否要在下一次调用中继续。

See warn for more details about warnings.
有关警告的更多详细信息，请参见 `warn`。

### lua_warning

`void lua_warning (lua_State *L, const char *msg, int tocont);`

[-0, +0, -]

Emits a warning with the given message. A message in a call with tocont true should be continued in another call to this function.
使用给定消息发出警告。在 `tocont` 为 true 的调用中，消息应在对此函数的另一次调用中继续。

See warn for more details about warnings.
有关警告的更多详细信息，请参见 `warn`。

### lua_Writer

`typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);`

The type of the writer function used by lua_dump. Every time lua_dump produces another piece of chunk, it calls the writer, passing along the buffer to be written (p), its size (sz), and the ud parameter supplied to lua_dump.
`lua_dump` 使用的写入器函数的类型。每次 `lua_dump` 生成代码块的另一部分时，它都会调用写入器，传递要写入的缓冲区（`p`）、其大小（`sz`）以及提供给 `lua_dump` 的 `ud` 参数。

The writer returns an error code: 0 means no errors; any other value means an error and stops lua_dump from calling the writer again.
写入器返回错误码：0 表示没有错误；任何其他值都表示错误，并阻止 `lua_dump` 再次调用写入器。

### lua_xmove

`void lua_xmove (lua_State *from, lua_State *to, int n);`

[-?, +?, -]

Exchange values between different threads of the same state.
在同一状态的不同线程之间交换值。

This function pops n values from the stack from, and pushes them onto the stack to.
此函数从栈 `from` 中弹出 `n` 个值，并将它们推入栈 `to`。

### lua_yield

`int lua_yield (lua_State *L, int nresults);`

[-?, +?, v]

This function is equivalent to lua_yieldk, but it has no continuation (see §4.5). Therefore, when the thread resumes, it continues the function that called the function calling lua_yield. To avoid surprises, this function should be called only in a tail call.
此函数等价于 `lua_yieldk`，但它没有延续（见 §4.5）。因此，当线程恢复时，它会继续调用调用 `lua_yield` 的函数的函数。为避免意外，此函数应仅在尾调用中调用。

### lua_yieldk

`int lua_yieldk (lua_State *L, int nresults, lua_KContext ctx, lua_KFunction k);`

[-?, +?, v]

Yields a coroutine (thread).
让出协程（线程）。

When a C function calls lua_yieldk, the running coroutine suspends its execution, and the call to lua_resume that started this coroutine returns. The parameter nresults is the number of values from the stack that will be passed as results to lua_resume.
当 **C 函数**调用 `lua_yieldk` 时，运行中的协程挂起其执行，启动此协程的 `lua_resume` 调用返回。参数 `nresults` 是从栈中传递作为 `lua_resume` 结果的值的数量。

When the coroutine is resumed again, Lua calls the given continuation function k to continue the execution of the C function that yielded (see §4.5). This continuation function receives the same stack from the previous function, with the n results removed and replaced by the arguments passed to lua_resume. Moreover, the continuation function receives the value ctx that was passed to lua_yieldk.
当协程再次恢复时，Lua 调用给定的延续函数 `k` 来继续让出的 **C 函数**的执行（见 §4.5）。此延续函数从上一个函数接收相同的栈，`n` 个结果被移除并由传递给 `lua_resume` 的参数替换。此外，延续函数接收传递给 `lua_yieldk` 的值 `ctx`。

Usually, this function does not return; when the coroutine eventually resumes, it continues executing the continuation function. However, there is one special case, which is when this function is called from inside a line or a count hook (see §4.7). In that case, lua_yieldk should be called with no continuation (probably in the form of lua_yield) and no results, and the hook should return immediately after the call. Lua will yield and, when the coroutine resumes again, it will continue the normal execution of the (Lua) function that triggered the hook.
通常，此函数不会返回；当协程最终恢复时，它会继续执行延续函数。但是，有一种特殊情况，即当此函数从行或计数钩子内部调用时（见 §4.7）。在这种情况下，`lua_yieldk` 应该在没有延续（可能以 `lua_yield` 的形式）且没有结果的情况下调用，并且钩子应在调用后立即返回。Lua 将让出，当协程再次恢复时，它将继续触发钩子的（Lua）函数的正常执行。

This function can raise an error if it is called from a thread with a pending C call with no continuation function (what is called a C-call boundary), or it is called from a thread that is not running inside a resume (typically the main thread).
如果从未具有延续函数的挂起 **C 调用**的线程调用此函数（称为 **C 调用边界**），或者从不在恢复内部运行的线程（通常是主线程）调用此函数，则此函数可能引发错误。

- **实现**（implementation）：`ldo.c:lua_yieldk` 将线程状态设为 `LUA_YIELD`，保存让出结果数量到 `ci->u2.nyield`；恢复时由 `lua_resume` 通过 `luaD_rawrunprotected` 重新进入执行流。

## 4.7 – The Debug Interface（调试接口）

Lua has no built-in debugging facilities. Instead, it offers a special interface by means of functions and hooks. This interface allows the construction of different kinds of debuggers, profilers, and other tools that need "inside information" from the interpreter.
Lua 没有内置调试工具。相反，它通过函数和钩子提供特殊接口。该接口允许构建不同类型的调试器、分析器和其他需要解释器"内部信息"的工具。

### lua_Debug

```c
typedef struct lua_Debug {
  int event;
  const char *name;           /* (n) */
  const char *namewhat;       /* (n) */
  const char *what;           /* (S) */
  const char *source;         /* (S) */
  size_t srclen;              /* (S) */
  int currentline;            /* (l) */
  int linedefined;            /* (S) */
  int lastlinedefined;        /* (S) */
  unsigned char nups;         /* (u) number of upvalues */
  unsigned char nparams;      /* (u) number of parameters */
  char isvararg;              /* (u) */
  char istailcall;            /* (t) */
  unsigned short ftransfer;   /* (r) index of first value transferred */
  unsigned short ntransfer;   /* (r) number of transferred values */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  other fields
} lua_Debug;
```

A structure used to carry different pieces of information about a function or an activation record. lua_getstack fills only the private part of this structure, for later use. To fill the other fields of lua_Debug with useful information, you must call lua_getinfo with an appropriate parameter.
用于携带有关函数或活动记录的不同信息的结构。`lua_getstack` 仅填充此结构的私有部分，供以后使用。要使用有用信息填充 `lua_Debug` 的其他字段，你必须使用适当的参数调用 `lua_getinfo`。

The fields of lua_Debug have the following meaning:
`lua_Debug` 的字段具有以下含义：

- **source**：创建函数的代码块的来源。如果 `source` 以 `@` 开头，则表示函数定义在文件中，文件名跟在 `@` 后面。如果 `source` 以 `=` 开头，则其余内容以用户相关的方式描述来源。否则，函数定义在字符串中，`source` 就是该字符串。
- **srclen**：字符串 `source` 的长度。
- **short_src**：`source` 的"可打印"版本，用于错误消息。
- **linedefined**：函数定义开始的行号。
- **lastlinedefined**：函数定义结束的行号。
- **what**：如果函数是 Lua 函数，则为字符串 `"Lua"`；如果是 **C 函数**，则为 `"C"`；如果是代码块的主部分，则为 `"main"`。
- **currentline**：给定函数正在执行的当前行。当没有行信息可用时，`currentline` 设置为 -1。
- **name**：给定函数的合理名称。由于 Lua 中的函数是一等值，它们没有固定名称：某些函数可以是多个全局变量的值，而其他函数只能存储在表字段中。`lua_getinfo` 函数检查函数的调用方式以找到合适的名称。如果找不到名称，则 `name` 设置为 `NULL`。
- **namewhat**：解释 `name` 字段。根据函数的调用方式，`namewhat` 的值可以是 `"global"`、`"local"`、`"method"`、`"field"`、`"upvalue"` 或 `""`（空字符串）。（当没有其他选项适用时，Lua 使用空字符串。）
- **istailcall**：如果此函数调用是通过尾调用调用的，则为 true。在这种情况下，此级别的调用者不在栈中。
- **nups**：函数的上值数量。
- **nparams**：函数的参数数量（对于 **C 函数**始终为 0）。
- **isvararg**：如果函数是变长函数，则为 true（对于 **C 函数**始终为 true）。
- **ftransfer**：栈中第一个被"传输"的值的索引，即调用中的参数或返回中的返回值。（其他值在连续的索引中。）使用此索引，你可以通过 `lua_getlocal` 和 `lua_setlocal` 访问和修改这些值。此字段仅在调用钩子期间有意义，表示第一个参数，或在返回钩子期间，表示第一个返回值。（对于调用钩子，此值始终为 1。）
- **ntransfer**：传输的值的数量（参见上一项）。（对于 Lua 函数的调用，此值始终等于 `nparams`。）

### lua_gethook

`lua_Hook lua_gethook (lua_State *L);`

[-0, +0, -]

Returns the current hook function.
返回当前的钩子函数。

### lua_gethookcount

`int lua_gethookcount (lua_State *L);`

[-0, +0, -]

Returns the current hook count.
返回当前的钩子计数。

### lua_gethookmask

`int lua_gethookmask (lua_State *L);`

[-0, +0, -]

Returns the current hook mask.
返回当前的钩子掩码。

### lua_getinfo

`int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar);`

[-0|1, +0|1|2, m]

Gets information about a specific function or function invocation.
获取有关特定函数或函数调用的信息。

To get information about a function invocation, the parameter ar must be a valid activation record that was filled by a previous call to lua_getstack or given as argument to a hook (see lua_Hook).
要获取有关函数调用的信息，参数 `ar` 必须是有效的活动记录，该记录由先前对 `lua_getstack` 的调用填充或作为参数传递给钩子（见 `lua_Hook`）。

To get information about a function, you push it onto the stack and start the what string with the character '>'. (In that case, lua_getinfo pops the function from the top of the stack.)
要获取有关函数的信息，请将其推入栈并以字符 `'>'` 开始 `what` 字符串。（在这种情况下，`lua_getinfo` 从栈顶弹出函数。）

Each character in the string what selects some fields of the structure ar to be filled or a value to be pushed on the stack.
字符串 `what` 中的每个字符选择要填充的结构 `ar` 的某些字段或要推入栈的值。

- **'f'**：将给定级别运行的函数推入栈；
- **'l'**：填充字段 `currentline`；
- **'n'**：填充字段 `name` 和 `namewhat`；
- **'r'**：填充字段 `ftransfer` 和 `ntransfer`；
- **'S'**：填充字段 `source`、`short_src`、`linedefined`、`lastlinedefined` 和 `what`；
- **'t'**：填充字段 `istailcall`；
- **'u'**：填充字段 `nups`、`nparams` 和 `isvararg`；
- **'L'**：将表推入栈，其索引是函数中具有某些关联代码的行，即可以放置断点的行。（没有代码的行包括空行和注释。）如果此选项与选项 `'f'` 一起给出，其表在函数之后推入。这是唯一可能引发内存错误的选项。

This function returns 0 to signal an invalid option in what; even then the valid options are handled correctly.
此函数返回 0 以表示 `what` 中的无效选项；即使如此，有效选项也会正确处理。

- **实现**（implementation）：`ldebug.c:lua_getinfo` 根据 `what` 字符串调用 `auxgetinfo` 填充 `lua_Debug` 字段；`'f'` 时压入函数，`'L'` 时通过 `collectvalidlines` 压入行号表。

### lua_getlocal

`const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n);`

[-0, +0|1, -]

Gets information about a local variable or a temporary value of a given activation record or a given function.
获取有关给定活动记录或给定函数的局部变量或临时值的信息。

In the first case, the parameter ar must be a valid activation record that was filled by a previous call to lua_getstack or given as argument to a hook (see lua_Hook). The index n selects which local variable to inspect; see debug.getlocal for details about variable indices and names.
在第一种情况下，参数 `ar` 必须是有效的活动记录，该记录由先前对 `lua_getstack` 的调用填充或作为参数传递给钩子（见 `lua_Hook`）。索引 `n` 选择要检查哪个局部变量；有关变量索引和名称的详细信息，请参见 `debug.getlocal`。

lua_getlocal pushes the variable's value onto the stack and returns its name.
`lua_getlocal` 将变量的值推入栈并返回其名称。

In the second case, ar must be NULL and the function to be inspected must be on the top of the stack. In this case, only parameters of Lua functions are visible (as there is no information about what variables are active) and no values are pushed onto the stack.
在第二种情况下，`ar` 必须为 `NULL`，并且要检查的函数必须在栈顶。在这种情况下，只有 Lua 函数的参数可见（因为没有关于哪些变量处于活动状态的信息），并且没有值被推入栈。

Returns NULL (and pushes nothing) when the index is greater than the number of active local variables.
当索引大于活动局部变量的数量时，返回 `NULL`（且不推送任何内容）。

### lua_getstack

`int lua_getstack (lua_State *L, int level, lua_Debug *ar);`

[-0, +0, -]

Gets information about the interpreter runtime stack.
获取有关解释器运行时栈的信息。

This function fills parts of a lua_Debug structure with an identification of the activation record of the function executing at a given level. Level 0 is the current running function, whereas level n+1 is the function that has called level n (except for tail calls, which do not count in the stack). When called with a level greater than the stack depth, lua_getstack returns 0; otherwise it returns 1.
此函数用执行给定级别的函数的激活记录的标识填充 `lua_Debug` 结构的部分。级别 0 是当前运行的函数，而级别 n+1 是调用级别 n 的函数（尾调用除外，它们在栈中不计数）。当用大于栈深度的级别调用时，`lua_getstack` 返回 0；否则返回 1。

- **实现**（implementation）：`ldebug.c:lua_getstack` 从 `L->ci` 开始沿 `previous` 链遍历 `CallInfo` 链表，跳过尾调用，找到对应层级后将 `ci` 指针保存到 `ar->i_ci`。

### lua_getupvalue

`const char *lua_getupvalue (lua_State *L, int funcindex, int n);`

[-0, +0|1, -]

Gets information about the n-th upvalue of the closure at index funcindex. It pushes the upvalue's value onto the stack and returns its name. Returns NULL (and pushes nothing) when the index n is greater than the number of upvalues.
获取索引 `funcindex` 处闭包的第 `n` 个上值的信息。它将上值的值推入栈并返回其名称。当索引 `n` 大于上值的数量时，返回 `NULL`（且不推送任何内容）。

See debug.getupvalue for more information about upvalues.
有关上值的更多信息，请参见 `debug.getupvalue`。

### lua_Hook

`typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);`

Type for debugging hook functions.
调试钩子函数的类型。

Whenever a hook is called, its ar argument has its field event set to the specific event that triggered the hook. Lua identifies these events with the following constants: LUA_HOOKCALL, LUA_HOOKRET, LUA_HOOKTAILCALL, LUA_HOOKLINE, and LUA_HOOKCOUNT. Moreover, for line events, the field currentline is also set. To get the value of any other field in ar, the hook must call lua_getinfo.
每当调用钩子时，其 `ar` 参数的 `event` 字段设置为触发钩子的特定事件。Lua 使用以下常量标识这些事件：`LUA_HOOKCALL`、`LUA_HOOKRET`、`LUA_HOOKTAILCALL`、`LUA_HOOKLINE` 和 `LUA_HOOKCOUNT`。此外，对于行事件，`currentline` 字段也会设置。要获取 `ar` 中任何其他字段的值，钩子必须调用 `lua_getinfo`。

For call events, event can be LUA_HOOKCALL, the normal value, or LUA_HOOKTAILCALL, for a tail call; in this case, there will be no corresponding return event.
对于调用事件，`event` 可以是 `LUA_HOOKCALL`（正常值），或尾调用的 `LUA_HOOKTAILCALL`；在这种情况下，将没有相应的返回事件。

While Lua is running a hook, it disables other calls to hooks. Therefore, if a hook calls back Lua to execute a function or a chunk, this execution occurs without any calls to hooks.
当 Lua 运行钩子时，它会禁用对钩子的其他调用。因此，如果钩子回调 Lua 以执行函数或代码块，则此执行不会发生任何钩子调用。

Hook functions cannot have continuations, that is, they cannot call lua_yieldk, lua_pcallk, or lua_callk with a non-null k.
钩子函数不能有延续，也就是说，它们不能调用 `lua_yieldk`、`lua_pcallk` 或 `lua_callk` 且 `k` 不为 null。

Hook functions can yield under the following conditions: Only count and line events can yield; to yield, a hook function must finish its execution calling lua_yield with nresults equal to zero (that is, with no values).
钩子函数可以在以下条件下让出：只有计数和行事件可以让出；要让出，钩子函数必须通过调用 `lua_yield` 且 `nresults` 等于零（即没有值）来完成其执行。

### lua_sethook

`void lua_sethook (lua_State *L, lua_Hook f, int mask, int count);`

[-0, +0, -]

Sets the debugging hook function.
设置调试钩子函数。

Argument f is the hook function. mask specifies on which events the hook will be called: it is formed by a bitwise OR of the constants LUA_MASKCALL, LUA_MASKRET, LUA_MASKLINE, and LUA_MASKCOUNT. The count argument is only meaningful when the mask includes LUA_MASKCOUNT.
参数 `f` 是钩子函数。`mask` 指定将在哪些事件上调用钩子：它由常量 `LUA_MASKCALL`、`LUA_MASKRET`、`LUA_MASKLINE` 和 `LUA_MASKCOUNT` 的按位或组成。`count` 参数仅在掩码包含 `LUA_MASKCOUNT` 时有意义。

For each event, the hook is called as explained below:
对于每个事件，钩子的调用方式如下：

- **The call hook**：当解释器调用函数时调用。钩子在 Lua 进入新函数后立即调用。
- **The return hook**：当解释器从函数返回时调用。钩子在 Lua 离开函数之前调用。
- **The line hook**：当解释器即将开始执行新代码行或在代码中跳回时（即使到同一行）调用。此事件仅在 Lua 执行 Lua 函数时发生。
- **The count hook**：在解释器执行每 `count` 条指令后调用。此事件仅在 Lua 执行 Lua 函数时发生。

Hooks are disabled by setting mask to zero.
通过将 `mask` 设置为零来禁用钩子。

- **实现**（implementation）：`ldebug.c:lua_sethook` 将钩子函数、掩码和计数直接写入 `lua_State` 的对应字段；若掩码非零则调用 `settraps` 在 `CallInfo` 中设置陷阱标志，以便 `luaV_execute` 定期检查钩子。

### lua_setlocal

`const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n);`

[-0|1, +0, -]

Sets the value of a local variable of a given activation record. It assigns the value on the top of the stack to the variable and returns its name. It also pops the value from the stack.
设置给定活动记录的局部变量的值。它将栈顶的值赋给变量并返回其名称。它还会从栈中弹出该值。

Returns NULL (and pops nothing) when the index is greater than the number of active local variables.
当索引大于活动局部变量的数量时，返回 `NULL`（且不弹出任何内容）。

Parameters ar and n are as in the function lua_getlocal.
参数 `ar` 和 `n` 与 `lua_getlocal` 函数中的相同。

### lua_setupvalue

`const char *lua_setupvalue (lua_State *L, int funcindex, int n);`

[-0|1, +0, -]

Sets the value of a closure's upvalue. It assigns the value on the top of the stack to the upvalue and returns its name. It also pops the value from the stack.
设置闭包上值的值。它将栈顶的值赋给上值并返回其名称。它还会从栈中弹出该值。

Returns NULL (and pops nothing) when the index n is greater than the number of upvalues.
当索引 `n` 大于上值的数量时，返回 `NULL`（且不弹出任何内容）。

Parameters funcindex and n are as in the function lua_getupvalue.
参数 `funcindex` 和 `n` 与 `lua_getupvalue` 函数中的相同。

### lua_upvalueid

`void *lua_upvalueid (lua_State *L, int funcindex, int n);`

[-0, +0, -]

Returns a unique identifier for the upvalue numbered n from the closure at index funcindex.
返回索引 `funcindex` 处闭包的第 `n` 个上值的唯一标识符。

These unique identifiers allow a program to check whether different closures share upvalues. Lua closures that share an upvalue (that is, that access a same external local variable) will return identical ids for those upvalue indices.
这些唯一标识符允许程序检查不同的闭包是否共享上值。共享上值的 Lua 闭包（即访问相同的外部局部变量）将为这些上值索引返回相同的 ID。

Parameters funcindex and n are as in the function lua_getupvalue, but n cannot be greater than the number of upvalues.
参数 `funcindex` 和 `n` 与 `lua_getupvalue` 函数中的相同，但 `n` 不能大于上值的数量。

### lua_upvaluejoin

`void lua_upvaluejoin (lua_State *L, int funcindex1, int n1, int funcindex2, int n2);`

[-0, +0, -]

Make the n1-th upvalue of the Lua closure at index funcindex1 refer to the n2-th upvalue of the Lua closure at index funcindex2.
使索引 `funcindex1` 处 Lua 闭包的第 `n1` 个上值引用索引 `funcindex2` 处 Lua 闭包的第 `n2` 个上值。

---

# 5 – The Auxiliary Library（辅助库）

The auxiliary library provides several convenient functions to interface C with Lua. While the basic API provides the primitive functions for all interactions between C and Lua, the auxiliary library provides higher-level functions for some common tasks.
**辅助库**提供了几个方便的函数，用于在 C 和 Lua 之间进行交互。虽然基本 API 提供了 **C** 和 Lua 之间所有交互的原始函数，但辅助库为一些常见任务提供了更高级的函数。

All functions and types from the auxiliary library are defined in header file lauxlib.h and have a prefix luaL_.
辅助库中的所有函数和类型都在头文件 `lauxlib.h` 中定义，并具有前缀 `luaL_`。

All functions in the auxiliary library are built on top of the basic API, and so they provide nothing that cannot be done with that API. Nevertheless, the use of the auxiliary library ensures more consistency to your code.
辅助库中的所有函数都建立在基本 API 之上，因此它们不提供任何无法用该 API 完成的功能。然而，使用辅助库可以确保代码具有更高的一致性。

Several functions in the auxiliary library use internally some extra stack slots. When a function in the auxiliary library uses less than five slots, it does not check the stack size; it simply assumes that there are enough slots.
辅助库中的几个函数在内部使用一些额外的栈槽。当辅助库中的函数使用少于五个槽时，它不检查栈大小；它只是假设有足够的槽。

Several functions in the auxiliary library are used to check C function arguments. Because the error message is formatted for arguments (e.g., "bad argument #1"), you should not use these functions for other stack values.
辅助库中的几个函数用于检查 **C 函数**参数。因为错误消息是为参数格式化的（例如，"bad argument #1"），所以你不应将这些函数用于其他栈值。

Functions called luaL_check* always raise an error if the check is not satisfied.
名为 `luaL_check*` 的函数如果检查未满足，则始终引发错误。

## 5.1 – Functions and Types（函数和类型）

### luaL_addchar

`void luaL_addchar (luaL_Buffer *B, char c);`

[-?, +?, m]

Adds the byte c to the buffer B (see luaL_Buffer).
将字节 `c` 添加到缓冲区 `B`（见 `luaL_Buffer`）。

- **实现**（implementation）：定义为宏，若当前空间不足会自动调用 `luaL_prepbuffsize` 扩容。Key source files: `lauxlib.h:luaL_addchar`。

### luaL_addgsub

`const void luaL_addgsub (luaL_Buffer *B, const char *s, const char *p, const char *r);`

[-?, +?, m]

Adds a copy of the string s to the buffer B (see luaL_Buffer), replacing any occurrence of the string p with the string r.
将字符串 `s` 的副本添加到缓冲区 `B`（见 `luaL_Buffer`），将字符串 `p` 的任何出现替换为字符串 `r`。

- **实现**（implementation）：使用 `strstr` 查找匹配子串，逐段将前缀与替换串追加到缓冲区。Key source files: `lauxlib.c:luaL_addgsub`。

### luaL_addlstring

`void luaL_addlstring (luaL_Buffer *B, const char *s, size_t l);`

[-?, +?, m]

Adds the string pointed to by s with length l to the buffer B (see luaL_Buffer). The string can contain embedded zeros.
将 `s` 指向的长度为 `l` 的字符串添加到缓冲区 `B`（见 `luaL_Buffer`）。字符串可以包含嵌入的零。

- **实现**（implementation）：通过 `prepbuffsize` 获取可写空间，再用 `memcpy` 复制数据。Key source files: `lauxlib.c:luaL_addlstring`。

### luaL_addsize

`void luaL_addsize (luaL_Buffer *B, size_t n);`

[-?, +?, -]

Adds to the buffer B a string of length n previously copied to the buffer area (see luaL_prepbuffer).
将长度为 `n` 的字符串添加到缓冲区 `B`，该字符串先前已复制到缓冲区区域（见 `luaL_prepbuffer`）。

- **实现**（implementation）：定义为宏，直接增加缓冲区内部计数 `B->n += n`。Key source files: `lauxlib.h:luaL_addsize`。

### luaL_addstring

`void luaL_addstring (luaL_Buffer *B, const char *s);`

[-?, +?, m]

Adds the zero-terminated string pointed to by s to the buffer B (see luaL_Buffer).
将 `s` 指向的以零结尾的字符串添加到缓冲区 `B`（见 `luaL_Buffer`）。

- **实现**（implementation）：内部调用 `luaL_addlstring` 并自动计算 `strlen(s)`。Key source files: `lauxlib.c:luaL_addstring`。

### luaL_addvalue

`void luaL_addvalue (luaL_Buffer *B);`

[-?, +?, m]

Adds the value on the top of the stack to the buffer B (see luaL_Buffer). Pops the value.
将栈顶的值添加到缓冲区 `B`（见 `luaL_Buffer`）。弹出该值。

- **实现**（implementation）：将栈顶字符串复制到缓冲区内建或动态扩容的空间，随后 `lua_pop` 弹出该值；动态扩容时使用 `-2` 作为 box 位置。Key source files: `lauxlib.c:luaL_addvalue`。

This is the only function on string buffers that can (and must) be called with an extra element on the stack, which is the value to be added to the buffer.
这是字符串缓冲区上唯一可以（且必须）在栈上有额外元素时调用的函数，该额外元素是要添加到缓冲区的值。

### luaL_argcheck

`void lua_Largcheck (lua_State *L, int cond, int arg, const char *extramsg);`

[-0, +0, v]

Checks whether cond is true. If it is not, raises an error with a standard message (see luaL_argerror).
检查 `cond` 是否为 true。如果不是，则引发带有标准消息的错误（见 `luaL_argerror`）。

- **实现**（implementation）：定义为宏，在 `cond` 为假时调用 `luaL_argerror` 抛出错误。Key source files: `lauxlib.h:luaL_argcheck`。

### luaL_argerror

`int luaL_argerror (lua_State *L, int arg, const char *extramsg);`

[-0, +0, v]

Raises an error reporting a problem with argument arg of the C function that called it, using a standard message that includes extramsg as a comment:
引发错误，报告调用它的 **C 函数**的参数 `arg` 有问题，使用包含 `extramsg` 作为注释的标准消息：

```
bad argument #arg to 'funcname' (extramsg)
```

This function never returns.
此函数永远不会返回。

- **实现**（implementation）：通过 `lua_Debug` 获取当前 C 函数名，区分 method 调用调整参数序号，最终调用 `luaL_error` 格式化错误。Key source files: `lauxlib.c:luaL_argerror`。

### luaL_argexpected

`void luaL_argexpected (lua_State *L, int cond, int arg, const char *tname);`

[-0, +0, v]

Checks whether cond is true. If it is not, raises an error about the type of the argument arg with a standard message (see luaL_typeerror).
检查 `cond` 是否为 true。如果不是，则引发有关参数 `arg` 类型的错误，并带有标准消息（见 `luaL_typeerror`）。

- **实现**（implementation）：定义为宏，在 `cond` 为假时调用 `luaL_typeerror` 抛出类型错误。Key source files: `lauxlib.h:luaL_argexpected`。

### luaL_Buffer

`typedef struct luaL_Buffer luaL_Buffer;`

Type for a string buffer.
**字符串缓冲区**的类型。

- **实现**（implementation）：结构体包含内联初始缓冲区 `init.b`，当容量不足时会在栈上创建 userdata box 并通过 `resizebox` 动态扩容。Key source files: `lauxlib.h:luaL_Buffer` / `lauxlib.c:prepbuffsize`。

A string buffer allows C code to build Lua strings piecemeal. Its pattern of use is as follows:
字符串缓冲区允许 **C 代码**逐步构建 Lua 字符串。其使用模式如下：

- First declare a variable b of type luaL_Buffer.
- Then initialize it with a call luaL_buffinit(L, &b).
- Then add string pieces to the buffer calling any of the luaL_add* functions.
- Finish by calling luaL_pushresult(&b). This call leaves the final string on the top of the stack.

If you know beforehand the maximum size of the resulting string, you can use the buffer like this:
如果预先知道结果字符串的最大大小，则可以这样使用缓冲区：

- First declare a variable b of type luaL_Buffer.
- Then initialize it and preallocate a space of size sz with a call luaL_buffinitsize(L, &b, sz).
- Then produce the string into that space.
- Finish by calling luaL_pushresultsize(&b, sz), where sz is the total size of the resulting string copied into that space (which may be less than or equal to the preallocated size).

During its normal operation, a string buffer uses a variable number of stack slots. So, while using a buffer, you cannot assume that you know where the top of the stack is. You can use the stack between successive calls to buffer operations as long as that use is balanced; that is, when you call a buffer operation, the stack is at the same level it was immediately after the previous buffer operation. (The only exception to this rule is luaL_addvalue.) After calling luaL_pushresult, the stack is back to its level when the buffer was initialized, plus the final string on its top.
在正常操作期间，字符串缓冲区使用可变数量的栈槽。因此，在使用缓冲区时，你不能假设你知道栈顶在哪里。你可以在连续调用缓冲区操作之间使用栈，只要该使用是平衡的；也就是说，当你调用缓冲区操作时，栈处于与上次缓冲区操作后立即相同的级别。（此规则的唯一例外是 `luaL_addvalue`。）调用 `luaL_pushresult` 后，栈恢复到缓冲区初始化时的级别，加上其顶部的最终字符串。

### luaL_buffaddr

`char *luaL_buffaddr (luaL_Buffer *B);`

[-0, +0, -]

Returns the address of the current content of buffer B (see luaL_Buffer). Note that any addition to the buffer may invalidate this address.
返回缓冲区 `B` 当前内容的地址（见 `luaL_Buffer`）。请注意，对缓冲区的任何添加都可能使此地址无效。

- **实现**（implementation）：定义为宏，直接返回缓冲区指针 `B->b`。Key source files: `lauxlib.h:luaL_buffaddr`。

### luaL_buffinit

`void luaL_buffinit (lua_State *L, luaL_Buffer *B);`

[-0, +?, -]

Initializes a buffer B (see luaL_Buffer). This function does not allocate any space; the buffer must be declared as a variable.
初始化缓冲区 `B`（见 `luaL_Buffer`）。此函数不分配任何空间；缓冲区必须声明为变量。

- **实现**（implementation）：将缓冲区指向内联数组 `init.b`，并在栈上压入一个 light userdata 作为占位符。Key source files: `lauxlib.c:luaL_buffinit`。

### luaL_bufflen

`size_t luaL_bufflen (luaL_Buffer *B);`

[-0, +0, -]

Returns the length of the current content of buffer B (see luaL_Buffer).
返回缓冲区 `B` 当前内容的长度（见 `luaL_Buffer`）。

- **实现**（implementation）：定义为宏，直接返回已使用长度 `B->n`。Key source files: `lauxlib.h:luaL_bufflen`。

### luaL_buffinitsize

`char *luaL_buffinitsize (lua_State *L, luaL_Buffer *B, size_t sz);`

[-?, +?, m]

Equivalent to the sequence luaL_buffinit, luaL_prepbuffsize.
等价于序列 `luaL_buffinit`、`luaL_prepbuffsize`。

- **实现**（implementation）：先调用 `luaL_buffinit` 初始化，再调用 `prepbuffsize` 预分配 `sz` 字节并返回可写指针。Key source files: `lauxlib.c:luaL_buffinitsize`。

### luaL_buffsub

`void luaL_buffsub (luaL_Buffer *B, int n);`

[-?, +?, -]

Removes n bytes from the buffer B (see luaL_Buffer). The buffer must have at least that many bytes.
从缓冲区 `B` 中移除 `n` 个字节（见 `luaL_Buffer`）。缓冲区必须至少有那么多字节。

- **实现**（implementation）：定义为宏，直接减少内部计数 `B->n -= n`。Key source files: `lauxlib.h:luaL_buffsub`。

### luaL_callmeta

`int luaL_callmeta (lua_State *L, int obj, const char *e);`

[-0, +0|1, e]

Calls a metamethod.
调用元方法。

If the object at index obj has a metatable and this metatable has a field e, this function calls this field passing the object as its only argument. In this case this function returns true and pushes onto the stack the value returned by the call. If there is no metatable or no metamethod, this function returns false without pushing any value on the stack.
如果索引 `obj` 处的对象具有元表，并且此元表具有字段 `e`，则此函数调用此字段，将对象作为其唯一参数传递。在这种情况下，此函数返回 true 并将调用返回的值推入栈。如果没有元表或元方法，此函数返回 false 且不在栈上推送任何值。

### luaL_checkany

`void luaL_checkany (lua_State *L, int arg);`

[-0, +0, v]

Checks whether the function has an argument of any type (including nil) at position arg.
检查函数在位置 `arg` 处是否有任何类型（包括 `nil`）的参数。

- **实现**（implementation）：检查 `lua_type` 是否为 `LUA_TNONE`，若是则调用 `luaL_argerror`。Key source files: `lauxlib.c:luaL_checkany`。

### luaL_checkinteger

`lua_Integer luaL_checkinteger (lua_State *L, int arg);`

[-0, +0, v]

Checks whether the function argument arg is an integer (or can be converted to an integer) and returns this integer.
检查函数参数 `arg` 是否为整数（或可转换为整数）并返回此整数。

- **实现**（implementation）：调用 `lua_tointegerx` 检查转换是否成功，失败时调用 `interror` 给出更精确的错误信息。Key source files: `lauxlib.c:luaL_checkinteger`。

### luaL_checklstring

`const char *luaL_checklstring (lua_State *L, int arg, size_t *l);`

[-0, +0, v]

Checks whether the function argument arg is a string and returns this string; if l is not NULL fills its referent with the string's length.
检查函数参数 `arg` 是否为字符串并返回此字符串；如果 `l` 不为 `NULL`，则用字符串长度填充其引用。

- **实现**（implementation）：调用 `lua_tolstring` 获取字符串，若失败则通过 `tag_error` 引发错误。Key source files: `lauxlib.c:luaL_checklstring`。

This function uses lua_tolstring to get its result, so all conversions and caveats of that function apply here.
此函数使用 `lua_tolstring` 来获取其结果，因此该函数的所有转换和注意事项都适用于此处。

### luaL_checknumber

`lua_Number luaL_checknumber (lua_State *L, int arg);`

[-0, +0, v]

Checks whether the function argument arg is a number and returns this number converted to a lua_Number.
检查函数参数 `arg` 是否为数字并返回转换为 `lua_Number` 的此数字。

- **实现**（implementation）：调用 `lua_tonumberx` 并检查 `isnum` 标志，失败时通过 `tag_error` 引发错误。Key source files: `lauxlib.c:luaL_checknumber`。

### luaL_checkoption

`int luaL_checkoption (lua_State *L, int arg, const char *def, const char *const lst[]);`

[-0, +0, v]

Checks whether the function argument arg is a string and searches for this string in the array lst (which must be NULL-terminated). Returns the index in the array where the string was found. Raises an error if the argument is not a string or if the string cannot be found.
检查函数参数 `arg` 是否为字符串，并在数组 `lst`（必须以 `NULL` 结尾）中搜索此字符串。返回找到字符串的数组索引。如果参数不是字符串或找不到字符串，则引发错误。

- **实现**（implementation）：利用 `luaL_optstring` 处理默认值，再用线性搜索在 `lst` 中匹配，失败时调用 `luaL_argerror`。Key source files: `lauxlib.c:luaL_checkoption`。

If def is not NULL, the function uses def as a default value when there is no argument arg or when this argument is nil.
如果 `def` 不为 `NULL`，则当没有参数 `arg` 或此参数为 `nil` 时，函数使用 `def` 作为默认值。

This is a useful function for mapping strings to C enums. (The usual convention in Lua libraries is to use strings instead of numbers to select options.)
这是将字符串映射到 **C 枚举**的有用函数。（Lua 库中的通常约定是使用字符串而不是数字来选择选项。）

### luaL_checkstack

`void luaL_checkstack (lua_State *L, int sz, const char *msg);`

[-0, +0, v]

Grows the stack size to top + sz elements, raising an error if the stack cannot grow to that size. msg is an additional text to go into the error message (or NULL for no additional text).
将栈大小增加到 `top + sz` 个元素，如果栈无法增长到该大小，则引发错误。`msg` 是要进入错误消息的附加文本（或 `NULL` 表示没有附加文本）。

- **实现**（implementation）：调用 `lua_checkstack` 检查容量，不足时通过 `luaL_error` 抛出 "stack overflow" 错误。Key source files: `lauxlib.c:luaL_checkstack`。

### luaL_checkstring

`const char *luaL_checkstring (lua_State *L, int arg);`

[-0, +0, v]

Checks whether the function argument arg is a string and returns this string.
检查函数参数 `arg` 是否为字符串并返回此字符串。

- **实现**（implementation）：定义为宏，等价于 `luaL_checklstring(L, n, NULL)`。Key source files: `lauxlib.h:luaL_checkstring`。

This function uses lua_tolstring to get its result, so all conversions and caveats of that function apply here.
此函数使用 `lua_tolstring` 来获取其结果，因此该函数的所有转换和注意事项都适用于此处。

### luaL_checktype

`void luaL_checktype (lua_State *L, int arg, int t);`

[-0, +0, v]

Checks whether the function argument arg has type t. See lua_type for the encoding of types for t.
检查函数参数 `arg` 是否具有类型 `t`。有关 `t` 的类型编码，请参见 `lua_type`。

- **实现**（implementation）：直接比较 `lua_type` 返回值，不匹配时调用 `tag_error`（内部调用 `luaL_typeerror`）。Key source files: `lauxlib.c:luaL_checktype`。

### luaL_checkudata

`void *luaL_checkudata (lua_State *L, int arg, const char *tname);`

[-0, +0, v]

Checks whether the function argument arg is a userdata of the type tname (see luaL_newmetatable) and returns the userdata's memory-block address (see lua_touserdata).
检查函数参数 `arg` 是否为类型 `tname` 的 userdata（见 `luaL_newmetatable`）并返回 userdata 的内存块地址（见 `lua_touserdata`）。

- **实现**（implementation）：先调用 `luaL_testudata` 进行测试，若返回 `NULL` 则通过 `luaL_argexpected` 引发类型错误。Key source files: `lauxlib.c:luaL_checkudata`。

### luaL_checkversion

`void luaL_checkversion (lua_State *L);`

[-0, +0, v]

Checks whether the code making the call and the Lua library being called are using the same version of Lua and the same numeric types.
检查进行调用的代码和被调用的 Lua 库是否使用相同版本的 Lua 和相同的数字类型。

- **实现**（implementation）：定义为宏，调用 `luaL_checkversion_` 比较 `LUA_VERSION_NUM` 与数值类型大小 `LUAL_NUMSIZES`。Key source files: `lauxlib.h:luaL_checkversion`。

### luaL_dofile

`int luaL_dofile (lua_State *L, const char *filename);`

[-0, +?, m]

Loads and runs the given file. It is defined as the following macro:
加载并运行给定文件。它定义为以下宏：

```c
(luaL_loadfile(L, filename) || lua_pcall(L, 0, LUA_MULTRET, 0))
```

It returns 0 (LUA_OK) if there are no errors, or 1 in case of errors.
如果没有错误，则返回 0（`LUA_OK`），如果出错则返回 1。

- **实现**（implementation）：定义为宏，先调用 `luaL_loadfile` 加载，成功后再调用 `lua_pcall` 执行。Key source files: `lauxlib.h:luaL_dofile`。

### luaL_dostring

`int luaL_dostring (lua_State *L, const char *str);`

[-0, +?, -]

Loads and runs the given string. It is defined as the following macro:
加载并运行给定字符串。它定义为以下宏：

```c
(luaL_loadstring(L, str) || lua_pcall(L, 0, LUA_MULTRET, 0))
```

It returns 0 (LUA_OK) if there are no errors, or 1 in case of errors.
如果没有错误，则返回 0（`LUA_OK`），如果出错则返回 1。

- **实现**（implementation）：定义为宏，先调用 `luaL_loadstring` 加载，成功后再调用 `lua_pcall` 执行。Key source files: `lauxlib.h:luaL_dostring`。

### luaL_error

`int luaL_error (lua_State *L, const char *fmt, ...);`

[-0, +0, v]

Raises an error. The error message format is given by fmt plus any extra arguments, following the same rules of lua_pushfstring. It also adds at the beginning of the message the file name and the line number where the error occurred, if this information is available.
引发错误。错误消息格式由 `fmt` 加上任何额外参数给出，遵循与 `lua_pushfstring` 相同的规则。如果此信息可用，它还会在消息开头添加错误发生的文件名和行号。

- **实现**（implementation）：先调用 `luaL_where` 获取位置前缀，再用 `lua_pushvfstring` 格式化消息，最后拼接并调用 `lua_error`。Key source files: `lauxlib.c:luaL_error`。

This function never returns, but it is an idiom to use it in C functions as return luaL_error(args).
此函数永远不会返回，但在 **C 函数**中使用它作为 `return luaL_error(args)` 是一种惯用法。

### luaL_execresult

`int luaL_execresult (lua_State *L, int stat);`

[-0, +3, m]

This function produces the return values for process-related functions in the standard library (os.execute and io.close).
此函数为标准库中与进程相关的函数（`os.execute` 和 `io.close`）生成返回值。

- **实现**（implementation）：解析子进程退出状态（含 POSIX 下的 `WEXITSTATUS`/`WTERMSIG`），返回布尔状态、终止方式字符串与状态码。Key source files: `lauxlib.c:luaL_execresult`。

### luaL_fileresult

`int luaL_fileresult (lua_State *L, int stat, const char *fname);`

[-0, +1|3, m]

This function produces the return values for file-related functions in the standard library (io.open, os.rename, file:seek, etc.).
此函数为标准库中与文件相关的函数（`io.open`、`os.rename`、`file:seek` 等）生成返回值。

- **实现**（implementation）：成功时压入 `true`，失败时压入 `fail`、错误消息与 `errno`。Key source files: `lauxlib.c:luaL_fileresult`。

### luaL_getmetafield

`int luaL_getmetafield (lua_State *L, int obj, const char *e);`

[-0, +0|1, m]

Pushes onto the stack the field e from the metatable of the object at index obj and returns the type of the pushed value. If the object does not have a metatable, or if the metatable does not have this field, pushes nothing and returns LUA_TNIL.
将索引 `obj` 处对象的元表中的字段 `e` 推入栈，并返回推送值的类型。如果对象没有元表，或者元表没有此字段，则不推送任何内容并返回 `LUA_TNIL`。

- **实现**（implementation）：先 `lua_getmetatable` 获取元表，再 `lua_rawget` 读取字段，根据结果调整栈。Key source files: `lauxlib.c:luaL_getmetafield`。

### luaL_getmetatable

`int luaL_getmetatable (lua_State *L, const char *tname);`

[-0, +1, m]

Pushes onto the stack the metatable associated with the name tname in the registry (see luaL_newmetatable), or nil if there is no metatable associated with that name. Returns the type of the pushed value.
将注册表中与名称 `tname` 关联的元表推入栈（见 `luaL_newmetatable`），如果没有与该名称关联的元表，则推入 `nil`。返回推送值的类型。

- **实现**（implementation）：定义为宏，等价于 `lua_getfield(L, LUA_REGISTRYINDEX, tname)`。Key source files: `lauxlib.h:luaL_getmetatable`。

### luaL_getsubtable

`int luaL_getsubtable (lua_State *L, int idx, const char *fname);`

[-0, +1, e]

Ensures that the value t[fname], where t is the value at index idx, is a table, and pushes that table onto the stack. Returns true if it finds a previous table there and false if it creates a new table.
确保值 `t[fname]`（其中 `t` 是索引 `idx` 处的值）是表，并将该表推入栈。如果它在其中找到先前的表，则返回 true，如果创建新表，则返回 false。

- **实现**（implementation）：先尝试 `lua_getfield` 获取字段，若非表则弹出并新建表后赋值。Key source files: `lauxlib.c:luaL_getsubtable`。

### luaL_gsub

`const char *luaL_gsub (lua_State *L, const char *s, const char *p, const char *r);`

[-0, +1, m]

Creates a copy of string s, replacing any occurrence of the string p with the string r. Pushes the resulting string on the stack and returns it.
创建字符串 `s` 的副本，将字符串 `p` 的任何出现替换为字符串 `r`。将结果字符串推入栈并返回它。

- **实现**（implementation）：内部使用 `luaL_Buffer` 与 `luaL_addgsub` 逐段构建结果字符串。Key source files: `lauxlib.c:luaL_gsub`。

### luaL_len

`lua_Integer luaL_len (lua_State *L, int index);`

[-0, +0, e]

Returns the "length" of the value at the given index as a number; it is equivalent to the # operator in Lua (see §3.4.7). Raises an error if the result of the operation is not an integer. (This case can only happen through metamethods.)
返回给定索引处值的"长度"作为数字；它等价于 Lua 中的 `#` 运算符（见 §3.4.7）。如果操作结果不是整数，则引发错误。（这种情况只能通过元方法发生。）

- **实现**（implementation）：调用 `lua_len` 触发元方法，再检查返回值是否为整数。Key source files: `lauxlib.c:luaL_len`。

### luaL_loadbuffer

`int luaL_loadbuffer (lua_State *L, const char *buff, size_t sz, const char *name);`

[-0, +1, -]

Equivalent to luaL_loadbufferx with mode equal to NULL.
等价于 `luaL_loadbufferx` 且 `mode` 等于 `NULL`。

- **实现**（implementation）：定义为宏，直接转发为 `luaL_loadbufferx(L, s, sz, n, NULL)`。Key source files: `lauxlib.h:luaL_loadbuffer`。

### luaL_loadbufferx

`int luaL_loadbufferx (lua_State *L, const char *buff, size_t sz, const char *name, const char *mode);`

[-0, +1, -]

Loads a buffer as a Lua chunk. This function uses lua_load to load the chunk in the buffer pointed to by buff with size sz.
将缓冲区加载为 Lua 代码块。此函数使用 `lua_load` 将代码块加载到 `buff` 指向的大小为 `sz` 的缓冲区中。

- **实现**（implementation）：通过 `LoadS` 结构包装缓冲区，并调用 `lua_load` 以 `getS` 作为 reader 完成加载。Key source files: `lauxlib.c:luaL_loadbufferx`。

This function returns the same results as lua_load. name is the chunk name, used for debug information and error messages. The string mode works as in the function lua_load.
此函数返回与 `lua_load` 相同的结果。`name` 是代码块名称，用于调试信息和错误消息。字符串 `mode` 的工作方式与 `lua_load` 函数相同。

### luaL_loadfile

`int luaL_loadfile (lua_State *L, const char *filename);`

[-0, +1, m]

Equivalent to luaL_loadfilex with mode equal to NULL.
等价于 `luaL_loadfilex` 且 `mode` 等于 `NULL`。

- **实现**（implementation）：定义为宏，直接转发为 `luaL_loadfilex(L, f, NULL)`。Key source files: `lauxlib.h:luaL_loadfile`。

### luaL_loadfilex

`int luaL_loadfilex (lua_State *L, const char *filename, const char *mode);`

[-0, +1, m]

Loads a file as a Lua chunk. This function uses lua_load to load the chunk in the file named filename. If filename is NULL, then it loads from the standard input. The first line in the file is ignored if it starts with a #.
将文件加载为 Lua 代码块。此函数使用 `lua_load` 将代码块加载到名为 `filename` 的文件中。如果 `filename` 为 `NULL`，则从标准输入加载。如果文件中的第一行以 `#` 开头，则忽略它。

- **实现**（implementation）：通过 `LoadF` 结构逐块读取文件，自动跳过 BOM 与首行 `#` 注释，最后调用 `lua_load` 完成编译。Key source files: `lauxlib.c:luaL_loadfilex`。

The string mode works as in the function lua_load.
字符串 `mode` 的工作方式与 `lua_load` 函数相同。

This function returns the same results as lua_load or LUA_ERRFILE for file-related errors.
此函数返回与 `lua_load` 相同的结果，对于与文件相关的错误返回 `LUA_ERRFILE`。

As lua_load, this function only loads the chunk; it does not run it.
与 `lua_load` 一样，此函数仅加载代码块；它不运行它。

### luaL_loadstring

`int luaL_loadstring (lua_State *L, const char *s);`

[-0, +1, -]

Loads a string as a Lua chunk. This function uses lua_load to load the chunk in the zero-terminated string s.
将字符串加载为 Lua 代码块。此函数使用 `lua_load` 将代码块加载到以零结尾的字符串 `s` 中。

- **实现**（implementation）：直接调用 `luaL_loadbuffer`，以 `strlen(s)` 计算长度。Key source files: `lauxlib.c:luaL_loadstring`。

This function returns the same results as lua_load.
此函数返回与 `lua_load` 相同的结果。

Also as lua_load, this function only loads the chunk; it does not run it.
与 `lua_load` 一样，此函数仅加载代码块；它不运行它。

### luaL_newlib

`void luaL_newlib (lua_State *L, const luaL_Reg l[]);`

[-0, +1, m]

Creates a new table and registers there the functions in the list l.
创建一个新表并在其中注册列表 `l` 中的函数。

- **实现**（implementation）：定义为宏，先调用 `luaL_newlibtable` 预分配合适大小的表，再调用 `luaL_setfuncs` 注册函数。Key source files: `lauxlib.h:luaL_newlib`。

It is implemented as the following macro:
它实现为以下宏：

```c
(luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))
```

The array l must be the actual array, not a pointer to it.
数组 `l` 必须是实际数组，而不是指向它的指针。

### luaL_newlibtable

`void luaL_newlibtable (lua_State *L, const luaL_Reg l[]);`

[-0, +1, m]

Creates a new table with a size optimized to store all entries in the array l (but does not actually store them). It is intended to be used in conjunction with luaL_setfuncs (see luaL_newlib).
创建一个大小优化的新表，以存储数组 `l` 中的所有条目（但实际上不存储它们）。它旨在与 `luaL_setfuncs` 一起使用（见 `luaL_newlib`）。

- **实现**（implementation）：定义为宏，根据数组元素个数预计算哈希表大小并调用 `lua_createtable`。Key source files: `lauxlib.h:luaL_newlibtable`。

It is implemented as a macro. The array l must be the actual array, not a pointer to it.
它实现为宏。数组 `l` 必须是实际数组，而不是指向它的指针。

### luaL_newmetatable

`int luaL_newmetatable (lua_State *L, const char *tname);`

[-0, +1, m]

If the registry already has the key tname, returns 0. Otherwise, creates a new table to be used as a metatable for userdata, adds to this new table the pair __name = tname, adds to the registry the pair [tname] = new table, and returns 1.
如果注册表已经有键 `tname`，则返回 0。否则，创建一个新表用作 userdata 的元表，向此新表添加对 `__name = tname`，向注册表添加对 `[tname] = new table`，并返回 1。

- **实现**（implementation）：先查询注册表判断是否存在，若不存在则创建表并设置 `__name` 字段后再写回注册表。Key source files: `lauxlib.c:luaL_newmetatable`。

In both cases, the function pushes onto the stack the final value associated with tname in the registry.
在这两种情况下，函数都将注册表中与 `tname` 关联的最终值推入栈。

### luaL_newstate

`lua_State *luaL_newstate (void);`

[-0, +0, -]

Creates a new Lua state. It calls lua_newstate with an allocator based on the ISO C allocation functions and then sets a warning function and a panic function (see §4.4) that print messages to the standard error output.
创建新的 Lua 状态。它使用基于 **ISO C** 分配函数的分配器调用 `lua_newstate`，然后设置警告函数和恐慌函数（见 §4.4），将消息打印到标准错误输出。

- **实现**（implementation）：内部使用标准 C 的 `realloc`/`free` 作为分配器（`l_alloc`），并设置默认的 panic 与警告回调。Key source files: `lauxlib.c:luaL_newstate`。

Returns the new state, or NULL if there is a memory allocation error.
返回新状态，如果存在内存分配错误，则返回 `NULL`。

### luaL_openlibs

`void luaL_openlibs (lua_State *L);`

[-0, +0, e]

Opens all standard Lua libraries into the given state.
将所有标准 Lua 库打开到给定状态中。

- **实现**（implementation）：遍历 `loadedlibs` 数组，对每个库调用 `luaL_requiref` 将其注册到全局表。Key source files: `linit.c:luaL_openlibs`。

### luaL_opt

`T luaL_opt (L, func, arg, dflt);`

[-0, +0, -]

This macro is defined as follows:
此宏定义如下：

```c
(lua_isnoneornil(L,(arg)) ? (dflt) : func(L,(arg)))
```

In words, if the argument arg is nil or absent, the macro results in the default dflt. Otherwise, it results in the result of calling func with the state L and the argument index arg as arguments. Note that it evaluates the expression dflt only if needed.
换句话说，如果参数 `arg` 为 `nil` 或缺失，则宏结果为默认值 `dflt`。否则，它结果是使用状态 `L` 和参数索引 `arg` 作为参数调用 `func` 的结果。请注意，它仅在需要时才计算表达式 `dflt`。

### luaL_optinteger

`lua_Integer luaL_optinteger (lua_State *L, int arg, lua_Integer d);`

[-0, +0, v]

If the function argument arg is an integer (or it is convertible to an integer), returns this integer. If this argument is absent or is nil, returns d. Otherwise, raises an error.
如果函数参数 `arg` 是整数（或可转换为整数），则返回此整数。如果此参数缺失或为 `nil`，则返回 `d`。否则，引发错误。

- **实现**（implementation）：利用 `luaL_opt` 宏，在参数不存在时返回默认值，否则调用 `luaL_checkinteger`。Key source files: `lauxlib.c:luaL_optinteger` / `lauxlib.h:luaL_opt`。

### luaL_optlstring

`const char *luaL_optlstring (lua_State *L, int arg, const char *d, size_t *l);`

[-0, +0, v]

If the function argument arg is a string, returns this string. If this argument is absent or is nil, returns d. Otherwise, raises an error.
如果函数参数 `arg` 是字符串，则返回此字符串。如果此参数缺失或为 `nil`，则返回 `d`。否则，引发错误。

- **实现**（implementation）：利用 `luaL_opt` 宏，在参数为 nil 或缺失时返回默认值并设置长度，否则调用 `luaL_checklstring`。Key source files: `lauxlib.c:luaL_optlstring` / `lauxlib.h:luaL_opt`。

If l is not NULL, fills its referent with the result's length. If the result is NULL (only possible when returning d and d == NULL), its length is considered zero.
如果 `l` 不为 `NULL`，则用结果的长度填充其引用。如果结果为 `NULL`（仅在返回 `d` 且 `d == NULL` 时可能），则其长度被视为零。

This function uses lua_tolstring to get its result, so all conversions and caveats of that function apply here.
此函数使用 `lua_tolstring` 来获取其结果，因此该函数的所有转换和注意事项都适用于此处。

### luaL_optnumber

`lua_Number luaL_optnumber (lua_State *L, int arg, lua_Number d);`

[-0, +0, v]

If the function argument arg is a number, returns this number as a lua_Number. If this argument is absent or is nil, returns d. Otherwise, raises an error.
如果函数参数 `arg` 是数字，则将其作为 `lua_Number` 返回。如果此参数缺失或为 `nil`，则返回 `d`。否则，引发错误。

- **实现**（implementation）：利用 `luaL_opt` 宏，在参数为 nil 或缺失时返回默认值，否则调用 `luaL_checknumber`。Key source files: `lauxlib.c:luaL_optnumber` / `lauxlib.h:luaL_opt`。

### luaL_optstring

`const char *luaL_optstring (lua_State *L, int arg, const char *d);`

[-0, +0, v]

If the function argument arg is a string, returns this string. If this argument is absent or is nil, returns d. Otherwise, raises an error.
如果函数参数 `arg` 是字符串，则返回此字符串。如果此参数缺失或为 `nil`，则返回 `d`。否则，引发错误。

- **实现**（implementation）：定义为宏，等价于 `luaL_optlstring(L, n, d, NULL)`。Key source files: `lauxlib.h:luaL_optstring`。

### luaL_prepbuffer

`char *luaL_prepbuffer (luaL_Buffer *B);`

[-?, +?, m]

Equivalent to luaL_prepbuffsize with the predefined size LUAL_BUFFERSIZE.
等价于 `luaL_prepbuffsize` 且预定义大小为 `LUAL_BUFFERSIZE`。

- **实现**（implementation）：定义为宏，直接调用 `luaL_prepbuffsize(B, LUAL_BUFFERSIZE)`。Key source files: `lauxlib.h:luaL_prepbuffer`。

### luaL_prepbuffsize

`char *luaL_prepbuffsize (luaL_Buffer *B, size_t sz);`

[-?, +?, m]

Returns an address to a space of size sz where you can copy a string to be added to buffer B (see luaL_Buffer). After copying the string into this space you must call luaL_addsize with the size of the string to actually add it to the buffer.
返回大小为 `sz` 的空间的地址，你可以在其中复制要添加到缓冲区 `B` 的字符串（见 `luaL_Buffer`）。将字符串复制到此空间后，你必须调用 `luaL_addsize` 并传入字符串的大小，以实际将其添加到缓冲区。

- **实现**（implementation）：若内联空间不足，则创建或扩容 userdata box（`_UBOX*`），按 1.5 倍增长并复制旧数据。Key source files: `lauxlib.c:luaL_prepbuffsize`。

### luaL_pushfail

`void luaL_pushfail (lua_State *L);`

[-0, +1, -]

Pushes the fail value onto the stack (see §6).
将 `fail` 值推入栈（见 §6）。

- **实现**（implementation）：定义为宏，当前等价于 `lua_pushnil(L)`。Key source files: `lauxlib.h:luaL_pushfail`。

### luaL_pushresult

`void luaL_pushresult (luaL_Buffer *B);`

[-?, +1, m]

Finishes the use of buffer B leaving the final string on the top of the stack.
完成缓冲区 `B` 的使用，将最终字符串留在栈顶。

- **实现**（implementation）：调用 `lua_pushlstring` 将缓冲区内容转为 Lua 字符串，并清理栈上的 box 或占位符。Key source files: `lauxlib.c:luaL_pushresult`。

### luaL_pushresultsize

`void luaL_pushresultsize (luaL_Buffer *B, size_t sz);`

[-?, +1, m]

Equivalent to the sequence luaL_addsize, luaL_pushresult.
等价于序列 `luaL_addsize`、`luaL_pushresult`。

- **实现**（implementation）：先通过 `luaL_addsize` 更新长度，再调用 `luaL_pushresult` 生成最终字符串。Key source files: `lauxlib.c:luaL_pushresultsize`。

### luaL_ref

`int luaL_ref (lua_State *L, int t);`

[-1, +0, m]

Creates and returns a reference, in the table at index t, for the object on the top of the stack (and pops the object).
在索引 `t` 处的表中为栈顶的对象创建并返回**引用**（并弹出该对象）。

- **实现**（implementation）：使用注册表中的空闲链表（freelist）复用整数键，首次访问时初始化链表头。Key source files: `lauxlib.c:luaL_ref`。

A reference is a unique integer key. As long as you do not manually add integer keys into the table t, luaL_ref ensures the uniqueness of the key it returns. You can retrieve an object referred by the reference r by calling lua_rawgeti(L, t, r). The function luaL_unref frees a reference.
引用是唯一整数键。只要你没有手动将整数键添加到表 `t` 中，`luaL_ref` 就确保其返回的键的唯一性。你可以通过调用 `lua_rawgeti(L, t, r)` 来检索引用 `r` 所引用的对象。`luaL_unref` 函数释放引用。

If the object on the top of the stack is nil, luaL_ref returns the constant LUA_REFNIL. The constant LUA_NOREF is guaranteed to be different from any reference returned by luaL_ref.
如果栈顶的对象是 `nil`，则 `luaL_ref` 返回常量 `LUA_REFNIL`。常量 `LUA_NOREF` 保证与 `luaL_ref` 返回的任何引用不同。

### luaL_Reg

```c
typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;
```

Type for arrays of functions to be registered by luaL_setfuncs. name is the function name and func is a pointer to the function. Any array of luaL_Reg must end with a sentinel entry in which both name and func are NULL.
要由 `luaL_setfuncs` 注册的函数数组的类型。`name` 是函数名称，`func` 是指向函数的指针。任何 `luaL_Reg` 数组都必须以哨兵条目结尾，其中 `name` 和 `func` 均为 `NULL`。

- **实现**（implementation）：结构体仅含 `name` 与 `func` 两个字段，`luaL_setfuncs` 通过遍历至 `NULL` 哨兵来注册所有函数。Key source files: `lauxlib.h:luaL_Reg`。

### luaL_requiref

`void luaL_requiref (lua_State *L, const char *modname, lua_CFunction openf, int glb);`

[-0, +1, e]

If package.loaded[modname] is not true, calls the function openf with the string modname as an argument and sets the call result to package.loaded[modname], as if that function has been called through require.
如果 `package.loaded[modname]` 不为 true，则使用字符串 `modname` 作为参数调用函数 `openf`，并将调用结果设置为 `package.loaded[modname]`，就好像该函数已通过 `require` 调用一样。

- **实现**（implementation）：先查询 `_LOADED` 表，若未加载则调用 `openf`，将结果存入 `_LOADED` 并在 `glb` 为 true 时写入全局变量。Key source files: `lauxlib.c:luaL_requiref`。

If glb is true, also stores the module into the global modname.
如果 `glb` 为 true，还将模块存储到全局变量 `modname` 中。

Leaves a copy of the module on the stack.
在栈上留下模块的副本。

### luaL_setfuncs

`void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);`

[-nup, +0, m]

Registers all functions in the array l (see luaL_Reg) into the table on the top of the stack (below optional upvalues, see next).
将数组 `l` 中的所有函数（见 `luaL_Reg`）注册到栈顶的表中（在可选的上值下方，见下文）。

- **实现**（implementation）：遍历 `luaL_Reg` 数组，对每个条目复制上值并创建 `lua_CClosure`，然后用 `lua_setfield` 写入表中。Key source files: `lauxlib.c:luaL_setfuncs`。

When nup is not zero, all functions are created with nup upvalues, initialized with copies of the nup values previously pushed on the stack on top of the library table. These values are popped from the stack after the registration.
当 `nup` 不为零时，所有函数都使用 `nup` 个上值创建，使用先前推入库表上方栈的 `nup` 个值的副本初始化。这些值在注册后从栈中弹出。

A function with a NULL value represents a placeholder, which is filled with false.
值为 `NULL` 的函数表示占位符，用 `false` 填充。

### luaL_setmetatable

`void luaL_setmetatable (lua_State *L, const char *tname);`

[-0, +0, -]

Sets the metatable of the object on the top of the stack as the metatable associated with name tname in the registry (see luaL_newmetatable).
将栈顶对象的元表设置为注册表中与名称 `tname` 关联的元表（见 `luaL_newmetatable`）。

- **实现**（implementation）：先从注册表获取元表，再对栈顶对象调用 `lua_setmetatable`。Key source files: `lauxlib.c:luaL_setmetatable`。

### luaL_Stream

```c
typedef struct luaL_Stream {
  FILE *f;
  lua_CFunction closef;
} luaL_Stream;
```

The standard representation for file handles used by the standard I/O library.
标准 I/O 库使用的**文件句柄**的标准表示。

- **实现**（implementation）：文件句柄以该结构体开头作为完整 userdata，元表名固定为宏 `LUA_FILEHANDLE`（即 `"FILE*"`），`closef` 负责在关闭或 GC 时释放底层 `FILE*`。Key source files: `lauxlib.h:luaL_Stream` / `liolib.c`。

A file handle is implemented as a full userdata, with a metatable called LUA_FILEHANDLE (where LUA_FILEHANDLE is a macro with the actual metatable's name). The metatable is created by the I/O library (see luaL_newmetatable).
文件句柄实现为完整的 userdata，具有名为 `LUA_FILEHANDLE` 的元表（其中 `LUA_FILEHANDLE` 是具有实际元表名称的宏）。元表由 I/O 库创建（见 `luaL_newmetatable`）。

This userdata must start with the structure luaL_Stream; it can contain other data after this initial structure. The field f points to the corresponding C stream (or it can be NULL to indicate an incompletely created handle). The field closef points to a Lua function that will be called to close the stream when the handle is closed or collected; this function receives the file handle as its sole argument and must return either a true value, in case of success, or a false value plus an error message, in case of error. Once Lua calls this field, it changes the field value to NULL to signal that the handle is closed.
此 userdata 必须以结构 `luaL_Stream` 开头；它可以在初始结构之后包含其他数据。字段 `f` 指向相应的 **C 流**（或者可以为 `NULL` 以指示未完全创建的句柄）。字段 `closef` 指向当句柄关闭或收集时将调用以关闭流的 Lua 函数；此函数接收文件句柄作为其唯一参数，并且必须在成功时返回 true 值，或在出错时返回 false 值加错误消息。一旦 Lua 调用此字段，它将字段值更改为 `NULL` 以表示句柄已关闭。

### luaL_testudata

`void *luaL_testudata (lua_State *L, int arg, const char *tname);`

[-0, +0, m]

This function works like luaL_checkudata, except that, when the test fails, it returns NULL instead of raising an error.
此函数的工作方式类似于 `luaL_checkudata`，不同之处在于，当测试失败时，它返回 `NULL` 而不是引发错误。

- **实现**（implementation）：依次检查 userdata 指针、元表是否存在，并与注册表中 `tname` 对应的元表进行 `lua_rawequal` 比较。Key source files: `lauxlib.c:luaL_testudata`。

### luaL_tolstring

`const char *luaL_tolstring (lua_State *L, int idx, size_t *len);`

[-0, +1, e]

Converts any Lua value at the given index to a C string in a reasonable format. The resulting string is pushed onto the stack and also returned by the function (see §4.1.3). If len is not NULL, the function also sets *len with the string length.
将给定索引处的任何 Lua 值以合理的格式转换为 **C 字符串**。结果字符串被推入栈，也由函数返回（见 §4.1.3）。如果 `len` 不为 `NULL`，则函数还用字符串长度设置 `*len`。

- **实现**（implementation）：优先调用 `__tostring` 元方法，否则根据类型（数字、布尔、nil 等）分别格式化，对 userdata/table 使用 `__name` 或类型名加地址。Key source files: `lauxlib.c:luaL_tolstring`。

If the value has a metatable with a __tostring field, then luaL_tolstring calls the corresponding metamethod with the value as argument, and uses the result of the call as its result.
如果值具有带有 `__tostring` 字段的元表，则 `luaL_tolstring` 使用值作为参数调用相应的元方法，并使用调用的结果作为其结果。

### luaL_traceback

`void luaL_traceback (lua_State *L, lua_State *L1, const char *msg, int level);`

[-0, +1, m]

Creates and pushes a traceback of the stack L1. If msg is not NULL, it is appended at the beginning of the traceback. The level parameter tells at which level to start the traceback.
创建并推送栈 `L1` 的回溯。如果 `msg` 不为 `NULL`，则将其附加到回溯的开头。`level` 参数指示从哪个级别开始回溯。

- **实现**（implementation）：通过 `luaL_Buffer` 拼接回溯信息，使用 `lua_getstack` 与 `lua_getinfo` 遍历调用栈，并处理尾调用与跳过过多层级的提示。Key source files: `lauxlib.c:luaL_traceback`。

### luaL_typeerror

`int luaL_typeerror (lua_State *L, int arg, const char *tname);`

[-0, +0, v]

Raises a type error for the argument arg of the C function that called it, using a standard message; tname is a "name" for the expected type. This function never returns.
为调用它的 **C 函数**的参数 `arg` 引发类型错误，使用标准消息；`tname` 是预期类型的"名称"。此函数永远不会返回。

- **实现**（implementation）：先尝试 `__name` 元字段获取实际类型名，构造 "expected, got" 消息后调用 `luaL_argerror`。Key source files: `lauxlib.c:luaL_typeerror`。

### luaL_typename

`const char *luaL_typename (lua_State *L, int index);`

[-0, +0, -]

Returns the name of the type of the value at the given index.
返回给定索引处值类型的名称。

- **实现**（implementation）：定义为宏，先取 `lua_type` 再调用 `lua_typename` 获取类型名字符串。Key source files: `lauxlib.h:luaL_typename`。

### luaL_unref

`void luaL_unref (lua_State *L, int t, int ref);`

[-0, +0, -]

Releases the reference ref from the table at index t (see luaL_ref). The entry is removed from the table, so that the referred object can be collected. The reference ref is also freed to be used again.
从索引 `t` 处的表中释放引用 `ref`（见 `luaL_ref`）。条目从表中移除，以便可以收集被引用的对象。引用 `ref` 也被释放以供再次使用。

- **实现**（implementation）：将释放的引用插入空闲链表头部（`t[freelist]`），实现整数键的复用。Key source files: `lauxlib.c:luaL_unref`。

If ref is LUA_NOREF or LUA_REFNIL, luaL_unref does nothing.
如果 `ref` 是 `LUA_NOREF` 或 `LUA_REFNIL`，则 `luaL_unref` 不执行任何操作。

### luaL_where

`void luaL_where (lua_State *L, int lvl);`

[-0, +1, m]

Pushes onto the stack a string identifying the current position of the control at level lvl in the call stack. Typically this string has the following format:
将字符串推入栈，标识调用栈中级别 `lvl` 处控制的当前位置。通常，此字符串具有以下格式：

- **实现**（implementation）：通过 `lua_getstack` 与 `lua_getinfo("Sl")` 获取源码文件名与行号，再用 `lua_pushfstring` 格式化为 `"file:line: "`。Key source files: `lauxlib.c:luaL_where`。

```
chunkname:currentline:
```

Level 0 is the running function, level 1 is the function that called the running function, etc.
级别 0 是正在运行的函数，级别 1 是调用正在运行的函数的函数，依此类推。

This function is used to build a prefix for error messages.
此函数用于构建错误消息的前缀。


---

# 6 – The Standard Libraries（标准库）

The standard Lua libraries provide useful functions that are implemented in C through the C API. Some of these functions provide essential services to the language (e.g., type and getmetatable); others provide access to outside services (e.g., I/O); and others could be implemented in Lua itself, but that for different reasons deserve an implementation in C (e.g., table.sort).
标准 Lua 库提供了通过 **C API** 用 **C** 实现的有用函数。其中一些函数为语言提供基本服务（例如 `type` 和 `getmetatable`）；另一些提供对外部服务的访问（例如 I/O）；还有一些本可以在 Lua 本身中实现，但由于不同原因值得在 C 中实现（例如 `table.sort`）。

All libraries are implemented through the official C API and are provided as separate C modules. Unless otherwise noted, these library functions do not adjust its number of arguments to its expected parameters. For instance, a function documented as foo(arg) should not be called without an argument.
所有库都通过官方 **C API** 实现，并作为单独的 **C 模块**提供。除非另有说明，否则这些库函数不会将其参数数量调整到预期的参数。例如，文档中记载为 `foo(arg)` 的函数不应在没有参数的情况下调用。

The notation fail means a false value representing some kind of failure. (Currently, fail is equal to nil, but that may change in future versions. The recommendation is to always test the success of these functions with (not status), instead of (status == nil).)
符号 `fail` 表示表示某种失败的假值。（目前，`fail` 等于 `nil`，但这可能在将来的版本中更改。建议始终使用 `(not status)` 测试这些函数的成功，而不是 `(status == nil)`。）

Currently, Lua has the following standard libraries:
目前，Lua 具有以下标准库：

- basic library（基本库）
- coroutine library（协程库）
- package library（包库）
- string manipulation（字符串操作）
- basic UTF-8 support（基本 UTF-8 支持）
- table manipulation（表操作）
- mathematical functions（数学函数）（sin、log 等）
- input and output（输入和输出）
- operating system facilities（操作系统工具）
- debug facilities（调试工具）

Except for the basic and the package libraries, each library provides all its functions as fields of a global table or as methods of its objects.
除了基本库和包库之外，每个库都将其所有函数作为全局表的字段或其对象的方法提供。

To have access to these libraries, the C host program should call the luaL_openlibs function, which opens all standard libraries. Alternatively, the host program can open them individually by using luaL_requiref to call luaopen_base (for the basic library), luaopen_package (for the package library), luaopen_coroutine (for the coroutine library), luaopen_string (for the string library), luaopen_utf8 (for the UTF-8 library), luaopen_table (for the table library), luaopen_math (for the mathematical library), luaopen_io (for the I/O library), luaopen_os (for the operating system library), and luaopen_debug (for the debug library). These functions are declared in lualib.h.
要访问这些库，**C 宿主**程序应调用 `luaL_openlibs` 函数，该函数打开所有标准库。或者，宿主程序可以使用 `luaL_requiref` 单独打开它们，以调用 `luaopen_base`（基本库）、`luaopen_package`（包库）、`luaopen_coroutine`（协程库）、`luaopen_string`（字符串库）、`luaopen_utf8`（UTF-8 库）、`luaopen_table`（表库）、`luaopen_math`（数学库）、`luaopen_io`（I/O 库）、`luaopen_os`（操作系统库）和 `luaopen_debug`（调试库）。这些函数在 `lualib.h` 中声明。

## 6.1 – Basic Functions（基本函数）

The basic library provides core functions to Lua. If you do not include this library in your application, you should check carefully whether you need to provide implementations for some of its facilities.
基本库为 Lua 提供核心函数。如果你不在应用程序中包含此库，则应仔细检查是否需要为其某些工具提供实现。

### assert (v [, message])

Raises an error if the value of its argument v is false (i.e., nil or false); otherwise, returns all its arguments. In case of error, message is the error object; when absent, it defaults to "assertion failed!"
如果其参数 `v` 的值为假（即 `nil` 或 `false`），则引发错误；否则，返回其所有参数。在出错的情况下，`message` 是错误对象；当缺失时，默认为 `"assertion failed!"`。

### collectgarbage ([opt [, arg]])

This function is a generic interface to the garbage collector. It performs different functions according to its first argument, opt:
此函数是垃圾回收器的通用接口。它根据其第一个参数 `opt` 执行不同的功能：

- **"collect"**：执行完整的垃圾回收周期。这是默认选项。
- **"stop"**：停止垃圾回收器的自动执行。回收器仅在显式调用时运行，直到调用重新启动它。
- **"restart"**：重新启动垃圾回收器的自动执行。
- **"count"**：返回 Lua 使用的总内存（以 Kbytes 为单位）。该值具有小数部分，因此乘以 1024 可以得到 Lua 使用的精确字节数。
- **"step"**：执行垃圾回收步骤。步骤"大小"由 `arg` 控制。使用零值，回收器将执行一个基本的（不可分的）步骤。对于非零值，回收器将执行好像 Lua 分配了那么多内存（以 Kbytes 为单位）。如果步骤完成了一个收集周期，则返回 `true`。
- **"isrunning"**：返回一个布尔值，指示回收器是否正在运行（即未停止）。
- **"incremental"**：将回收器模式更改为增量模式。此选项后面可以跟三个数字：垃圾回收器暂停、步骤乘数和步骤大小（见 §2.5.1）。零表示不更改该值。
- **"generational"**：将回收器模式更改为分代模式。此选项后面可以跟两个数字：垃圾回收器次要乘数和主要乘数（见 §2.5.2）。零表示不更改该值。

See §2.5 for more details about garbage collection and some of these options.
有关垃圾回收和其中一些选项的更多详细信息，请参见 §2.5。

This function should not be called by a finalizer.
终结器不应调用此函数。

- **实现**（implementation）：该函数是 `lua_gc` API 的封装，根据选项字符串调用不同模式的垃圾回收操作。Key source files: `lbaselib.c:luaB_collectgarbage`。

### dofile ([filename])

Opens the named file and executes its content as a Lua chunk. When called without arguments, dofile executes the content of the standard input (stdin). Returns all values returned by the chunk. In case of errors, dofile propagates the error to its caller. (That is, dofile does not run in protected mode.)
打开命名文件并将其内容作为 Lua 代码块执行。在没有参数的情况下调用时，`dofile` 执行标准输入（`stdin`）的内容。返回代码块返回的所有值。在出错的情况下，`dofile` 将错误传播给其调用者。（也就是说，`dofile` 不在受保护模式下运行。）

- **实现**（implementation）：先调用 `luaL_loadfile` 加载文件，再通过 `lua_callk` 执行代码块并返回所有结果。Key source files: `lbaselib.c:luaB_dofile`。

### error (message [, level])

Raises an error (see §2.3) with message as the error object. This function never returns.
使用 `message` 作为错误对象引发错误（见 §2.3）。此函数永远不会返回。

Usually, error adds some information about the error position at the beginning of the message, if the message is a string. The level argument specifies how to get the error position. With level 1 (the default), the error position is where the error function was called. Level 2 points the error to where the function that called error was called; and so on. Passing a level 0 avoids the addition of error position information to the message.
通常，如果消息是字符串，`error` 会在消息开头添加有关错误位置的一些信息。`level` 参数指定如何获取错误位置。使用级别 1（默认值），错误位置是调用 `error` 函数的位置。级别 2 将错误指向调用 `error` 的函数被调用的位置；依此类推。传递级别 0 可以避免将错误位置信息添加到消息中。

### _G

A global variable (not a function) that holds the global environment (see §2.2). Lua itself does not use this variable; changing its value does not affect any environment, nor vice versa.
一个全局变量（不是函数），保存全局环境（见 §2.2）。Lua 本身不使用此变量；更改其值不会影响任何环境，反之亦然。

### getmetatable (object)

If object does not have a metatable, returns nil. Otherwise, if the object's metatable has a __metatable field, returns the associated value. Otherwise, returns the metatable of the given object.
如果 `object` 没有元表，则返回 `nil`。否则，如果对象的元表具有 `__metatable` 字段，则返回关联的值。否则，返回给定对象的元表。

### ipairs (t)

Returns three values (an iterator function, the table t, and 0) so that the construction
返回三个值（迭代器函数、表 `t` 和 0），以便构造

```lua
for i,v in ipairs(t) do body end
```

will iterate over the key-value pairs (1,t[1]), (2,t[2]), ..., up to the first absent index.
将遍历键值对 `(1,t[1])`、`(2,t[2])`、...，直到第一个缺失的索引。

- **实现**（implementation）：返回迭代器函数 `ipairsaux`、表 `t` 和初始值 0，由 `ipairsaux` 通过 `lua_geti` 逐次读取序列元素。Key source files: `lbaselib.c:luaB_ipairs`、`lbaselib.c:ipairsaux`。

### load (chunk [, chunkname [, mode [, env]]])

Loads a chunk.
加载代码块。

If chunk is a string, the chunk is this string. If chunk is a function, load calls it repeatedly to get the chunk pieces. Each call to chunk must return a string that concatenates with previous results. A return of an empty string, nil, or no value signals the end of the chunk.
如果 `chunk` 是字符串，则代码块就是此字符串。如果 `chunk` 是函数，则 `load` 重复调用它以获取代码块片段。每次对 `chunk` 的调用都必须返回一个与前一个结果连接的字符串。返回空字符串、`nil` 或无值表示代码块结束。

If there are no syntactic errors, load returns the compiled chunk as a function; otherwise, it returns fail plus the error message.
如果没有语法错误，`load` 将编译后的代码块作为函数返回；否则，它返回 `fail` 加错误消息。

When you load a main chunk, the resulting function will always have exactly one upvalue, the _ENV variable (see §2.2). However, when you load a binary chunk created from a function (see string.dump), the resulting function can have an arbitrary number of upvalues, and there is no guarantee that its first upvalue will be the _ENV variable. (A non-main function may not even have an _ENV upvalue.)
当你加载主代码块时，结果函数将始终只有一个上值，即 `_ENV` 变量（见 §2.2）。然而，当你加载从函数创建的二进制代码块时（见 `string.dump`），结果函数可以有任意数量的上值，并且不能保证其第一个上值将是 `_ENV` 变量。（非主函数甚至可能没有 `_ENV` 上值。）

Regardless, if the resulting function has any upvalues, its first upvalue is set to the value of env, if that parameter is given, or to the value of the global environment. Other upvalues are initialized with nil. All upvalues are fresh, that is, they are not shared with any other function.
无论如何，如果结果函数有任何上值，如果给定了 `env` 参数，则其第一个上值设置为 `env` 的值，否则设置为全局环境的值。其他上值用 `nil` 初始化。所有上值都是新的，也就是说，它们不与任何其他函数共享。

chunkname is used as the name of the chunk for error messages and debug information (see §4.7). When absent, it defaults to chunk, if chunk is a string, or to "=(load)" otherwise.
`chunkname` 用作错误消息和调试信息的代码块名称（见 §4.7）。当缺失时，如果 `chunk` 是字符串，则默认为 `chunk`，否则默认为 `"=(load)"`。

The string mode controls whether the chunk can be text or binary (that is, a precompiled chunk). It may be the string "b" (only binary chunks), "t" (only text chunks), or "bt" (both binary and text). The default is "bt".
字符串 `mode` 控制代码块可以是文本还是二进制（即预编译的代码块）。它可以是字符串 `"b"`（仅二进制代码块）、`"t"`（仅文本代码块）或 `"bt"`（二进制和文本）。默认值为 `"bt"`。

It is safe to load malformed binary chunks; load signals an appropriate error. However, Lua does not check the consistency of the code inside binary chunks; running maliciously crafted bytecode can crash the interpreter.
加载格式错误的二进制代码块是安全的；`load` 会发出适当的错误信号。然而，Lua 不检查二进制代码块内部代码的一致性；运行恶意构造的字节码可能会使解释器崩溃。

- **实现**（implementation）：若为字符串则调用 `luaL_loadbufferx`，若为函数则通过 `generic_reader` 回调配合 `lua_load` 逐段读取。Key source files: `lbaselib.c:luaB_load`、`lbaselib.c:generic_reader`。

### loadfile ([filename [, mode [, env]]])

Similar to load, but gets the chunk from file filename or from the standard input, if no file name is given.
类似于 `load`，但如果没有给出文件名，则从文件 `filename` 或标准输入获取代码块。

### next (table [, index])

Allows a program to traverse all fields of a table. Its first argument is a table and its second argument is an index in this table. A call to next returns the next index of the table and its associated value. When called with nil as its second argument, next returns an initial index and its associated value. When called with the last index, or with nil in an empty table, next returns nil. If the second argument is absent, then it is interpreted as nil. In particular, you can use next(t) to check whether a table is empty.
允许程序遍历表的所有字段。其第一个参数是表，第二个参数是此表中的索引。对 `next` 的调用返回表的下一个索引及其关联的值。当使用 `nil` 作为其第二个参数调用时，`next` 返回初始索引及其关联的值。当使用最后一个索引调用时，或在空表中使用 `nil` 调用时，`next` 返回 `nil`。如果第二个参数缺失，则将其解释为 `nil`。特别地，你可以使用 `next(t)` 来检查表是否为空。

The order in which the indices are enumerated is not specified, even for numeric indices. (To traverse a table in numerical order, use a numerical for.)
枚举索引的顺序是未指定的，即使对于数字索引也是如此。（要按数字顺序遍历表，请使用数字 `for`。）

You should not assign any value to a non-existent field in a table during its traversal. You may however modify existing fields. In particular, you may set existing fields to nil.
在遍历期间，你不应为表中的不存在的字段赋值。但是，你可以修改现有字段。特别地，你可以将现有字段设置为 `nil`。

- **实现**（implementation）：直接调用底层 `lua_next` 遍历表的键值对，参数缺失时自动补 `nil`。Key source files: `lbaselib.c:luaB_next`。

### pairs (t)

If t has a metamethod __pairs, calls it with t as argument and returns the first three results from the call.
如果 `t` 具有元方法 `__pairs`，则使用 `t` 作为参数调用它，并返回调用的前三个结果。

Otherwise, returns three values: the next function, the table t, and nil, so that the construction
否则，返回三个值：`next` 函数、表 `t` 和 `nil`，以便构造

```lua
for k,v in pairs(t) do body end
```

will iterate over all key-value pairs of table t.
将遍历表 `t` 的所有键值对。

See function next for the caveats of modifying the table during its traversal.
有关在遍历期间修改表的注意事项，请参见 `next` 函数。

- **实现**（implementation）：若存在 `__pairs` 元方法则调用它，否则返回 `luaB_next`、表和 `nil`。Key source files: `lbaselib.c:luaB_pairs`。

### pcall (f [, arg1, ...])

Calls the function f with the given arguments in protected mode. This means that any error inside f is not propagated; instead, pcall catches the error and returns a status code. Its first result is the status code (a boolean), which is true if the call succeeds without errors. In such case, pcall also returns all results from the call, after this first result. In case of any error, pcall returns false plus the error object. Note that errors caught by pcall do not call a message handler.
使用给定参数在受保护模式下调用函数 `f`。这意味着 `f` 内部的任何错误都不会传播；相反，`pcall` 捕获错误并返回状态码。其第一个结果是状态码（布尔值），如果调用成功且没有错误，则为 `true`。在这种情况下，`pcall` 还会返回调用的所有结果，在此第一个结果之后。在出现任何错误的情况下，`pcall` 返回 `false` 加错误对象。请注意，`pcall` 捕获的错误不会调用消息处理程序。

- **实现**（implementation）：先压入 `true` 作为占位结果，再通过 `lua_pcallk` 进行保护调用，由 `finishpcall` 整理返回值。Key source files: `lbaselib.c:luaB_pcall`、`lbaselib.c:finishpcall`。

### print (...)

Receives any number of arguments and prints their values to stdout, converting each argument to a string following the same rules of tostring.
接收任意数量的参数，并将它们的值打印到 `stdout`，按照与 `tostring` 相同的规则将每个参数转换为字符串。

The function print is not intended for formatted output, but only as a quick way to show a value, for instance for debugging. For complete control over the output, use string.format and io.write.
`print` 函数不用于格式化输出，而只是作为显示值的快速方式，例如用于调试。要完全控制输出，请使用 `string.format` 和 `io.write`。

### rawequal (v1, v2)

Checks whether v1 is equal to v2, without invoking the __eq metamethod. Returns a boolean.
检查 `v1` 是否等于 `v2`，而不调用 `__eq` 元方法。返回布尔值。

### rawget (table, index)

Gets the real value of table[index], without using the __index metavalue. table must be a table; index may be any value.
获取 `table[index]` 的真实值，而不使用 `__index` 元值。`table` 必须是表；`index` 可以是任何值。

### rawlen (v)

Returns the length of the object v, which must be a table or a string, without invoking the __len metamethod. Returns an integer.
返回对象 `v` 的长度，它必须是表或字符串，而不调用 `__len` 元方法。返回整数。

### rawset (table, index, value)

Sets the real value of table[index] to value, without using the __newindex metavalue. table must be a table, index any value different from nil and NaN, and value any Lua value.
将 `table[index]` 的真实值设置为 `value`，而不使用 `__newindex` 元值。`table` 必须是表，`index` 是任何不同于 `nil` 和 **NaN** 的值，`value` 是任何 Lua 值。

This function returns table.
此函数返回 `table`。

### select (index, ...)

If index is a number, returns all arguments after argument number index; a negative number indexes from the end (-1 is the last argument). Otherwise, index must be the string "#", and select returns the total number of extra arguments it received.
如果 `index` 是数字，则返回参数编号 `index` 之后的所有参数；负数从末尾索引（`-1` 是最后一个参数）。否则，`index` 必须是字符串 `"#"`，`select` 返回它接收的额外参数的总数。

### setmetatable (table, metatable)

Sets the metatable for the given table. If metatable is nil, removes the metatable of the given table. If the original metatable has a __metatable field, raises an error.
为给定表设置元表。如果 `metatable` 为 `nil`，则移除给定表的元表。如果原始元表具有 `__metatable` 字段，则引发错误。

This function returns table.
此函数返回 `table`。

To change the metatable of other types from Lua code, you must use the debug library.
要从 Lua 代码更改其他类型的元表，你必须使用调试库。

### tonumber (e [, base])

When called with no base, tonumber tries to convert its argument to a number. If the argument is already a number or a string convertible to a number, then tonumber returns this number; otherwise, it returns fail.
在没有 `base` 的情况下调用时，`tonumber` 尝试将其参数转换为数字。如果参数已经是数字或可转换为数字的字符串，则 `tonumber` 返回此数字；否则，它返回 `fail`。

The conversion of strings can result in integers or floats, according to the lexical conventions of Lua (see §3.1). The string may have leading and trailing spaces and a sign.
根据 Lua 的词法约定（见 §3.1），字符串的转换可以产生整数或浮点数。字符串可能有前导和尾随空格以及符号。

When called with base, then e must be a string to be interpreted as an integer numeral in that base. The base may be any integer between 2 and 36, inclusive. In bases above 10, the letter A (in either upper or lower case) represents 10, B represents 11, and so forth, with Z representing 35. If the string e is not a valid numeral in the given base, the function returns fail.
使用 `base` 调用时，`e` 必须是字符串，以解释为该进制中的整数数字。进制可以是 2 到 36 之间的任何整数（含）。在高于 10 的进制中，字母 `A`（大写或小写）表示 10，`B` 表示 11，依此类推，`Z` 表示 35。如果字符串 `e` 不是给定进制中的有效数字，则函数返回 `fail`。

- **实现**（implementation）：无进制参数时调用 `lua_stringtonumber` 进行转换；有进制时通过自定义的 `b_str2int` 解析 2~36 进制整数。Key source files: `lbaselib.c:luaB_tonumber`、`lbaselib.c:b_str2int`。

### tostring (v)

Receives a value of any type and converts it to a string in a human-readable format.
接收任何类型的值并将其转换为人类可读的格式的字符串。

If the metatable of v has a __tostring field, then tostring calls the corresponding value with v as argument, and uses the result of the call as its result. Otherwise, if the metatable of v has a __name field with a string value, tostring may use that string in its final result.
如果 `v` 的元表具有 `__tostring` 字段，则 `tostring` 使用 `v` 作为参数调用相应的值，并使用调用的结果作为其结果。否则，如果 `v` 的元表具有字符串值的 `__name` 字段，`tostring` 可能会在其最终结果中使用该字符串。

For complete control of how numbers are converted, use string.format.
要完全控制数字的转换方式，请使用 `string.format`。

- **实现**（implementation）：调用 `luaL_tolstring` 完成转换，若值有 `__tostring` 元方法则优先调用。Key source files: `lbaselib.c:luaB_tostring`。

### type (v)

Returns the type of its only argument, coded as a string. The possible results of this function are "nil" (a string, not the value nil), "number", "string", "boolean", "table", "function", "thread", and "userdata".
返回其唯一参数的类型，编码为字符串。此函数的可能结果是 `"nil"`（字符串，不是值 `nil`）、`"number"`、`"string"`、`"boolean"`、`"table"`、`"function"`、`"thread"` 和 `"userdata"`。

### _VERSION

A global variable (not a function) that holds a string containing the running Lua version. The current value of this variable is "Lua 5.4".
一个全局变量（不是函数），保存包含正在运行的 Lua 版本的字符串。此变量的当前值为 `"Lua 5.4"`。

### warn (msg1, ...)

Emits a warning with a message composed by the concatenation of all its arguments (which should be strings).
使用由所有参数（应为字符串）连接组成的消息发出警告。

By convention, a one-piece message starting with @ is intended to be a control message, which is a message to the warning system itself. In particular, the standard warning function in Lua recognizes the control messages "@off", to stop the emission of warnings, and "@on", to (re)start the emission; it ignores unknown control messages.
按照惯例，以 `@` 开头的单条消息旨在作为**控制消息**，即发送给警告系统本身的消息。特别地，Lua 中的标准警告函数识别控制消息 `"@off"`（停止发出警告）和 `"@on"`（（重新）开始发出）；它忽略未知的控制消息。

### xpcall (f, msgh [, arg1, ...])

This function is similar to pcall, except that it sets a new message handler msgh.
此函数类似于 `pcall`，不同之处在于它设置了新的消息处理程序 `msgh`。

- **实现**（implementation）：在栈上重新排列函数、错误处理程序和参数，通过 `lua_pcallk` 以错误处理程序作为消息处理函数进行保护调用。Key source files: `lbaselib.c:luaB_xpcall`。

## 6.2 – Coroutine Manipulation（协程操作）

This library comprises the operations to manipulate coroutines, which come inside the table coroutine. See §2.6 for a general description of coroutines.
此库包含操作协程的操作，这些操作位于表 `coroutine` 中。有关协程的一般描述，请参见 §2.6。

### coroutine.close (co)

Closes coroutine co, that is, closes all its pending to-be-closed variables and puts the coroutine in a dead state. The given coroutine must be dead or suspended. In case of error (either the original error that stopped the coroutine or errors in closing methods), returns false plus the error object; otherwise returns true.
关闭协程 `co`，也就是说，关闭其所有待处理的待关闭变量并将协程置于死亡状态。给定的协程必须是死亡或挂起的。在出错的情况下（无论是停止协程的原始错误还是关闭方法中的错误），返回 `false` 加错误对象；否则返回 `true`。

### coroutine.create (f)

Creates a new coroutine, with body f. f must be a function. Returns this new coroutine, an object with type "thread".
创建一个新的协程，主体为 `f`。`f` 必须是函数。返回此新协程，一个类型为 `"thread"` 的对象。

- **实现**（implementation）：调用 `lua_newthread` 创建新线程，并将函数 `f` 通过 `lua_xmove` 移至新线程栈顶。Key source files: `lcorolib.c:luaB_cocreate`。

### coroutine.isyieldable ([co])

Returns true when the coroutine co can yield. The default for co is the running coroutine.
当协程 `co` 可以让出时返回 `true`。`co` 的默认值是正在运行的协程。

A coroutine is yieldable if it is not the main thread and it is not inside a non-yieldable C function.
如果协程不是主线程且不在不可让出的 **C 函数**内部，则它是可让出的。

### coroutine.resume (co [, val1, ...])

Starts or continues the execution of coroutine co. The first time you resume a coroutine, it starts running its body. The values val1, ... are passed as the arguments to the body function. If the coroutine has yielded, resume restarts it; the values val1, ... are passed as the results from the yield.
启动或继续协程 `co` 的执行。第一次恢复协程时，它开始运行其主体。值 `val1`、... 作为参数传递给主体函数。如果协程已让出，则 `resume` 重新启动它；值 `val1`、... 作为 yield 的结果传递。

If the coroutine runs without any errors, resume returns true plus any values passed to yield (when the coroutine yields) or any values returned by the body function (when the coroutine terminates). If there is any error, resume returns false plus the error message.
如果协程运行没有任何错误，则 `resume` 返回 `true` 加传递给 `yield` 的任何值（当协程让出时）或主体函数返回的任何值（当协程终止时）。如果有任何错误，`resume` 返回 `false` 加错误消息。

- **实现**（implementation）：通过 `auxresume` 将参数 `lua_xmove` 到协程栈，再调用 `lua_resume` 执行，出错时返回 `false` 和错误信息。Key source files: `lcorolib.c:luaB_coresume`、`lcorolib.c:auxresume`。

### coroutine.running ()

Returns the running coroutine plus a boolean, true when the running coroutine is the main one.
返回正在运行的协程和一个布尔值，当正在运行的协程是主协程时为 `true`。

### coroutine.status (co)

Returns the status of the coroutine co, as a string: "running", if the coroutine is running (that is, it is the one that called status); "suspended", if the coroutine is suspended in a call to yield, or if it has not started running yet; "normal" if the coroutine is active but not running (that is, it has resumed another coroutine); and "dead" if the coroutine has finished its body function, or if it has stopped with an error.
返回协程 `co` 的状态，作为字符串：如果协程正在运行（即它是调用 `status` 的协程），则为 `"running"`；如果协程在调用 `yield` 时挂起，或者尚未开始运行，则为 `"suspended"`；如果协程处于活动状态但未运行（即它已恢复另一个协程），则为 `"normal"`；如果协程已完成其主体函数，或者因错误而停止，则为 `"dead"`。

### coroutine.wrap (f)

Creates a new coroutine, with body f; f must be a function. Returns a function that resumes the coroutine each time it is called. Any arguments passed to this function behave as the extra arguments to resume. The function returns the same values returned by resume, except the first boolean. In case of error, the function closes the coroutine and propagates the error.
创建一个新的协程，主体为 `f`；`f` 必须是函数。返回一个每次调用时恢复协程的函数。传递给此函数的任何参数都表现为 `resume` 的额外参数。该函数返回与 `resume` 返回的相同值，除了第一个布尔值。在出错的情况下，该函数关闭协程并传播错误。

- **实现**（implementation）：先创建协程，再返回一个 C 闭包 `luaB_auxwrap`，每次调用时通过 `auxresume` 恢复协程并在出错时关闭协程。Key source files: `lcorolib.c:luaB_cowrap`、`lcorolib.c:luaB_auxwrap`。

### coroutine.yield (...)

Suspends the execution of the calling coroutine. Any arguments to yield are passed as extra results to resume.
挂起调用协程的执行。`yield` 的任何参数都作为额外结果传递给 `resume`。

- **实现**（implementation）：直接调用底层 `lua_yield` 挂起当前协程，并将所有参数作为 yield 结果返回。Key source files: `lcorolib.c:luaB_yield`。

## 6.3 – Modules（模块）

The package library provides basic facilities for loading modules in Lua. It exports one function directly in the global environment: require. Everything else is exported in the table package.
包库提供了在 Lua 中加载模块的基本工具。它直接在全局环境中导出一个函数：`require`。其他所有内容都导出在表 `package` 中。

### require (modname)

Loads the given module. The function starts by looking into the package.loaded table to determine whether modname is already loaded. If it is, then require returns the value stored at package.loaded[modname]. (The absence of a second result in this case signals that this call did not have to load the module.) Otherwise, it tries to find a loader for the module.
加载给定模块。函数首先查看 `package.loaded` 表以确定 `modname` 是否已加载。如果是，则 `require` 返回存储在 `package.loaded[modname]` 处的值。（在这种情况下缺少第二个结果表示此调用不必加载模块。）否则，它尝试找到模块的加载器。

To find a loader, require is guided by the table package.searchers. Each item in this table is a search function, that searches for the module in a particular way. By changing this table, we can change how require looks for a module. The following explanation is based on the default configuration for package.searchers.
要查找加载器，`require` 由表 `package.searchers` 引导。此表中的每个项目都是一个搜索函数，以特定方式搜索模块。通过更改此表，我们可以更改 `require` 查找模块的方式。以下解释基于 `package.searchers` 的默认配置。

First require queries package.preload[modname]. If it has a value, this value (which must be a function) is the loader.
首先，`require` 查询 `package.preload[modname]`。如果它有值，则此值（必须是函数）就是加载器。

Otherwise require searches for a Lua loader using the path stored in package.path. If that also fails, it searches for a C loader using the path stored in package.cpath. If that also fails, it tries an all-in-one loader (see package.searchers).
否则，`require` 使用存储在 `package.path` 中的路径搜索 Lua 加载器。如果这也失败了，它使用存储在 `package.cpath` 中的路径搜索 **C 加载器**。如果这也失败了，它尝试**一体化加载器**（见 `package.searchers`）。

Once a loader is found, require calls the loader with two arguments: modname and an extra value, a loader data, also returned by the searcher. The loader data can be any value useful to the module; for the default searchers, it indicates where the loader was found. (For instance, if the loader came from a file, this extra value is the file path.) If the loader returns any non-nil value, require assigns the returned value to package.loaded[modname]. If the loader does not return a non-nil value and has not assigned any value to package.loaded[modname], then require assigns true to this entry. In any case, require returns the final value of package.loaded[modname]. Besides that value, require also returns as a second result the loader data returned by the searcher, which indicates how require found the module.
找到加载器后，`require` 使用两个参数调用加载器：`modname` 和一个额外值，即**加载器数据**，也由搜索器返回。加载器数据可以是模块有用的任何值；对于默认搜索器，它指示加载器是在哪里找到的。（例如，如果加载器来自文件，则此额外值是文件路径。）如果加载器返回任何非 `nil` 值，则 `require` 将返回的值赋给 `package.loaded[modname]`。如果加载器不返回非 `nil` 值且没有将任何值赋给 `package.loaded[modname]`，则 `require` 将 `true` 赋给此条目。无论如何，`require` 返回 `package.loaded[modname]` 的最终值。除了该值之外，`require` 还返回搜索器返回的加载器数据作为第二个结果，该数据表明 `require` 如何找到模块。

If there is any error loading or running the module, or if it cannot find any loader for the module, then require raises an error.
如果加载或运行模块时出现任何错误，或者找不到模块的任何加载器，则 `require` 会引发错误。

- **实现**（implementation）：先检查 `package.loaded`，未加载时通过 `findloader` 遍历 `package.searchers` 查找加载器，调用后将结果写入 `package.loaded`。Key source files: `loadlib.c:ll_require`、`loadlib.c:findloader`。

### package.config

A string describing some compile-time configurations for packages. This string is a sequence of lines:
描述包的一些编译时配置的字符串。此字符串是一系列行：

- The first line is the directory separator string. Default is "\" for Windows and "/" for all other systems.
- The second line is the character that separates templates in a path. Default is ";".
- The third line is the string that marks the substitution points in a template. Default is "?".
- The fourth line is a string that, in a path in Windows, is replaced by the executable's directory. Default is "!".
- The fifth line is a mark to ignore all text after it when building the luaopen_ function name. Default is "-".

### package.cpath

A string with the path used by require to search for a C loader.
`require` 用于搜索 **C 加载器**的路径字符串。

Lua initializes the C path package.cpath in the same way it initializes the Lua path package.path, using the environment variable LUA_CPATH_5_4, or the environment variable LUA_CPATH, or a default path defined in luaconf.h.
Lua 初始化 **C 路径** `package.cpath` 的方式与初始化 Lua 路径 `package.path` 的方式相同，使用环境变量 `LUA_CPATH_5_4`、环境变量 `LUA_CPATH` 或 `luaconf.h` 中定义的默认路径。

### package.loaded

A table used by require to control which modules are already loaded. When you require a module modname and package.loaded[modname] is not false, require simply returns the value stored there.
`require` 用于控制哪些模块已加载的表。当你请求模块 `modname` 且 `package.loaded[modname]` 不为 false 时，`require` 只返回存储在那里的值。

This variable is only a reference to the real table; assignments to this variable do not change the table used by require. The real table is stored in the C registry (see §4.3), indexed by the key LUA_LOADED_TABLE, a string.
此变量只是对真实表的引用；对此变量的赋值不会更改 `require` 使用的表。真实表存储在 **C 注册表**中（见 §4.3），由键 `LUA_LOADED_TABLE`（字符串）索引。

### package.loadlib (libname, funcname)

Dynamically links the host program with the C library libname.
将宿主程序与 **C 库** `libname` 动态链接。

If funcname is "*", then it only links with the library, making the symbols exported by the library available to other dynamically linked libraries. Otherwise, it looks for a function funcname inside the library and returns this function as a C function. So, funcname must follow the lua_CFunction prototype (see lua_CFunction).
如果 `funcname` 为 `"*"`，则它仅与库链接，使库导出的符号可供其他动态链接库使用。否则，它在库中查找函数 `funcname` 并将此函数作为 **C 函数**返回。因此，`funcname` 必须遵循 `lua_CFunction` 原型（见 `lua_CFunction`）。

This is a low-level function. It completely bypasses the package and module system. Unlike require, it does not perform any path searching and does not automatically adds extensions. libname must be the complete file name of the C library, including if necessary a path and an extension. funcname must be the exact name exported by the C library (which may depend on the C compiler and linker used).
这是一个低级函数。它完全绕过包和模块系统。与 `require` 不同，它不执行任何路径搜索，也不会自动添加扩展名。`libname` 必须是 **C 库**的完整文件名，包括必要时的路径和扩展名。`funcname` 必须是 **C 库**导出的确切名称（这可能取决于使用的 **C 编译器**和链接器）。

This functionality is not supported by ISO C. As such, it is only available on some platforms (Windows, Linux, Mac OS X, Solaris, BSD, plus other Unix systems that support the dlfcn standard).
此功能不受 **ISO C** 支持。因此，它仅在某些平台上可用（Windows、Linux、Mac OS X、Solaris、BSD，以及支持 `dlfcn` 标准的其他 Unix 系统）。

This function is inherently insecure, as it allows Lua to call any function in any readable dynamic library in the system. (Lua calls any function assuming the function has a proper prototype and respects a proper protocol (see lua_CFunction). Therefore, calling an arbitrary function in an arbitrary dynamic library more often than not results in an access violation.)
此函数本质上是不安全的，因为它允许 Lua 调用系统中任何可读动态库中的任何函数。（Lua 调用任何函数时都假设该函数具有适当的原型并遵守适当的协议（见 `lua_CFunction`）。因此，在任意动态库中调用任意函数通常会导致访问冲突。）

### package.path

A string with the path used by require to search for a Lua loader.
`require` 用于搜索 Lua 加载器的路径字符串。

At start-up, Lua initializes this variable with the value of the environment variable LUA_PATH_5_4 or the environment variable LUA_PATH or with a default path defined in luaconf.h, if those environment variables are not defined. A ";;" in the value of the environment variable is replaced by the default path.
启动时，Lua 使用环境变量 `LUA_PATH_5_4` 或环境变量 `LUA_PATH` 的值初始化此变量，如果这些环境变量未定义，则使用 `luaconf.h` 中定义的默认路径。环境变量值中的 `";;"` 被默认路径替换。

### package.preload

A table to store loaders for specific modules (see require).
用于存储特定模块加载器的表（见 `require`）。

This variable is only a reference to the real table; assignments to this variable do not change the table used by require. The real table is stored in the C registry (see §4.3), indexed by the key LUA_PRELOAD_TABLE, a string.
此变量只是对真实表的引用；对此变量的赋值不会更改 `require` 使用的表。真实表存储在 **C 注册表**中（见 §4.3），由键 `LUA_PRELOAD_TABLE`（字符串）索引。

### package.searchers

A table used by require to control how to find modules.
`require` 用于控制如何查找模块的表。

Each entry in this table is a searcher function. When looking for a module, require calls each of these searchers in ascending order, with the module name (the argument given to require) as its sole argument. If the searcher finds the module, it returns another function, the module loader, plus an extra value, a loader data, that will be passed to that loader and returned as a second result by require. If it cannot find the module, it returns a string explaining why (or nil if it has nothing to say).
此表中的每个条目都是一个搜索器函数。查找模块时，`require` 按升序调用这些搜索器中的每一个，以模块名称（给 `require` 的参数）作为其唯一参数。如果搜索器找到模块，则返回另一个函数，即模块**加载器**，加上一个额外值，即**加载器数据**，该数据将传递给该加载器并作为 `require` 的第二个结果返回。如果找不到模块，则返回解释原因的字符串（或如果无话可说则返回 `nil`）。

Lua initializes this table with four searcher functions.
Lua 使用四个搜索器函数初始化此表。

The first searcher simply looks for a loader in the package.preload table.
第一个搜索器只是在 `package.preload` 表中查找加载器。

The second searcher looks for a loader as a Lua library, using the path stored at package.path. The search is done as described in function package.searchpath.
第二个搜索器使用存储在 `package.path` 的路径将加载器查找为 Lua 库。搜索按照 `package.searchpath` 函数中的描述进行。

The third searcher looks for a loader as a C library, using the path given by the variable package.cpath. Again, the search is done as described in function package.searchpath. For instance, if the C path is the string
第三个搜索器使用变量 `package.cpath` 给出的路径将加载器查找为 **C 库**。同样，搜索按照 `package.searchpath` 函数中的描述进行。例如，如果 **C 路径**是字符串

```
"./?.so;./?.dll;/usr/local/?/init.so"
```

the searcher for module foo will try to open the files ./foo.so, ./foo.dll, and /usr/local/foo/init.so, in that order. Once it finds a C library, this searcher first uses a dynamic link facility to link the application with the library. Then it tries to find a C function inside the library to be used as the loader. The name of this C function is the string "luaopen_" concatenated with a copy of the module name where each dot is replaced by an underscore. Moreover, if the module name has a hyphen, its suffix after (and including) the first hyphen is removed. For instance, if the module name is a.b.c-v2.1, the function name will be luaopen_a_b_c.
模块 `foo` 的搜索器将尝试按此顺序打开文件 `./foo.so`、`./foo.dll` 和 `/usr/local/foo/init.so`。找到 **C 库**后，此搜索器首先使用动态链接工具将应用程序与库链接。然后它尝试在库中查找要用作加载器的 **C 函数**。此 **C 函数**的名称是字符串 `"luaopen_"` 与模块名称副本的连接，其中每个点都替换为下划线。此外，如果模块名称有连字符，则删除第一个连字符之后（包括）的后缀。例如，如果模块名称为 `a.b.c-v2.1`，则函数名称将为 `luaopen_a_b_c`。

The fourth searcher tries an all-in-one loader. It searches the C path for a library for the root name of the given module. For instance, when requiring a.b.c, it will search for a C library for a. If found, it looks into it for an open function for the submodule; in our example, that would be luaopen_a_b_c. With this facility, a package can pack several C submodules into one single library, with each submodule keeping its original open function.
第四个搜索器尝试**一体化加载器**。它在 **C 路径**中搜索给定模块的根名称的库。例如，当请求 `a.b.c` 时，它将搜索 `a` 的 **C 库**。如果找到，它将在其中查找子模块的打开函数；在我们的示例中，那将是 `luaopen_a_b_c`。借助此工具，包可以将多个 **C 子模块**打包到单个库中，每个子模块保留其原始打开函数。

All searchers except the first one (preload) return as the extra value the file path where the module was found, as returned by package.searchpath. The first searcher always returns the string ":preload:".
除第一个（预加载）之外的所有搜索器都返回模块找到的文件路径作为额外值，如 `package.searchpath` 返回。第一个搜索器始终返回字符串 `":preload:"`。

Searchers should raise no errors and have no side effects in Lua. (They may have side effects in C, for instance by linking the application with a library.)
搜索器不应引发错误，也不应在 Lua 中产生副作用。（它们可能在 C 中产生副作用，例如通过将应用程序与库链接。）

- **实现**（implementation）：`luaopen_package` 通过 `createsearcherstable` 初始化四个搜索器：preload、Lua 文件、C 库、C 一体化加载器，依次尝试查找模块。Key source files: `loadlib.c:createsearcherstable`、`loadlib.c:searcher_preload`、`loadlib.c:searcher_Lua`、`loadlib.c:searcher_C`、`loadlib.c:searcher_Croot`。

### package.searchpath (name, path [, sep [, rep]])

Searches for the given name in the given path.
在给定路径中搜索给定名称。

A path is a string containing a sequence of templates separated by semicolons. For each template, the function replaces each interrogation mark (if any) in the template with a copy of name wherein all occurrences of sep (a dot, by default) were replaced by rep (the system's directory separator, by default), and then tries to open the resulting file name.
路径是包含由分号分隔的一系列模板的字符串。对于每个模板，函数将模板中的每个问号（如果有）替换为 `name` 的副本，其中 `sep` 的所有出现（默认为点）都被替换为 `rep`（默认为系统的目录分隔符），然后尝试打开结果文件名。

For instance, if the path is the string
例如，如果路径是字符串

```
"./?.lua;./?.lc;/usr/local/?/init.lua"
```

the search for the name foo.a will try to open the files ./foo/a.lua, ./foo/a.lc, and /usr/local/foo/a/init.lua, in that order.
名称 `foo.a` 的搜索将尝试按此顺序打开文件 `./foo/a.lua`、`./foo/a.lc` 和 `/usr/local/foo/a/init.lua`。

Returns the resulting name of the first file that it can open in read mode (after closing the file), or fail plus an error message if none succeeds. (This error message lists all file names it tried to open.)
返回它可以以读取模式打开的第一个文件的结果名称（关闭文件后），如果都不成功，则返回 `fail` 加错误消息。（此错误消息列出它尝试打开的所有文件名。）


## 6.4 – String Manipulation（字符串操作）

This library provides generic functions for string manipulation, such as finding and extracting substrings, and pattern matching. When indexing a string in Lua, the first character is at position 1 (not at 0, as in C). Indices are allowed to be negative and are interpreted as indexing backwards, from the end of the string. Thus, the last character is at position -1, and so on.
此库提供了用于字符串操作的通用函数，例如查找和提取子字符串以及模式匹配。在 Lua 中索引字符串时，第一个字符位于位置 1（不是位置 0，与 C 中不同）。允许使用负索引，并将其解释为从字符串末尾向后索引。因此，最后一个字符位于位置 -1，依此类推。

The string library provides all its functions inside the table string. It also sets a metatable for strings where the __index field points to the string table. Therefore, you can use the string functions in object-oriented style. For instance, string.byte(s,i) can be written as s:byte(i).
字符串库在表 `string` 中提供其所有函数。它还为字符串设置了一个元表，其中 `__index` 字段指向 `string` 表。因此，你可以以面向对象的风格使用字符串函数。例如，`string.byte(s,i)` 可以写为 `s:byte(i)`。

The string library assumes one-byte character encodings.
字符串库假设使用单字节字符编码。

### string.byte (s [, i [, j]])

Returns the internal numeric codes of the characters s[i], s[i+1], ..., s[j]. The default value for i is 1; the default value for j is i. These indices are corrected following the same rules of function string.sub.
返回字符 `s[i]`、`s[i+1]`、...、`s[j]` 的内部数字代码。`i` 的默认值为 1；`j` 的默认值为 `i`。这些索引按照与 `string.sub` 函数相同的规则进行修正。

Numeric codes are not necessarily portable across platforms.
数字代码不一定跨平台可移植。

### string.char (...)

Receives zero or more integers. Returns a string with length equal to the number of arguments, in which each character has the internal numeric code equal to its corresponding argument.
接收零个或多个整数。返回长度等于参数数量的字符串，其中每个字符的内部数字代码等于其对应的参数。

Numeric codes are not necessarily portable across platforms.
数字代码不一定跨平台可移植。

### string.dump (function [, strip])

Returns a string containing a binary representation (a binary chunk) of the given function, so that a later load on this string returns a copy of the function (but with new upvalues). If strip is a true value, the binary representation may not include all debug information about the function, to save space.
返回包含给定函数的二进制表示（**二进制块**）的字符串，以便稍后在此字符串上调用 `load` 返回函数的副本（但具有新的上值）。如果 `strip` 为真值，二进制表示可能不包含有关函数的所有调试信息，以节省空间。

Functions with upvalues have only their number of upvalues saved. When (re)loaded, those upvalues receive fresh instances. (See the load function for details about how these upvalues are initialized. You can use the debug library to serialize and reload the upvalues of a function in a way adequate to your needs.)
具有上值的函数仅保存其上值的数量。当（重新）加载时，这些上值接收新的实例。（有关如何初始化这些上值的详细信息，请参见 `load` 函数。你可以使用调试库以适合你需求的方式序列化和重新加载函数的上值。）

- **实现**（implementation）：调用 `lua_dump` 将函数序列化为二进制块，通过自定义 `writer` 回调将结果写入缓冲区。Key source files: `lstrlib.c:str_dump`。

### string.find (s, pattern [, init [, plain]])

Looks for the first match of pattern (see §6.4.1) in the string s. If it finds a match, then find returns the indices of s where this occurrence starts and ends; otherwise, it returns fail. A third, optional numeric argument init specifies where to start the search; its default value is 1 and can be negative. A true as a fourth, optional argument plain turns off the pattern matching facilities, so the function does a plain "find substring" operation, with no characters in pattern being considered magic.
在字符串 `s` 中查找 `pattern` 的第一次匹配（见 §6.4.1）。如果找到匹配，则 `find` 返回此出现开始和结束的 `s` 的索引；否则，它返回 `fail`。第三个可选数字参数 `init` 指定开始搜索的位置；其默认值为 1，可以为负数。第四个可选参数 `plain` 为 `true` 时关闭模式匹配功能，因此函数执行简单的"查找子字符串"操作，`pattern` 中的任何字符都不被视为魔法字符。

If the pattern has captures, then in a successful match the captured values are also returned, after the two indices.
如果模式具有捕获，则在成功匹配后，捕获的值也会在两个索引之后返回。

- **实现**（implementation）：内部统一调用 `str_find_aux` 进行模式匹配，`plain` 为真时关闭模式匹配只做简单子串查找。Key source files: `lstrlib.c:str_find`、`lstrlib.c:str_find_aux`。

### string.format (formatstring, ...)

Returns a formatted version of its variable number of arguments following the description given in its first argument, which must be a string. The format string follows the same rules as the ANSI sprintf. The only differences are that the conversion specifiers and modifiers F, n, *, h, L, and l are not supported and that there is an extra specifier, q. Both width and precision, when present, are limited to two digits.
返回根据其第一个参数（必须是字符串）中给出的描述对其可变数量参数进行格式化的版本。格式字符串遵循与 ANSI `sprintf` 相同的规则。唯一的区别是不支持转换说明符和修饰符 `F`、`n`、`*`、`h`、`L` 和 `l`，并且有一个额外的说明符 `q`。宽度和精度（如果存在）均限制为两位数。

The specifier q formats booleans, nil, numbers, and strings in a way that the result is a valid constant in Lua source code. Booleans and nil are written in the obvious way (true, false, nil). Floats are written in hexadecimal, to preserve full precision. A string is written between double quotes, using escape sequences when necessary to ensure that it can safely be read back by the Lua interpreter.
说明符 `q` 以结果成为 Lua 源代码中有效常量的方式格式化布尔值、`nil`、数字和字符串。布尔值和 `nil` 以明显的方式书写（`true`、`false`、`nil`）。浮点数以十六进制书写，以保留完整精度。字符串写在双引号之间，必要时使用转义序列，以确保 Lua 解释器可以安全地读回它。

The conversion specifiers A, a, E, e, f, G, and g all expect a number as argument. The specifiers c, d, i, o, u, X, and x expect an integer. When Lua is compiled with a C89 compiler, the specifiers A and a (hexadecimal floats) do not support modifiers.
转换说明符 `A`、`a`、`E`、`e`、`f`、`G` 和 `g` 都需要数字作为参数。说明符 `c`、`d`、`i`、`o`、`u`、`X` 和 `x` 需要整数。当使用 C89 编译器编译 Lua 时，说明符 `A` 和 `a`（十六进制浮点数）不支持修饰符。

The specifier s expects a string; if its argument is not a string, it is converted to one following the same rules of tostring. If the specifier has any modifier, the corresponding string argument should not contain embedded zeros.
说明符 `s` 需要字符串；如果其参数不是字符串，则按照与 `tostring` 相同的规则将其转换为字符串。如果说明符具有任何修饰符，则相应的字符串参数不应包含嵌入的零。

The specifier p formats the pointer returned by lua_topointer. That gives a unique string identifier for tables, userdata, threads, strings, and functions. For other values (numbers, nil, booleans), this specifier results in a string representing the pointer NULL.
说明符 `p` 格式化 `lua_topointer` 返回的指针。这为表、userdata、线程、字符串和函数提供了唯一的字符串标识符。对于其他值（数字、`nil`、布尔值），此说明符产生表示指针 `NULL` 的字符串。

- **实现**（implementation）：解析格式字符串后逐字符处理，对说明符 `q` 做特殊处理以生成 Lua 安全常量，其余调用标准 C 格式化。Key source files: `lstrlib.c:str_format`。

### string.gmatch (s, pattern [, init])

Returns an iterator function that, each time it is called, returns the next captures from pattern (see §6.4.1) over the string s. If pattern specifies no captures, then the whole match is produced in each call. A third, optional numeric argument init specifies where to start the search; its default value is 1 and can be negative.
返回一个迭代器函数，每次调用时，它返回字符串 `s` 上 `pattern` 的下一个捕获（见 §6.4.1）。如果 `pattern` 没有指定捕获，则每次调用都产生整个匹配。第三个可选数字参数 `init` 指定开始搜索的位置；其默认值为 1，可以为负数。

As an example, the following loop will iterate over all the words from string s, printing one per line:
例如，以下循环将遍历字符串 `s` 中的所有单词，每行打印一个：

```lua
s = "hello world from Lua"
for w in string.gmatch(s, "%a+") do
  print(w)
end
```

The next example collects all pairs key=value from the given string into a table:
下一个示例从给定字符串中将所有 `key=value` 对收集到表中：

```lua
t = {}
s = "from=world, to=Lua"
for k, v in string.gmatch(s, "(%w+)=(%w+)") do
  t[k] = v
end
```

For this function, a caret ^ at the start of a pattern does not work as an anchor, as this would prevent the iteration.
对于此函数，模式开头的 `^` 不作为锚点工作，因为这会阻止迭代。

- **实现**（implementation）：返回一个 C 闭包 `gmatch_aux`，每次调用时执行模式匹配并返回下一个捕获，内部通过 `match` 函数遍历。Key source files: `lstrlib.c:gmatch`、`lstrlib.c:gmatch_aux`。

### string.gsub (s, pattern, repl [, n])

Returns a copy of s in which all (or the first n, if given) occurrences of the pattern (see §6.4.1) have been replaced by a replacement string specified by repl, which can be a string, a table, or a function. gsub also returns, as its second value, the total number of matches that occurred. The name gsub comes from Global SUBstitution.
返回 `s` 的副本，其中 `pattern` 的所有（如果给定，则为前 `n` 个）出现已被 `repl` 指定的替换字符串替换，`repl` 可以是字符串、表或函数。`gsub` 还返回发生的匹配总数作为其第二个值。名称 `gsub` 来自 **Global SUBstitution**（全局替换）。

If repl is a string, then its value is used for replacement. The character % works as an escape character: any sequence in repl of the form %d, with d between 1 and 9, stands for the value of the d-th captured substring; the sequence %0 stands for the whole match; the sequence %% stands for a single %.
如果 `repl` 是字符串，则使用其值进行替换。字符 `%` 用作转义字符：`repl` 中任何形式为 `%d` 的序列（其中 `d` 在 1 到 9 之间）代表第 `d` 个捕获子字符串的值；序列 `%0` 代表整个匹配；序列 `%%` 代表单个 `%`。

If repl is a table, then the table is queried for every match, using the first capture as the key.
如果 `repl` 是表，则对每个匹配查询表，使用第一个捕获作为键。

If repl is a function, then this function is called every time a match occurs, with all captured substrings passed as arguments, in order.
如果 `repl` 是函数，则每次发生匹配时都会调用此函数，所有捕获的子字符串按顺序作为参数传递。

In any case, if the pattern specifies no captures, then it behaves as if the whole pattern was inside a capture.
无论如何，如果模式没有指定捕获，则其行为就好像整个模式都在捕获中一样。

If the value returned by the table query or by the function call is a string or a number, then it is used as the replacement string; otherwise, if it is false or nil, then there is no replacement (that is, the original match is kept in the string).
如果表查询或函数调用返回的值是字符串或数字，则将其用作替换字符串；否则，如果它是 `false` 或 `nil`，则没有替换（也就是说，原始匹配保留在字符串中）。

Here are some examples:
以下是一些示例：

```lua
x = string.gsub("hello world", "(%w+)", "%1 %1")
--> x="hello hello world world"

x = string.gsub("hello world", "%w+", "%0 %0", 1)
--> x="hello hello world"

x = string.gsub("hello world from Lua", "(%w+)%s*(%w+)", "%2 %1")
--> x="world hello Lua from"

x = string.gsub("home = $HOME, user = $USER", "%$(%w+)", os.getenv)
--> x="home = /home/roberto, user = roberto"

x = string.gsub("4+5 = $return 4+5$", "%$(.-)%$", function (s)
      return load(s)()
    end)
--> x="4+5 = 9"

local t = {name="lua", version="5.4"}
x = string.gsub("$name-$version.tar.gz", "%$(%w+)", t)
--> x="lua-5.4.tar.gz"
```

- **实现**（implementation）：内部调用 `str_gsub` 进行模式匹配与替换，支持字符串、表、函数三种替换形式，并返回替换次数。Key source files: `lstrlib.c:str_gsub`。

### string.len (s)

Receives a string and returns its length. The empty string "" has length 0. Embedded zeros are counted, so "a\000bc\000" has length 5.
接收字符串并返回其长度。空字符串 `""` 的长度为 0。计算嵌入的零，因此 `"a\000bc\000"` 的长度为 5。

### string.lower (s)

Receives a string and returns a copy of this string with all uppercase letters changed to lowercase. All other characters are left unchanged. The definition of what an uppercase letter is depends on the current locale.
接收字符串并返回此字符串的副本，其中所有大写字母都更改为小写。所有其他字符保持不变。大写字母的定义取决于当前区域设置。

### string.match (s, pattern [, init])

Looks for the first match of the pattern (see §6.4.1) in the string s. If it finds one, then match returns the captures from the pattern; otherwise it returns fail. If pattern specifies no captures, then the whole match is returned. A third, optional numeric argument init specifies where to start the search; its default value is 1 and can be negative.
在字符串 `s` 中查找 `pattern` 的第一次匹配（见 §6.4.1）。如果找到一个，则 `match` 返回模式中的捕获；否则返回 `fail`。如果 `pattern` 没有指定捕获，则返回整个匹配。第三个可选数字参数 `init` 指定开始搜索的位置；其默认值为 1，可以为负数。

- **实现**（implementation）：与 `string.find` 共用 `str_find_aux`，区别在于只返回捕获值而不返回位置索引。Key source files: `lstrlib.c:str_match`、`lstrlib.c:str_find_aux`。

### string.pack (fmt, v1, v2, ...)

Returns a binary string containing the values v1, v2, etc. serialized in binary form (packed) according to the format string fmt (see §6.4.2).
返回包含值 `v1`、`v2` 等的二进制字符串，根据格式字符串 `fmt` 以二进制形式序列化（打包）（见 §6.4.2）。

- **实现**（implementation）：解析格式字符串，按选项逐个将参数序列化为二进制字节，处理对齐、字节序和溢出检查。Key source files: `lstrlib.c:str_pack`。

### string.packsize (fmt)

Returns the length of a string resulting from string.pack with the given format. The format string cannot have the variable-length options s or z (see §6.4.2).
返回使用给定格式进行 `string.pack` 所得字符串的长度。格式字符串不能具有可变长度选项 `s` 或 `z`（见 §6.4.2）。

### string.rep (s, n [, sep])

Returns a string that is the concatenation of n copies of the string s separated by the string sep. The default value for sep is the empty string (that is, no separator). Returns the empty string if n is not positive.
返回字符串，它是 `n` 个 `s` 的副本由字符串 `sep` 分隔的连接。`sep` 的默认值是空字符串（也就是说，没有分隔符）。如果 `n` 不是正数，则返回空字符串。

(Note that it is very easy to exhaust the memory of your machine with a single call to this function.)
（请注意，通过对此函数的单个调用很容易耗尽计算机的内存。）

### string.reverse (s)

Returns a string that is the string s reversed.
返回反转的字符串 `s`。

### string.sub (s, i [, j])

Returns the substring of s that starts at i and continues until j; i and j can be negative. If j is absent, then it is assumed to be equal to -1 (which is the same as the string length). In particular, the call string.sub(s,1,j) returns a prefix of s with length j, and string.sub(s, -i) (for a positive i) returns a suffix of s with length i.
返回从 `i` 开始并持续到 `j` 的 `s` 的子字符串；`i` 和 `j` 可以为负数。如果 `j` 缺失，则假设它等于 -1（与字符串长度相同）。特别地，调用 `string.sub(s,1,j)` 返回长度为 `j` 的 `s` 的前缀，而 `string.sub(s, -i)`（对于正数 `i`）返回长度为 `i` 的 `s` 的后缀。

If, after the translation of negative indices, i is less than 1, it is corrected to 1. If j is greater than the string length, it is corrected to that length. If, after these corrections, i is greater than j, the function returns the empty string.
如果负索引转换后，`i` 小于 1，则将其修正为 1。如果 `j` 大于字符串长度，则将其修正为该长度。如果这些修正后 `i` 大于 `j`，则函数返回空字符串。

### string.unpack (fmt, s [, pos])

Returns the values packed in string s (see string.pack) according to the format string fmt (see §6.4.2). An optional pos marks where to start reading in s (default is 1). After the read values, this function also returns the index of the first unread byte in s.
返回根据格式字符串 `fmt` 打包在字符串 `s` 中的值（见 `string.pack`）（见 §6.4.2）。可选的 `pos` 标记在 `s` 中开始读取的位置（默认值为 1）。读取值后，此函数还返回 `s` 中第一个未读取字节的索引。

- **实现**（implementation）：按格式字符串从二进制字符串中反序列化值，同样处理字节序和对齐，并返回下一个未读取位置。Key source files: `lstrlib.c:str_unpack`。

### string.upper (s)

Receives a string and returns a copy of this string with all lowercase letters changed to uppercase. All other characters are left unchanged. The definition of what a lowercase letter is depends on the current locale.
接收字符串并返回此字符串的副本，其中所有小写字母都更改为大写。所有其他字符保持不变。小写字母的定义取决于当前区域设置。

### 6.4.1 – Patterns（模式）

Patterns in Lua are described by regular strings, which are interpreted as patterns by the pattern-matching functions string.find, string.gmatch, string.gsub, and string.match. This section describes the syntax and the meaning (that is, what they match) of these strings.
Lua 中的模式由常规字符串描述，这些字符串由模式匹配函数 `string.find`、`string.gmatch`、`string.gsub` 和 `string.match` 解释为模式。本节描述这些字符串的语法和含义（即它们匹配什么）。

#### Character Class（字符类）

A character class is used to represent a set of characters. The following combinations are allowed in describing a character class:
**字符类**用于表示一组字符。描述字符类时允许以下组合：

- **x**（其中 `x` 不是魔法字符 `^$()%.[]*+-?` 之一）表示字符 `x` 本身。
- **.**（点）表示所有字符。
- **%a** 表示所有字母。
- **%c** 表示所有控制字符。
- **%d** 表示所有数字。
- **%g** 表示所有可打印字符，但不包括空格。
- **%l** 表示所有小写字母。
- **%p** 表示所有标点字符。
- **%s** 表示所有空白字符。
- **%u** 表示所有大写字母。
- **%w** 表示所有字母数字字符。
- **%x** 表示所有十六进制数字。
- **%x**（其中 `x` 是任何非字母数字字符）表示字符 `x`。这是转义魔法字符的标准方式。任何非字母数字字符（包括所有标点字符，即使是非魔法的）前面都可以加 `%` 以在模式中表示其自身。
- **[set]** 表示所有在 `set` 中的字符的并集。可以通过用 `-` 分隔范围的起止字符（按升序）来指定字符范围。上面描述的所有类 `%x` 也可以用作 `set` 中的组件。`set` 中的所有其他字符表示其自身。例如，`[%w_]`（或 `[_%w]`）表示所有字母数字字符加下划线，`[0-7]` 表示八进制数字，`[0-7%l%-]` 表示八进制数字加小写字母加 `-` 字符。你可以通过将右方括号作为集合中的第一个字符来将其放入集合中。你可以通过将连字符作为集合中的第一个或最后一个字符来将其放入集合中。（你也可以在两种情况下都使用转义。）范围与类之间的交互未定义。因此，`[%a-z]` 或 `[a-%%]` 等模式没有意义。
- **[^set]** 表示 `set` 的补集，其中 `set` 如上所述解释。

For all classes represented by single letters (%a, %c, etc.), the corresponding uppercase letter represents the complement of the class. For instance, %S represents all non-space characters.
对于所有由单字母表示的类（`%a`、`%c` 等），相应的大写字母表示该类的补集。例如，`%S` 表示所有非空白字符。

The definitions of letter, space, and other character groups depend on the current locale. In particular, the class [a-z] may not be equivalent to %l.
字母、空格和其他字符组的定义取决于当前区域设置。特别地，`[a-z]` 类可能不等价于 `%l`。

#### Pattern Item（模式项）

A pattern item can be:
**模式项**可以是：

- 单个字符类，匹配类中的任何单个字符；
- 单个字符类后跟 `*`，匹配类中零个或多个字符的序列。这些重复项始终匹配尽可能长的序列；
- 单个字符类后跟 `+`，匹配类中一个或多个字符的序列。这些重复项始终匹配尽可能长的序列；
- 单个字符类后跟 `-`，也匹配类中零个或多个字符的序列。与 `*` 不同，这些重复项始终匹配尽可能短的序列；
- 单个字符类后跟 `?`，匹配类中零个或一个字符的出现。如果可能，它始终匹配一个出现；
- `%n`（`n` 在 1 到 9 之间）；此类项匹配等于第 `n` 个捕获字符串的子字符串（见下文）；
- `%bxy`（`x` 和 `y` 是两个不同的字符）；此类项匹配以 `x` 开头、以 `y` 结尾的字符串，其中 `x` 和 `y` 是**平衡的**。这意味着，如果从左到右读取字符串，为 `x` 计数 `+1`，为 `y` 计数 `-1`，则结尾的 `y` 是计数达到 0 的第一个 `y`。例如，项 `%b()` 匹配平衡括号的表达式。
- `%f[set]`，一个**边界模式**；此类项在任何位置匹配空字符串，使得下一个字符属于 `set`，而前一个字符不属于 `set`。集合 `set` 按前述方式解释。主题的开头和结尾被当作字符 `\0` 处理。

#### Pattern（模式）

A pattern is a sequence of pattern items. A caret ^ at the beginning of a pattern anchors the match at the beginning of the subject string. A $ at the end of a pattern anchors the match at the end of the subject string. At other positions, ^ and $ have no special meaning and represent themselves.
**模式**是模式项的序列。模式开头的 `^` 将匹配锚定在主题字符串的开头。模式末尾的 `$` 将匹配锚定在主题字符串的末尾。在其他位置，`^` 和 `$` 没有特殊含义，表示其自身。

#### Captures（捕获）

A pattern can contain sub-patterns enclosed in parentheses; they describe captures. When a match succeeds, the substrings of the subject string that match captures are stored (captured) for future use. Captures are numbered according to their left parentheses. For instance, in the pattern "(a*(.)%w(%s*))", the part of the string matching "a*(.)%w(%s*)" is stored as the first capture, and therefore has number 1; the character matching "." is captured with number 2, and the part matching "%s*" has number 3.
模式可以包含用括号括起来的子模式；它们描述**捕获**。当匹配成功时，主题字符串中与捕获匹配的子字符串被存储（捕获）以供将来使用。捕获根据其左括号编号。例如，在模式 `"(a*(.)%w(%s*))"` 中，与 `"a*(.)%w(%s*)"` 匹配的字符串部分作为第一个捕获存储，因此编号为 1；与 `"."` 匹配的字符以编号 2 捕获，而与 `"%s*"` 匹配的部分编号为 3。

As a special case, the capture () captures the current string position (a number). For instance, if we apply the pattern "()aa()" on the string "flaaap", there will be two captures: 3 and 5.
作为特例，捕获 `()` 捕获当前字符串位置（一个数字）。例如，如果我们在字符串 `"flaaap"` 上应用模式 `"()aa()"`，将有两个捕获：3 和 5。

#### Multiple matches（多次匹配）

The function string.gsub and the iterator string.gmatch match multiple occurrences of the given pattern in the subject. For these functions, a new match is considered valid only if it ends at least one byte after the end of the previous match. In other words, the pattern machine never accepts the empty string as a match immediately after another match.
函数 `string.gsub` 和迭代器 `string.gmatch` 匹配主题中给定模式的多次出现。对于这些函数，只有当新匹配在前一次匹配的结束之后至少一个字节处结束时，才认为它是有效的。换句话说，模式机永远不会在另一次匹配之后立即接受空字符串作为匹配。

### 6.4.2 – Format Strings for Pack and Unpack（打包和解包的格式字符串）

The first argument to string.pack, string.packsize, and string.unpack is a format string, which describes the layout of the structure being created or read.
`string.pack`、`string.packsize` 和 `string.unpack` 的第一个参数是格式字符串，它描述正在创建或读取的结构的布局。

A format string is a sequence of conversion options. The conversion options are as follows:
格式字符串是一系列转换选项。转换选项如下：

- **`<`**：设置小端序
- **`>`**：设置大端序
- **`=`**：设置本机字节序
- **`!n`**：将最大对齐设置为 `n`（默认是本机对齐）
- **`b`**：有符号字节（`char`）
- **`B`**：无符号字节（`char`）
- **`h`**：有符号 `short`（本机大小）
- **`H`**：无符号 `short`（本机大小）
- **`l`**：有符号 `long`（本机大小）
- **`L`**：无符号 `long`（本机大小）
- **`j`**：`lua_Integer`
- **`J`**：`lua_Unsigned`
- **`T`**：`size_t`（本机大小）
- **`in`**：具有 `n` 字节的有符号 `int`（默认是本机大小）
- **`In`**：具有 `n` 字节的无符号 `int`（默认是本机大小）
- **`f`**：`float`（本机大小）
- **`d`**：`double`（本机大小）
- **`n`**：`lua_Number`
- **`cn`**：具有 `n` 字节的固定大小字符串
- **`z`**：以零结尾的字符串
- **`sn`**：前面有其长度的字符串，编码为具有 `n` 字节的无符号整数（默认是 `size_t`）
- **`x`**：一字节填充
- **`Xop`**：根据选项 `op` 对齐的空项（否则忽略）
- **` `**（空格）：忽略

(A [n] means an optional integral numeral.)
（`[n]` 表示可选的整数数字。）

Except for padding, spaces, and configurations (options xX <=>!), each option corresponds to an argument in string.pack or a result in string.unpack.
除填充、空格和配置（选项 `xX <=>!`）外，每个选项对应于 `string.pack` 中的参数或 `string.unpack` 中的结果。

For options !n, sn, in, and In, n can be any integer between 1 and 16. All integral options check overflows; string.pack checks whether the given value fits in the given size; string.unpack checks whether the read value fits in a Lua integer. For the unsigned options, Lua integers are treated as unsigned values too.
对于选项 `!n`、`sn`、`in` 和 `In`，`n` 可以是 1 到 16 之间的任何整数。所有整数选项都检查溢出；`string.pack` 检查给定值是否适合给定大小；`string.unpack` 检查读取的值是否适合 Lua 整数。对于无符号选项，Lua 整数也被视为无符号值。

Any format string starts as if prefixed by "!1=", that is, with maximum alignment of 1 (no alignment) and native endianness.
任何格式字符串都像前缀为 `"!1="` 一样开始，也就是说，最大对齐为 1（无对齐）和本机字节序。

Native endianness assumes that the whole system is either big or little endian. The packing functions will not emulate correctly the behavior of mixed-endian formats.
本机字节序假设整个系统要么是大端序要么是小端序。打包函数不会正确模拟混合字节序格式的行为。

Alignment works as follows: For each option, the format gets extra padding until the data starts at an offset that is a multiple of the minimum between the option size and the maximum alignment; this minimum must be a power of 2. Options c and z are not aligned; option s follows the alignment of its starting integer.
对齐的工作方式如下：对于每个选项，格式获得额外的填充，直到数据在偏移量处开始，该偏移量是选项大小和最大对齐之间最小值的倍数；此最小值必须是 2 的幂。选项 `c` 和 `z` 不对齐；选项 `s` 遵循其起始整数的对齐方式。

All padding is filled with zeros by string.pack and ignored by string.unpack.
所有填充都由 `string.pack` 用零填充，并由 `string.unpack` 忽略。

## 6.5 – UTF-8 Support（UTF-8 支持）

This library provides basic support for UTF-8 encoding. It provides all its functions inside the table utf8. This library does not provide any support for Unicode other than the handling of the encoding. Any operation that needs the meaning of a character, such as character classification, is outside its scope.
此库为 **UTF-8** 编码提供基本支持。它在表 `utf8` 中提供其所有函数。此库除了处理编码外，不提供任何对 **Unicode** 的支持。任何需要字符含义的操作，例如字符分类，都不在其范围内。

Unless stated otherwise, all functions that expect a byte position as a parameter assume that the given position is either the start of a byte sequence or one plus the length of the subject string. As in the string library, negative indices count from the end of the string.
除非另有说明，否则所有期望字节位置作为参数的函数都假设给定位置是字节序列的开始或主题字符串长度加一。与字符串库一样，负索引从字符串末尾计数。

Functions that create byte sequences accept all values up to 0x7FFFFFFF, as defined in the original UTF-8 specification; that implies byte sequences of up to six bytes.
创建字节序列的函数接受直到 `0x7FFFFFFF` 的所有值，如原始 UTF-8 规范中所定义；这意味着最多六个字节的字节序列。

Functions that interpret byte sequences only accept valid sequences (well formed and not overlong). By default, they only accept byte sequences that result in valid Unicode code points, rejecting values greater than 10FFFF and surrogates. A boolean argument lax, when available, lifts these checks, so that all values up to 0x7FFFFFFF are accepted. (Not well formed and overlong sequences are still rejected.)
解释字节序列的函数只接受有效的序列（格式正确且不过长）。默认情况下，它们只接受产生有效 Unicode 代码点的字节序列，拒绝大于 `10FFFF` 的值和代理对。当可用时，布尔参数 `lax` 解除这些检查，以便接受直到 `0x7FFFFFFF` 的所有值。（格式不正确和过长的序列仍然被拒绝。）

### utf8.char (...)

Receives zero or more integers, converts each one to its corresponding UTF-8 byte sequence and returns a string with the concatenation of all these sequences.
接收零个或多个整数，将每个整数转换为其对应的 UTF-8 字节序列，并返回包含所有这些序列连接的字符串。

### utf8.charpattern

The pattern (a string, not a function) "[\0-\x7F\xC2-\xFD][\x80-\xBF]*" (see §6.4.1), which matches exactly one UTF-8 byte sequence, assuming that the subject is a valid UTF-8 string.
模式（字符串，不是函数）`"[\0-\x7F\xC2-\xFD][\x80-\xBF]*"`（见 §6.4.1），它恰好匹配一个 UTF-8 字节序列，假设主题是有效的 UTF-8 字符串。

### utf8.codes (s [, lax])

Returns values so that the construction
返回值，以便构造

```lua
for p, c in utf8.codes(s) do body end
```

will iterate over all UTF-8 characters in string s, with p being the position (in bytes) and c the code point of each character. It raises an error if it meets any invalid byte sequence.
将遍历字符串 `s` 中的所有 UTF-8 字符，`p` 是位置（以字节为单位），`c` 是每个字符的代码点。如果遇到任何无效字节序列，则会引发错误。

- **实现**（implementation）：返回 C 闭包 `iter_aux`，逐字符调用 `utf8_decode` 解码并检查延续字节，strict 模式下拒绝代理对和超出 Unicode 范围的值。Key source files: `lutf8lib.c:iter_codes`、`lutf8lib.c:iter_aux`、`lutf8lib.c:utf8_decode`。

### utf8.codepoint (s [, i [, j [, lax]]])

Returns the code points (as integers) from all characters in s that start between byte position i and j (both included). The default for i is 1 and for j is i. It raises an error if it meets any invalid byte sequence.
返回 `s` 中所有在字节位置 `i` 和 `j`（均包括）之间开始的字符的代码点（作为整数）。`i` 的默认值为 1，`j` 的默认值为 `i`。如果遇到任何无效字节序列，则会引发错误。

### utf8.len (s [, i [, j [, lax]]])

Returns the number of UTF-8 characters in string s that start between positions i and j (both inclusive). The default for i is 1 and for j is -1. If it finds any invalid byte sequence, returns fail plus the position of the first invalid byte.
返回字符串 `s` 中在位置 `i` 和 `j`（均包括）之间开始的 UTF-8 字符数量。`i` 的默认值为 1，`j` 的默认值为 -1。如果发现任何无效字节序列，则返回 `fail` 加第一个无效字节的位置。

- **实现**（implementation）：在指定区间内逐字节调用 `utf8_decode` 统计有效字符数，遇到无效序列时返回失败和当前位置。Key source files: `lutf8lib.c:utflen`、`lutf8lib.c:utf8_decode`。

### utf8.offset (s, n [, i])

Returns the position (in bytes) where the encoding of the n-th character of s (counting from position i) starts. A negative n gets characters before position i. The default for i is 1 when n is non-negative and #s + 1 otherwise, so that utf8.offset(s, -n) gets the offset of the n-th character from the end of the string. If the specified character is neither in the subject nor right after its end, the function returns fail.
返回 `s` 的第 `n` 个字符（从位置 `i` 计数）的编码开始的位置（以字节为单位）。负的 `n` 获取位置 `i` 之前的字符。当 `n` 为非负数时，`i` 的默认值为 1，否则为 `#s + 1`，因此 `utf8.offset(s, -n)` 获取从字符串末尾开始的第 `n` 个字符的偏移量。如果指定的字符既不在主题中也不在其结束之后，则函数返回 `fail`。

As a special case, when n is 0 the function returns the start of the encoding of the character that contains the i-th byte of s.
作为特例，当 `n` 为 0 时，函数返回包含 `s` 的第 `i` 个字节的字符的编码的开始。

This function assumes that s is a valid UTF-8 string.
此函数假设 `s` 是有效的 UTF-8 字符串。

- **实现**（implementation）：通过 `iscontp` 判断延续字节，从给定位置向前或向后遍历到字符边界，支持 `n == 0` 时返回当前字节所在字符的起始位置。Key source files: `lutf8lib.c:byteoffset`。

## 6.6 – Table Manipulation（表操作）

This library provides generic functions for table manipulation. It provides all its functions inside the table table.
此库提供了用于表操作的通用函数。它在表 `table` 中提供其所有函数。

Remember that, whenever an operation needs the length of a table, all caveats about the length operator apply (see §3.4.7). All functions ignore non-numeric keys in the tables given as arguments.
请记住，每当操作需要表的长度时，长度运算符的所有注意事项都适用（见 §3.4.7）。所有函数都忽略作为参数给出的表中的非数字键。

### table.concat (list [, sep [, i [, j]]])

Given a list where all elements are strings or numbers, returns the string list[i]..sep..list[i+1] ... sep..list[j]. The default value for sep is the empty string, the default for i is 1, and the default for j is #list. If i is greater than j, returns the empty string.
给定一个所有元素都是字符串或数字的列表，返回字符串 `list[i]..sep..list[i+1] ... sep..list[j]`。`sep` 的默认值为空字符串，`i` 的默认值为 1，`j` 的默认值为 `#list`。如果 `i` 大于 `j`，则返回空字符串。

- **实现**（implementation）：通过 `luaL_Buffer` 累积结果，对每个元素调用 `lua_geti` 读取并检查是否为字符串。Key source files: `ltablib.c:tconcat`、`ltablib.c:addfield`。

### table.insert (list, [pos,] value)

Inserts element value at position pos in list, shifting up the elements list[pos], list[pos+1], ..., list[#list]. The default value for pos is #list+1, so that a call table.insert(t,x) inserts x at the end of the list t.
在 `list` 的位置 `pos` 处插入元素 `value`，将元素 `list[pos]`、`list[pos+1]`、...、`list[#list]` 向上移动。`pos` 的默认值为 `#list+1`，因此调用 `table.insert(t,x)` 将 `x` 插入列表 `t` 的末尾。

- **实现**（implementation）：通过 `lua_seti` 和 `lua_geti` 移动元素，在末尾或指定位置插入新值。Key source files: `ltablib.c:tinsert`。

### table.move (a1, f, e, t [,a2])

Moves elements from the table a1 to the table a2, performing the equivalent to the following multiple assignment: a2[t],... = a1[f],...,a1[e]. The default for a2 is a1. The destination range can overlap with the source range. The number of elements to be moved must fit in a Lua integer.
将元素从表 `a1` 移动到表 `a2`，执行等同于以下多重赋值的操作：`a2[t],... = a1[f],...,a1[e]`。`a2` 的默认值为 `a1`。目标范围可以与源范围重叠。要移动的元素数量必须适合 Lua 整数。

Returns the destination table a2.
返回目标表 `a2`。

- **实现**（implementation）：根据目标范围与源范围的关系选择正序或倒序复制，通过 `lua_geti`/`lua_seti` 逐元素移动。Key source files: `ltablib.c:tmove`。

### table.pack (...)

Returns a new table with all arguments stored into keys 1, 2, etc. and with a field "n" with the total number of arguments. Note that the resulting table may not be a sequence, if some arguments are nil.
返回一个新表，其中所有参数存储在键 1、2 等中，并具有字段 `"n"`，其中包含参数的总数。请注意，如果某些参数为 `nil`，则结果表可能不是序列。

### table.remove (list [, pos])

Removes from list the element at position pos, returning the value of the removed element. When pos is an integer between 1 and #list, it shifts down the elements list[pos+1], list[pos+2], ..., list[#list] and erases element list[#list]; The index pos can also be 0 when #list is 0, or #list + 1.
从 `list` 中移除位置 `pos` 处的元素，返回被移除元素的值。当 `pos` 是 1 到 `#list` 之间的整数时，它将元素 `list[pos+1]`、`list[pos+2]`、...、`list[#list]` 向下移动并删除元素 `list[#list]`；当 `#list` 为 0 时，索引 `pos` 也可以为 0，或 `#list + 1`。

The default value for pos is #list, so that a call table.remove(l) removes the last element of the list l.
`pos` 的默认值为 `#list`，因此调用 `table.remove(l)` 会移除列表 `l` 的最后一个元素。

- **实现**（implementation）：通过 `lua_geti`/`lua_seti` 将后续元素向下移动一位，并在末尾置 `nil`。Key source files: `ltablib.c:tremove`。

### table.sort (list [, comp])

Sorts the list elements in a given order, in-place, from list[1] to list[#list]. If comp is given, then it must be a function that receives two list elements and returns true when the first element must come before the second in the final order, so that, after the sort, i <= j implies not comp(list[j],list[i]). If comp is not given, then the standard Lua operator < is used instead.
按给定顺序就地排序列表元素，从 `list[1]` 到 `list[#list]`。如果给定了 `comp`，则它必须是接收两个列表元素并返回 `true` 的函数，当第一个元素必须在最终顺序中位于第二个元素之前时，以便排序后，`i <= j` 意味着 `not comp(list[j],list[i])`。如果没有给定 `comp`，则使用标准 Lua 运算符 `<`。

The comp function must define a consistent order; more formally, the function must define a strict weak order. (A weak order is similar to a total order, but it can equate different elements for comparison purposes.)
`comp` 函数必须定义一致的顺序；更正式地说，该函数必须定义严格的弱序。（弱序类似于全序，但为了比较目的，它可以等同不同的元素。）

The sort algorithm is not stable: Different elements considered equal by the given order may have their relative positions changed by the sort.
排序算法不是稳定的：给定顺序认为相等的不同元素可能会因排序而改变其相对位置。

- **实现**（implementation）：使用基于 Sedgewick 的快速排序算法，在分区失衡时随机化 pivot，通过 `sort_comp` 调用 Lua 比较函数或 `lua_compare`。Key source files: `ltablib.c:sort`、`ltablib.c:auxsort`、`ltablib.c:partition`。

### table.unpack (list [, i [, j]])

Returns the elements from the given list. This function is equivalent to
返回给定列表中的元素。此函数等价于

```lua
return list[i], list[i+1], ..., list[j]
```

By default, i is 1 and j is #list.
默认情况下，`i` 为 1，`j` 为 `#list`。

- **实现**（implementation）：通过 `lua_geti` 将指定范围的元素压入栈并返回，使用 `lua_checkstack` 防止栈溢出。Key source files: `ltablib.c:tunpack`。

## 6.7 – Mathematical Functions（数学函数）

This library provides basic mathematical functions. It provides all its functions and constants inside the table math. Functions with the annotation "integer/float" give integer results for integer arguments and float results for non-integer arguments. The rounding functions math.ceil, math.floor, and math.modf return an integer when the result fits in the range of an integer, or a float otherwise.
此库提供了基本数学函数。它在表 `math` 中提供其所有函数和常量。带有注释 "integer/float" 的函数对整数参数给出整数结果，对非整数参数给出浮点结果。舍入函数 `math.ceil`、`math.floor` 和 `math.modf` 在结果适合整数范围时返回整数，否则返回浮点数。

### math.abs (x)

Returns the maximum value between x and -x. (integer/float)
返回 `x` 和 `-x` 之间的最大值。（整数/浮点数）

### math.acos (x)

Returns the arc cosine of x (in radians).
返回 `x` 的弧余弦（以弧度为单位）。

### math.asin (x)

Returns the arc sine of x (in radians).
返回 `x` 的弧正弦（以弧度为单位）。

### math.atan (y [, x])

Returns the arc tangent of y/x (in radians), using the signs of both arguments to find the quadrant of the result. It also handles correctly the case of x being zero.
返回 `y/x` 的弧正切（以弧度为单位），使用两个参数的符号来查找结果的象限。它还正确处理 `x` 为零的情况。

The default value for x is 1, so that the call math.atan(y) returns the arc tangent of y.
`x` 的默认值为 1，因此调用 `math.atan(y)` 返回 `y` 的弧正切。

### math.ceil (x)

Returns the smallest integral value greater than or equal to x.
返回大于或等于 `x` 的最小整数值。

### math.cos (x)

Returns the cosine of x (assumed to be in radians).
返回 `x` 的余弦（假设以弧度为单位）。

### math.deg (x)

Converts the angle x from radians to degrees.
将角度 `x` 从弧度转换为度。

### math.exp (x)

Returns the value e^x (where e is the base of natural logarithms).
返回值 `e^x`（其中 `e` 是自然对数的底）。

### math.floor (x)

Returns the largest integral value less than or equal to x.
返回小于或等于 `x` 的最大整数值。

### math.fmod (x, y)

Returns the remainder of the division of x by y that rounds the quotient towards zero. (integer/float)
返回 `x` 除以 `y` 的余数，商向零舍入。（整数/浮点数）

### math.huge

The float value HUGE_VAL, a value greater than any other numeric value.
浮点值 `HUGE_VAL`，一个大于任何其他数值的值。

### math.log (x [, base])

Returns the logarithm of x in the given base. The default for base is e (so that the function returns the natural logarithm of x).
返回以给定底数计算的 `x` 的对数。`base` 的默认值为 `e`（因此函数返回 `x` 的自然对数）。

### math.max (x, ...)

Returns the argument with the maximum value, according to the Lua operator <.
根据 Lua 运算符 `<` 返回具有最大值的参数。

### math.maxinteger

An integer with the maximum value for an integer.
具有整数最大值的整数。

### math.min (x, ...)

Returns the argument with the minimum value, according to the Lua operator <.
根据 Lua 运算符 `<` 返回具有最小值的参数。

### math.mininteger

An integer with the minimum value for an integer.
具有整数最小值的整数。

### math.modf (x)

Returns the integral part of x and the fractional part of x. Its second result is always a float.
返回 `x` 的整数部分和 `x` 的小数部分。其第二个结果始终是浮点数。

### math.pi

The value of pi.
`pi` 的值。

### math.rad (x)

Converts the angle x from degrees to radians.
将角度 `x` 从度转换为弧度。

### math.random ([m [, n]])

When called without arguments, returns a pseudo-random float with uniform distribution in the range [0,1). When called with two integers m and n, math.random returns a pseudo-random integer with uniform distribution in the range [m, n]. The call math.random(n), for a positive n, is equivalent to math.random(1,n). The call math.random(0) produces an integer with all bits (pseudo)random.
不带参数调用时，返回在范围 `[0,1)` 内均匀分布的伪随机浮点数。使用两个整数 `m` 和 `n` 调用时，`math.random` 返回在范围 `[m, n]` 内均匀分布的伪随机整数。对于正数 `n`，调用 `math.random(n)` 等价于 `math.random(1,n)`。调用 `math.random(0)` 产生所有位都是（伪）随机的整数。

This function uses the xoshiro256** algorithm to produce pseudo-random 64-bit integers, which are the results of calls with argument 0. Other results (ranges and floats) are unbiased extracted from these integers.
此函数使用 **xoshiro256**\*** 算法生成伪随机 64 位整数，这些是使用参数 0 调用的结果。其他结果（范围和浮点数）是从这些整数中无偏提取的。

Lua initializes its pseudo-random generator with the equivalent of a call to math.randomseed with no arguments, so that math.random should generate different sequences of results each time the program runs.
Lua 使用等效于无参数调用 `math.randomseed` 的方式初始化其伪随机生成器，以便 `math.random` 在程序每次运行时生成不同的结果序列。

- **实现**（implementation）：基于 xoshiro256** 算法生成 64 位伪随机整数，通过 `project` 函数将随机数无偏映射到指定区间。Key source files: `lmathlib.c:math_random`、`lmathlib.c:nextrand`、`lmathlib.c:project`。

### math.randomseed ([x [, y]])

When called with at least one argument, the integer parameters x and y are joined into a 128-bit seed that is used to reinitialize the pseudo-random generator; equal seeds produce equal sequences of numbers. The default for y is zero.
使用至少一个参数调用时，整数参数 `x` 和 `y` 连接成一个 128 位**种子**，用于重新初始化伪随机生成器；相等的种子产生相等的数字序列。`y` 的默认值为零。

When called with no arguments, Lua generates a seed with a weak attempt for randomness.
不带参数调用时，Lua 以弱的随机尝试生成种子。

This function returns the two seed components that were effectively used, so that setting them again repeats the sequence.
此函数返回实际使用的两个种子组件，以便再次设置它们可以重复序列。

To ensure a required level of randomness to the initial state (or contrarily, to have a deterministic sequence, for instance when debugging a program), you should call math.randomseed with explicit arguments.
要确保初始状态所需的随机性水平（或者相反，要获得确定性序列，例如在调试程序时），你应该使用显式参数调用 `math.randomseed`。

- **实现**（implementation）：将 `x` 和 `y` 组合为 128 位种子初始化内部状态，无参数时使用当前时间和 `lua_State` 地址生成弱随机种子。Key source files: `lmathlib.c:math_randomseed`、`lmathlib.c:setseed`、`lmathlib.c:randseed`。

### math.sin (x)

Returns the sine of x (assumed to be in radians).
返回 `x` 的正弦（假设以弧度为单位）。

### math.sqrt (x)

Returns the square root of x. (You can also use the expression x^0.5 to compute this value.)
返回 `x` 的平方根。（你也可以使用表达式 `x^0.5` 来计算此值。）

### math.tan (x)

Returns the tangent of x (assumed to be in radians).
返回 `x` 的正切（假设以弧度为单位）。

### math.tointeger (x)

If the value x is convertible to an integer, returns that integer. Otherwise, returns fail.
如果值 `x` 可转换为整数，则返回该整数。否则，返回 `fail`。

### math.type (x)

Returns "integer" if x is an integer, "float" if it is a float, or fail if x is not a number.
如果 `x` 是整数，则返回 `"integer"`；如果它是浮点数，则返回 `"float"`；如果 `x` 不是数字，则返回 `fail`。

- **实现**（implementation）：直接检查栈上值的类型标签，区分 `LUA_TNUMBER` 下的整数和浮点子类型。Key source files: `lmathlib.c:math_type`。

### math.ult (m, n)

Returns a boolean, true if and only if integer m is below integer n when they are compared as unsigned integers.
返回布尔值，当且仅当整数 `m` 和 `n` 作为无符号整数比较时 `m` 低于 `n` 时为 `true`。


## 6.8 – Input and Output Facilities（输入和输出工具）

The I/O library provides two different styles for file manipulation. The first one uses implicit file handles; that is, there are operations to set a default input file and a default output file, and all input/output operations are done over these default files. The second style uses explicit file handles.
I/O 库提供了两种不同的文件操作风格。第一种使用隐式文件句柄；也就是说，有设置默认输入文件和默认输出文件的操作，所有输入/输出操作都在这些默认文件上完成。第二种风格使用显式文件句柄。

When using implicit file handles, all operations are supplied by table io. When using explicit file handles, the operation io.open returns a file handle and then all operations are supplied as methods of the file handle.
使用隐式文件句柄时，所有操作都由表 `io` 提供。使用显式文件句柄时，`io.open` 操作返回文件句柄，然后所有操作都作为文件句柄的方法提供。

The metatable for file handles provides metamethods for __gc and __close that try to close the file when called.
文件句柄的元表为 `__gc` 和 `__close` 提供元方法，尝试在调用时关闭文件。

The table io also provides three predefined file handles with their usual meanings from C: io.stdin, io.stdout, and io.stderr. The I/O library never closes these files.
表 `io` 还提供三个预定义的文件句柄，具有来自 C 的通常含义：`io.stdin`、`io.stdout` 和 `io.stderr`。I/O 库从不关闭这些文件。

Unless otherwise stated, all I/O functions return fail on failure, plus an error message as a second result and a system-dependent error code as a third result, and some non-false value on success. On non-POSIX systems, the computation of the error message and error code in case of errors may be not thread safe, because they rely on the global C variable errno.
除非另有说明，否则所有 I/O 函数在失败时返回 `fail`，加错误消息作为第二个结果，以及系统相关的错误代码作为第三个结果，在成功时返回某个非假值。在非 POSIX 系统上，错误情况下错误消息和错误代码的计算可能不是线程安全的，因为它们依赖于全局 **C 变量** `errno`。

### io.close ([file])

Equivalent to file:close(). Without a file, closes the default output file.
等价于 `file:close()`。没有 `file` 时，关闭默认输出文件。

### io.flush ()

Equivalent to io.output():flush().
等价于 `io.output():flush()`。

### io.input ([file])

When called with a file name, it opens the named file (in text mode), and sets its handle as the default input file. When called with a file handle, it simply sets this file handle as the default input file. When called without arguments, it returns the current default input file.
使用文件名调用时，它打开命名文件（在文本模式下），并将其句柄设置为默认输入文件。使用文件句柄调用时，它只是将此文件句柄设置为默认输入文件。不带参数调用时，它返回当前的默认输入文件。

In case of errors this function raises the error, instead of returning an error code.
在出错的情况下，此函数引发错误，而不是返回错误代码。

### io.lines ([filename, ...])

Opens the given file name in read mode and returns an iterator function that works like file:lines(...) over the opened file. When the iterator function fails to read any value, it automatically closes the file. Besides the iterator function, io.lines returns three other values: two nil values as placeholders, plus the created file handle. Therefore, when used in a generic for loop, the file is closed also if the loop is interrupted by an error or a break.
以读取模式打开给定文件名，并返回迭代器函数，该函数在打开的文件上类似于 `file:lines(...)` 工作。当迭代器函数无法读取任何值时，它会自动关闭文件。除了迭代器函数之外，`io.lines` 还返回三个其他值：两个 `nil` 值作为占位符，加上创建的文件句柄。因此，当在泛型 `for` 循环中使用时，如果循环被错误或 `break` 中断，文件也会关闭。

The call io.lines() (with no file name) is equivalent to io.input():lines("l"); that is, it iterates over the lines of the default input file. In this case, the iterator does not close the file when the loop ends.
调用 `io.lines()`（没有文件名）等价于 `io.input():lines("l")`；也就是说，它遍历默认输入文件的行。在这种情况下，迭代器在循环结束时不会关闭文件。

In case of errors opening the file, this function raises the error, instead of returning an error code.
在打开文件时出错的情况下，此函数引发错误，而不是返回错误代码。

- **实现**（implementation）：通过 `aux_lines` 创建 C 闭包 `io_readline`，迭代结束或出错时自动关闭文件，支持作为泛型 `for` 的 to-be-closed 变量。Key source files: `liolib.c:io_lines`、`liolib.c:aux_lines`、`liolib.c:io_readline`。

### io.open (filename [, mode])

This function opens a file, in the mode specified in the string mode. In case of success, it returns a new file handle.
此函数以字符串 `mode` 中指定的模式打开文件。成功时，它返回新的文件句柄。

The mode string can be any of the following:
`mode` 字符串可以是以下任意一种：

- **"r"**：读取模式（默认）；
- **"w"**：写入模式；
- **"a"**：追加模式；
- **"r+"**：更新模式，保留所有先前数据；
- **"w+"**：更新模式，删除所有先前数据；
- **"a+"**：追加更新模式，保留先前数据，只允许在文件末尾写入。

The mode string can also have a 'b' at the end, which is needed in some systems to open the file in binary mode.
`mode` 字符串末尾还可以有 `'b'`，在某些系统中需要它以二进制模式打开文件。

- **实现**（implementation）：通过 `newfile` 创建文件句柄，调用 `fopen` 打开文件，并使用 `luaL_fileresult` 处理错误。Key source files: `liolib.c:io_open`、`liolib.c:newfile`。

### io.output ([file])

Similar to io.input, but operates over the default output file.
类似于 `io.input`，但操作默认输出文件。

### io.popen (prog [, mode])

This function is system dependent and is not available on all platforms.
此函数依赖于系统，并非在所有平台上都可用。

Starts the program prog in a separated process and returns a file handle that you can use to read data from this program (if mode is "r", the default) or to write data to this program (if mode is "w").
在单独的进程中启动程序 `prog`，并返回文件句柄，你可以使用它从此程序读取数据（如果 `mode` 为 `"r"`，默认值）或向此程序写入数据（如果 `mode` 为 `"w"`）。

- **实现**（implementation）：调用底层 `popen`（POSIX）或 `_popen`（Windows）创建进程管道，关闭函数使用 `io_pclose`。Key source files: `liolib.c:io_popen`、`liolib.c:io_pclose`。

### io.read (...)

Equivalent to io.input():read(...).
等价于 `io.input():read(...)`。

### io.tmpfile ()

In case of success, returns a handle for a temporary file. This file is opened in update mode and it is automatically removed when the program ends.
成功时，返回临时文件的句柄。此文件以更新模式打开，程序结束时自动删除。

### io.type (obj)

Checks whether obj is a valid file handle. Returns the string "file" if obj is an open file handle, "closed file" if obj is a closed file handle, or fail if obj is not a file handle.
检查 `obj` 是否是有效的文件句柄。如果 `obj` 是打开的文件句柄，则返回字符串 `"file"`；如果 `obj` 是关闭的文件句柄，则返回 `"closed file"`；如果 `obj` 不是文件句柄，则返回 `fail`。

### io.write (...)

Equivalent to io.output():write(...).
等价于 `io.output():write(...)`。

### file:close ()

Closes file. Note that files are automatically closed when their handles are garbage collected, but that takes an unpredictable amount of time to happen.
关闭 `file`。请注意，当文件句柄被垃圾回收时，文件会自动关闭，但这需要不可预测的时间才能发生。

When closing a file handle created with io.popen, file:close returns the same values returned by os.execute.
关闭使用 `io.popen` 创建的文件句柄时，`file:close` 返回与 `os.execute` 返回的相同值。

- **实现**（implementation）：调用句柄中存储的 `closef` 函数指针，普通文件使用 `fclose`，`popen` 文件使用 `pclose`。Key source files: `liolib.c:f_close`、`liolib.c:aux_close`、`liolib.c:io_fclose`、`liolib.c:io_pclose`。

### file:flush ()

Saves any written data to file.
将所有写入的数据保存到 `file`。

### file:lines (...)

Returns an iterator function that, each time it is called, reads the file according to the given formats. When no format is given, uses "l" as a default. As an example, the construction
返回迭代器函数，每次调用时，根据给定格式读取文件。当没有给定格式时，默认使用 `"l"`。例如，构造

```lua
for c in file:lines(1) do body end
```

will iterate over all characters of the file, starting at the current position. Unlike io.lines, this function does not close the file when the loop ends.
将从当前位置开始遍历文件的所有字符。与 `io.lines` 不同，此函数在循环结束时不会关闭文件。

### file:read (...)

Reads the file file, according to the given formats, which specify what to read. For each format, the function returns a string or a number with the characters read, or fail if it cannot read data with the specified format. (In this latter case, the function does not read subsequent formats.) When called without arguments, it uses a default format that reads the next line (see below).
根据给定格式读取文件 `file`，这些格式指定要读取的内容。对于每种格式，函数返回读取的字符的字符串或数字，或者如果无法以指定格式读取数据，则返回 `fail`。（在后一种情况下，函数不读取后续格式。）不带参数调用时，它使用默认格式读取下一行（见下文）。

The available formats are:
可用格式为：

- **"n"**：读取数字并将其作为浮点数或整数返回，遵循 Lua 的词法约定。（数字可能有前导空格和符号。）此格式始终读取作为数字的有效前缀的最长输入序列；如果该前缀不构成有效数字（例如，空字符串、`"0x"` 或 `"3.4e-"`）或太长（超过 200 个字符），则将其丢弃，格式返回 `fail`。
- **"a"**：从当前位置读取整个文件。在文件末尾，返回空字符串；此格式永远不会失败。
- **"l"**：读取下一行，跳过行尾，在文件末尾返回 `fail`。这是默认格式。
- **"L"**：读取下一行，保留行尾字符（如果有），在文件末尾返回 `fail`。
- **number**：读取最多此字节数的字符串，在文件末尾返回 `fail`。如果 `number` 为零，则不读取任何内容并返回空字符串，或在文件末尾返回 `fail`。

The formats "l" and "L" should be used only for text files.
格式 `"l"` 和 `"L"` 应仅用于文本文件。

- **实现**（implementation）：由 `g_read` 统一处理，根据格式调用 `read_number`、`read_line`、`read_all` 或 `read_chars`，通过 `luaL_Buffer` 管理读取缓冲区。Key source files: `liolib.c:f_read`、`liolib.c:g_read`、`liolib.c:read_line`、`liolib.c:read_number`。

### file:seek ([whence [, offset]])

Sets and gets the file position, measured from the beginning of the file, to the position given by offset plus a base specified by the string whence, as follows:
设置并获取文件位置（从文件开头测量），到 `offset` 加上字符串 `whence` 指定的基座给出的位置，如下所示：

- **"set"**：基座是位置 0（文件开头）；
- **"cur"**：基座是当前位置；
- **"end"**：基座是文件末尾；

In case of success, seek returns the final file position, measured in bytes from the beginning of the file. If seek fails, it returns fail, plus a string describing the error.
成功时，`seek` 返回最终文件位置，以从文件开头测量的字节为单位。如果 `seek` 失败，则返回 `fail` 加描述错误的字符串。

The default value for whence is "cur", and for offset is 0. Therefore, the call file:seek() returns the current file position, without changing it; the call file:seek("set") sets the position to the beginning of the file (and returns 0); and the call file:seek("end") sets the position to the end of the file, and returns its size.
`whence` 的默认值为 `"cur"`，`offset` 的默认值为 0。因此，调用 `file:seek()` 返回当前文件位置，而不更改它；调用 `file:seek("set")` 将位置设置为文件开头（并返回 0）；调用 `file:seek("end")` 将位置设置为文件末尾，并返回其大小。

### file:setvbuf (mode [, size])

Sets the buffering mode for a file. There are three available modes:
设置文件的缓冲模式。有三种可用模式：

- **"no"**：无缓冲。
- **"full"**：全缓冲。
- **"line"**：行缓冲。

For the last two cases, size is a hint for the size of the buffer, in bytes. The default is an appropriate size.
对于最后两种情况，`size` 是缓冲区大小的提示，以字节为单位。默认是适当的大小。

The specific behavior of each mode is non portable; check the underlying ANSI setvbuf in your platform for more details.
每种模式的具体行为是不可移植的；有关更多详细信息，请检查你平台上的底层 ANSI `setvbuf`。

### file:write (...)

Writes the value of each of its arguments to file. The arguments must be strings or numbers.
将其每个参数的值写入 `file`。参数必须是字符串或数字。

In case of success, this function returns file.
成功时，此函数返回 `file`。

- **实现**（implementation）：由 `g_write` 统一处理，字符串调用 `fwrite`，数字调用 `fprintf`，使用 `LUA_INTEGER_FMT` 和 `LUA_NUMBER_FMT` 格式化。Key source files: `liolib.c:f_write`、`liolib.c:g_write`。

## 6.9 – Operating System Facilities（操作系统工具）

This library is implemented through table os.
此库通过表 `os` 实现。

### os.clock ()

Returns an approximation of the amount in seconds of CPU time used by the program, as returned by the underlying ANSI clock.
返回程序使用的 CPU 时间量（以秒为单位）的近似值，由底层 ANSI `clock` 返回。

### os.date ([format [, time]])

Returns a string or a table containing date and time, formatted according to the given string format.
返回包含日期和时间的字符串或表，根据给定字符串 `format` 进行格式化。

If the time argument is present, this is the time to be formatted (see the os.time function for a description of this value). Otherwise, date formats the current time.
如果存在 `time` 参数，则这是要格式化的时间（有关此值的描述，请参见 `os.time` 函数）。否则，`date` 格式化当前时间。

If format starts with '!', then the date is formatted in Coordinated Universal Time. After this optional character, if format is the string "*t", then date returns a table with the following fields: year, month (1–12), day (1–31), hour (0–23), min (0–59), sec (0–61, due to leap seconds), wday (weekday, 1–7, Sunday is 1), yday (day of the year, 1–366), and isdst (daylight saving flag, a boolean). This last field may be absent if the information is not available.
如果 `format` 以 `'!'` 开头，则日期以协调世界时格式化。在这个可选字符之后，如果 `format` 是字符串 `"*t"`，则 `date` 返回具有以下字段的表：`year`、`month`（1–12）、`day`（1–31）、`hour`（0–23）、`min`（0–59）、`sec`（0–61，由于闰秒）、`wday`（星期几，1–7，星期日为 1）、`yday`（一年中的第几天，1–366）和 `isdst`（夏令时标志，布尔值）。如果信息不可用，则最后一个字段可能缺失。

If format is not "*t", then date returns the date as a string, formatted according to the same rules as the ANSI strftime.
如果 `format` 不是 `"*t"`，则 `date` 按照与 ANSI `strftime` 相同的规则将日期格式化为字符串返回。

If format is absent, it defaults to "%c", which gives a human-readable date and time representation using the current locale.
如果 `format` 缺失，则默认为 `"%c"`，它使用当前区域设置给出人类可读的日期和时间表示。

On non-POSIX systems, this function may be not thread safe because of its reliance on gmtime and localtime.
在非 POSIX 系统上，由于其依赖于 `gmtime` 和 `localtime`，此函数可能不是线程安全的。

- **实现**（implementation）：`"*t"` 格式时通过 `setallfields` 构建表，否则逐字符解析格式说明符并调用 `strftime`。Key source files: `loslib.c:os_date`、`loslib.c:setallfields`、`loslib.c:checkoption`。

### os.difftime (t2, t1)

Returns the difference, in seconds, from time t1 to time t2 (where the times are values returned by os.time). In POSIX, Windows, and some other systems, this value is exactly t2-t1.
返回从时间 `t1` 到时间 `t2` 的差值（以秒为单位）（其中时间是 `os.time` 返回的值）。在 **POSIX**、**Windows** 和其他一些系统上，此值恰好为 `t2-t1`。

### os.execute ([command])

This function is equivalent to the ANSI system. It passes command to be executed by an operating system shell. Its first result is true if the command terminated successfully, or fail otherwise. After this first result the function returns a string plus a number, as follows:
此函数等价于 ANSI `system`。它将 `command` 传递给操作系统 shell 执行。如果命令成功终止，则其第一个结果为 `true`，否则为 `fail`。在此第一个结果之后，函数返回字符串加数字，如下所示：

- **"exit"**：命令正常终止；后面的数字是命令的退出状态。
- **"signal"**：命令被信号终止；后面的数字是终止命令的信号。

When called without a command, os.execute returns a boolean that is true if a shell is available.
不带 `command` 调用时，如果 shell 可用，`os.execute` 返回 `true` 的布尔值。

- **实现**（implementation）：调用底层 C 函数 `system`，通过 `luaL_execresult` 解析返回状态以区分正常退出和信号终止。Key source files: `loslib.c:os_execute`。

### os.exit ([code [, close]])

Calls the ANSI exit to terminate the host program. If code is true, the returned status is EXIT_SUCCESS; if code is false, the returned status is EXIT_FAILURE; if code is a number, the returned status is this number. The default value for code is true.
调用 ANSI `exit` 以终止宿主程序。如果 `code` 为 `true`，则返回的状态为 `EXIT_SUCCESS`；如果 `code` 为 `false`，则返回的状态为 `EXIT_FAILURE`；如果 `code` 是数字，则返回的状态为此数字。`code` 的默认值为 `true`。

If the optional second argument close is true, the function closes the Lua state before exiting (see lua_close).
如果可选的第二个参数 `close` 为 `true`，则函数在退出之前关闭 Lua 状态（见 `lua_close`）。

- **实现**（implementation）：根据参数类型确定退出状态码，可选地调用 `lua_close` 关闭状态机，最后调用 C 标准库 `exit`。Key source files: `loslib.c:os_exit`。

### os.getenv (varname)

Returns the value of the process environment variable varname or fail if the variable is not defined.
返回进程环境变量 `varname` 的值，如果变量未定义，则返回 `fail`。

### os.remove (filename)

Deletes the file (or empty directory, on POSIX systems) with the given name. If this function fails, it returns fail plus a string describing the error and the error code. Otherwise, it returns true.
删除具有给定名称的文件（或在 **POSIX** 系统上为空目录）。如果此函数失败，则返回 `fail` 加描述错误的字符串和错误代码。否则，它返回 `true`。

- **实现**（implementation）：直接调用 C 标准库 `remove`，通过 `luaL_fileresult` 包装返回结果。Key source files: `loslib.c:os_remove`。

### os.rename (oldname, newname)

Renames the file or directory named oldname to newname. If this function fails, it returns fail, plus a string describing the error and the error code. Otherwise, it returns true.
将名为 `oldname` 的文件或目录重命名为 `newname`。如果此函数失败，则返回 `fail` 加描述错误的字符串和错误代码。否则，它返回 `true`。

- **实现**（implementation）：直接调用 C 标准库 `rename`，通过 `luaL_fileresult` 包装返回结果。Key source files: `loslib.c:os_rename`。

### os.setlocale (locale [, category])

Sets the current locale of the program. locale is a system-dependent string specifying a locale; category is an optional string describing which category to change: "all", "collate", "ctype", "monetary", "numeric", or "time"; the default category is "all". The function returns the name of the new locale, or fail if the request cannot be honored.
设置程序的当前区域设置。`locale` 是指定区域设置的系统相关字符串；`category` 是描述要更改哪个类别的可选字符串：`"all"`、`"collate"`、`"ctype"`、`"monetary"`、`"numeric"` 或 `"time"`；默认类别为 `"all"`。函数返回新区域设置的名称，如果无法满足请求，则返回 `fail`。

If locale is the empty string, the current locale is set to an implementation-defined native locale. If locale is the string "C", the current locale is set to the standard C locale.
如果 `locale` 为空字符串，则当前区域设置设置为实现定义的本机区域设置。如果 `locale` 为字符串 `"C"`，则当前区域设置设置为标准 C 区域设置。

When called with nil as the first argument, this function only returns the name of the current locale for the given category.
使用 `nil` 作为第一个参数调用时，此函数仅返回给定类别的当前区域设置的名称。

This function may be not thread safe because of its reliance on setlocale.
由于其依赖于 `setlocale`，此函数可能不是线程安全的。

### os.time ([table])

Returns the current time when called without arguments, or a time representing the local date and time specified by the given table. This table must have fields year, month, and day, and may have fields hour (default is 12), min (default is 0), sec (default is 0), and isdst (default is nil). Other fields are ignored. For a description of these fields, see the os.date function.
不带参数调用时返回当前时间，或返回表示给定表指定的本地日期和时间的时间。此表必须具有字段 `year`、`month` 和 `day`，并且可能具有字段 `hour`（默认值为 12）、`min`（默认值为 0）、`sec`（默认值为 0）和 `isdst`（默认值为 `nil`）。其他字段被忽略。有关这些字段的描述，请参见 `os.date` 函数。

When the function is called, the values in these fields do not need to be inside their valid ranges. For instance, if sec is -10, it means 10 seconds before the time specified by the other fields; if hour is 1000, it means 1000 hours after the time specified by the other fields.
调用函数时，这些字段中的值不需要在其有效范围内。例如，如果 `sec` 为 -10，则表示其他字段指定的时间之前 10 秒；如果 `hour` 为 1000，则表示其他字段指定的时间之后 1000 小时。

The returned value is a number, whose meaning depends on your system. In POSIX, Windows, and some other systems, this number counts the number of seconds since some given start time (the "epoch"). In other systems, the meaning is not specified, and the number returned by time can be used only as an argument to os.date and os.difftime.
返回值是一个数字，其含义取决于你的系统。在 **POSIX**、**Windows** 和其他一些系统上，此数字计算自某个给定开始时间（"纪元"）以来的秒数。在其他系统上，含义未指定，`time` 返回的数字只能用作 `os.date` 和 `os.difftime` 的参数。

When called with a table, os.time also normalizes all the fields documented in the os.date function, so that they represent the same time as before the call but with values inside their valid ranges.
使用表调用时，`os.time` 还规范化 `os.date` 函数中记录的所有字段，以便它们表示与调用之前相同的时间，但值在其有效范围内。

- **实现**（implementation）：无参数时直接调用 `time(NULL)`，有参数时从表中提取字段构造 `struct tm` 并调用 `mktime`。Key source files: `loslib.c:os_time`、`loslib.c:getfield`、`loslib.c:setallfields`。

### os.tmpname ()

Returns a string with a file name that can be used for a temporary file. The file must be explicitly opened before its use and explicitly removed when no longer needed.
返回可用于临时文件的文件名字符串。文件必须在使用前显式打开，并在不再需要时显式删除。

In POSIX systems, this function also creates a file with that name, to avoid security risks. (Someone else might create the file with wrong permissions in the time between getting the name and creating the file.) You still have to open the file to use it and to remove it (even if you do not use it).
在 **POSIX** 系统上，此函数还会创建具有该名称的文件，以避免安全风险。（其他人可能会在获取名称和创建文件之间的时间内以错误的权限创建文件。）你仍然必须打开文件才能使用它并删除它（即使你不使用它）。

When possible, you may prefer to use io.tmpfile, which automatically removes the file when the program ends.
如果可能，你可能更愿意使用 `io.tmpfile`，它在程序结束时自动删除文件。

## 6.10 – The Debug Library（调试库）

This library provides the functionality of the debug interface (see §4.7) to Lua programs. You should exert care when using this library. Several of its functions violate basic assumptions about Lua code (e.g., that variables local to a function cannot be accessed from outside; that userdata metatables cannot be changed by Lua code; that Lua programs do not crash) and therefore can compromise otherwise secure code. Moreover, some functions in this library may be slow.
此库为 Lua 程序提供调试接口（见 §4.7）的功能。使用此库时应小心谨慎。它的几个函数违反了关于 Lua 代码的基本假设（例如，函数的局部变量不能从外部访问；userdata 的元表不能由 Lua 代码更改；Lua 程序不会崩溃），因此可能会损害原本安全的代码。此外，此库中的某些函数可能很慢。

All functions in this library are provided inside the debug table. All functions that operate over a thread have an optional first argument which is the thread to operate over. The default is always the current thread.
此库中的所有函数都在 `debug` 表中提供。所有在线程上操作的函数都有一个可选的第一个参数，即要操作的线程。默认值始终是当前线程。

### debug.debug ()

Enters an interactive mode with the user, running each string that the user enters. Using simple commands and other debug facilities, the user can inspect global and local variables, change their values, evaluate expressions, and so on. A line containing only the word cont finishes this function, so that the caller continues its execution.
与用户进入交互模式，运行用户输入的每个字符串。使用简单命令和其他调试工具，用户可以检查全局和局部变量，更改其值，计算表达式等。仅包含单词 `cont` 的行完成此函数，以便调用者继续执行。

Note that commands for debug.debug are not lexically nested within any function and so have no direct access to local variables.
请注意，`debug.debug` 的命令在词法上不嵌套在任何函数中，因此不能直接访问局部变量。

### debug.gethook ([thread])

Returns the current hook settings of the thread, as three values: the current hook function, the current hook mask, and the current hook count, as set by the debug.sethook function.
以三个值返回线程的当前钩子设置：当前钩子函数、当前钩子掩码和当前钩子计数，如 `debug.sethook` 函数设置。

Returns fail if there is no active hook.
如果没有活动钩子，则返回 `fail`。

### debug.getinfo ([thread,] f [, what])

Returns a table with information about a function. You can give the function directly or you can give a number as the value of f, which means the function running at level f of the call stack of the given thread: level 0 is the current function (getinfo itself); level 1 is the function that called getinfo (except for tail calls, which do not count in the stack); and so on. If f is a number greater than the number of active functions, then getinfo returns fail.
返回包含函数信息的表。你可以直接给出函数，也可以给出数字作为 `f` 的值，这意味着在给定线程的调用栈的级别 `f` 处运行的函数：级别 0 是当前函数（`getinfo` 本身）；级别 1 是调用 `getinfo` 的函数（尾调用除外，它们在栈中不计数）；依此类推。如果 `f` 是大于活动函数数量的数字，则 `getinfo` 返回 `fail`。

The returned table can contain all the fields returned by lua_getinfo, with the string what describing which fields to fill in. The default for what is to get all information available, except the table of valid lines. The option 'f' adds a field named func with the function itself. The option 'L' adds a field named activelines with the table of valid lines, provided the function is a Lua function. If the function has no debug information, the table is empty.
返回的表可以包含 `lua_getinfo` 返回的所有字段，字符串 `what` 描述要填充哪些字段。`what` 的默认值是获取所有可用信息，除了有效行表。选项 `'f'` 添加名为 `func` 的字段，其中包含函数本身。选项 `'L'` 添加名为 `activelines` 的字段，其中包含有效行表，前提是函数是 Lua 函数。如果函数没有调试信息，则表为空。

For instance, the expression debug.getinfo(1,"n").name returns a name for the current function, if a reasonable name can be found, and the expression debug.getinfo(print) returns a table with all available information about the print function.
例如，表达式 `debug.getinfo(1,"n").name` 返回当前函数的名称，如果找不到合理的名称，则表达式 `debug.getinfo(print)` 返回包含有关 `print` 函数的所有可用信息的表。

- **实现**（implementation）：调用底层 `lua_getinfo` 获取调试信息，根据 `what` 选项通过 `settabss`/`settabsi`/`settabsb` 填充结果表。Key source files: `ldblib.c:db_getinfo`。

### debug.getlocal ([thread,] f, local)

This function returns the name and the value of the local variable with index local of the function at level f of the stack. This function accesses not only explicit local variables, but also parameters and temporary values.
此函数返回栈中级别 `f` 处函数的索引 `local` 的局部变量的名称和值。此函数不仅访问显式局部变量，还访问参数和临时值。

The first parameter or local variable has index 1, and so on, following the order that they are declared in the code, counting only the variables that are active in the current scope of the function. Compile-time constants may not appear in this listing, if they were optimized away by the compiler. Negative indices refer to vararg arguments; -1 is the first vararg argument. The function returns fail if there is no variable with the given index, and raises an error when called with a level out of range. (You can call debug.getinfo to check whether the level is valid.)
第一个参数或局部变量的索引为 1，依此类推，按照它们在代码中声明的顺序，只计算函数当前作用域中活动的变量。如果编译时常量被编译器优化掉，则可能不会出现在此列表中。负索引指变长参数；`-1` 是第一个变长参数。如果没有具有给定索引的变量，则函数返回 `fail`，当用超出范围的级别调用时引发错误。（你可以调用 `debug.getinfo` 来检查级别是否有效。）

Variable names starting with '(' (open parenthesis) represent variables with no known names (internal variables such as loop control variables, and variables from chunks saved without debug information).
以 `'('`（左括号）开头的变量名称表示没有已知名称的变量（内部变量，例如循环控制变量，以及从不带调试信息保存的代码块中的变量）。

The parameter f may also be a function. In that case, getlocal returns only the name of function parameters.
参数 `f` 也可以是函数。在这种情况下，`getlocal` 只返回函数参数的名称。

- **实现**（implementation）：对栈级别调用 `lua_getstack` 获取 `lua_Debug`，再通过 `lua_getlocal` 读取变量名和值；对函数参数则直接调用 `lua_getlocal`。Key source files: `ldblib.c:db_getlocal`。

### debug.getmetatable (value)

Returns the metatable of the given value or nil if it does not have a metatable.
返回给定值的元表，如果它没有元表，则返回 `nil`。

### debug.getregistry ()

Returns the registry table (see §4.3).
返回注册表（见 §4.3）。

### debug.getupvalue (f, up)

This function returns the name and the value of the upvalue with index up of the function f. The function returns fail if there is no upvalue with the given index.
此函数返回函数 `f` 的索引 `up` 的上值的名称和值。如果没有具有给定索引的上值，则函数返回 `fail`。

(For Lua functions, upvalues are the external local variables that the function uses, and that are consequently included in its closure.)
（对于 Lua 函数，上值是函数使用的外部局部变量，因此包含在其闭包中。）

For C functions, this function uses the empty string "" as a name for all upvalues.
对于 **C 函数**，此函数使用空字符串 `""` 作为所有上值的名称。

Variable name '?' (interrogation mark) represents variables with no known names (variables from chunks saved without debug information).
变量名称 `'?'`（问号）表示没有已知名称的变量（来自不带调试信息保存的代码块的变量）。

### debug.getuservalue (u, n)

Returns the n-th user value associated to the userdata u plus a boolean, false if the userdata does not have that value.
返回与 userdata `u` 关联的第 `n` 个用户值加一个布尔值，如果 userdata 没有该值，则为 `false`。

### debug.sethook ([thread,] hook, mask [, count])

Sets the given function as the debug hook. The string mask and the number count describe when the hook will be called. The string mask may have any combination of the following characters, with the given meaning:
将给定函数设置为调试钩子。字符串 `mask` 和数字 `count` 描述何时调用钩子。字符串 `mask` 可以具有以下字符的任意组合，具有给定含义：

- **'c'**：每次 Lua 调用函数时调用钩子；
- **'r'**：每次 Lua 从函数返回时调用钩子；
- **'l'**：每次 Lua 进入新代码行时调用钩子。

Moreover, with a count different from zero, the hook is called also after every count instructions.
此外，当 `count` 不为零时，在每 `count` 条指令后也会调用钩子。

When called without arguments, debug.sethook turns off the hook.
不带参数调用时，`debug.sethook` 关闭钩子。

When the hook is called, its first parameter is a string describing the event that has triggered its call: "call", "tail call", "return", "line", and "count". For line events, the hook also gets the new line number as its second parameter. Inside a hook, you can call getinfo with level 2 to get more information about the running function. (Level 0 is the getinfo function, and level 1 is the hook function.)
调用钩子时，其第一个参数是描述触发其调用的事件的字符串：`"call"`、`"tail call"`、`"return"`、`"line"` 和 `"count"`。对于行事件，钩子还获得新行号作为其第二个参数。在钩子内部，你可以使用级别 2 调用 `getinfo` 以获取有关运行函数的更多信息。（级别 0 是 `getinfo` 函数，级别 1 是钩子函数。）

### debug.setlocal ([thread,] level, local, value)

This function assigns the value value to the local variable with index local of the function at level level of the stack. The function returns fail if there is no local variable with the given index, and raises an error when called with a level out of range. (You can call getinfo to check whether the level is valid.) Otherwise, it returns the name of the local variable.
此函数将值 `value` 赋给栈中级别 `level` 处函数的索引 `local` 的局部变量。如果没有具有给定索引的局部变量，则函数返回 `fail`，当用超出范围的级别调用时引发错误。（你可以调用 `getinfo` 来检查级别是否有效。）否则，它返回局部变量的名称。

See debug.getlocal for more information about variable indices and names.
有关变量索引和名称的更多信息，请参见 `debug.getlocal`。

- **实现**（implementation）：通过 `lua_getstack` 定位栈帧，将值 `lua_xmove` 到目标线程，再调用 `lua_setlocal` 赋值。Key source files: `ldblib.c:db_setlocal`。

### debug.setmetatable (value, table)

Sets the metatable for the given value to the given table (which can be nil). Returns value.
将给定值的元表设置为给定表（可以为 `nil`）。返回 `value`。

### debug.setupvalue (f, up, value)

This function assigns the value value to the upvalue with index up of the function f. The function returns fail if there is no upvalue with the given index. Otherwise, it returns the name of the upvalue.
此函数将值 `value` 赋给函数 `f` 的索引 `up` 的上值。如果没有具有给定索引的上值，则函数返回 `fail`。否则，它返回上值的名称。

See debug.getupvalue for more information about upvalues.
有关上值的更多信息，请参见 `debug.getupvalue`。

### debug.setuservalue (udata, value, n)

Sets the given value as the n-th user value associated to the given udata. udata must be a full userdata.
将给定值设置为给定 `udata` 的第 `n` 个用户值。`udata` 必须是完整的 userdata。

Returns udata, or fail if the userdata does not have that value.
返回 `udata`，如果 userdata 没有该值，则返回 `fail`。

### debug.traceback ([thread,] [message [, level]])

If message is present but is neither a string nor nil, this function returns message without further processing. Otherwise, it returns a string with a traceback of the call stack. The optional message string is appended at the beginning of the traceback. An optional level number tells at which level to start the traceback (default is 1, the function calling traceback).
如果 `message` 存在但既不是字符串也不是 `nil`，则此函数返回 `message` 而不进行进一步处理。否则，它返回带有调用栈回溯的字符串。可选的 `message` 字符串附加到回溯的开头。可选的 `level` 数字指示从哪个级别开始回溯（默认值为 1，即调用 `traceback` 的函数）。

- **实现**（implementation）：通过 `getthread` 获取目标线程，调用 `luaL_traceback` 生成调用栈回溯字符串。Key source files: `ldblib.c:db_traceback`。

### debug.upvalueid (f, n)

Returns a unique identifier (as a light userdata) for the upvalue numbered n from the given function.
返回给定函数的第 `n` 个上值的唯一标识符（作为轻量 userdata）。

These unique identifiers allow a program to check whether different closures share upvalues. Lua closures that share an upvalue (that is, that access a same external local variable) will return identical ids for those upvalue indices.
这些唯一标识符允许程序检查不同的闭包是否共享上值。共享上值的 Lua 闭包（即访问相同的外部局部变量）将为这些上值索引返回相同的 ID。

### debug.upvaluejoin (f1, n1, f2, n2)

Make the n1-th upvalue of the Lua closure f1 refer to the n2-th upvalue of the Lua closure f2.
使 Lua 闭包 `f1` 的第 `n1` 个上值引用 Lua 闭包 `f2` 的第 `n2` 个上值。

---

# 7 – Lua Standalone（Lua 独立解释器）

Although Lua has been designed as an extension language, to be embedded in a host C program, it is also frequently used as a standalone language. An interpreter for Lua as a standalone language, called simply lua, is provided with the standard distribution. The standalone interpreter includes all standard libraries.
尽管 Lua 被设计为扩展语言，嵌入宿主 **C 程序**中，但它也经常用作独立语言。作为独立语言的 Lua 解释器，简称为 `lua`，随标准发行版提供。**独立解释器**包括所有标准库。

Its usage is:
其用法为：

```
lua [options] [script [args]]
```

The options are:
选项为：

- **-e stat**：执行字符串 `stat`；
- **-i**：运行 `script` 后进入交互模式；
- **-l mod**："require" `mod` 并将结果赋给全局 `mod`；
- **-l g=mod**："require" `mod` 并将结果赋给全局 `g`；
- **-v**：打印版本信息；
- **-E**：忽略环境变量；
- **-W**：打开警告；
- **--**：停止处理选项；
- **-**：将 `stdin` 作为文件执行并停止处理选项。

(The form -l g=mod was introduced in release 5.4.4.)
（形式 `-l g=mod` 在 5.4.4 版本中引入。）

After handling its options, lua runs the given script. When called without arguments, lua behaves as lua -v -i when the standard input (stdin) is a terminal, and as lua - otherwise.
处理其选项后，`lua` 运行给定的 `script`。不带参数调用时，当标准输入（`stdin`）是终端时，`lua` 表现为 `lua -v -i`，否则表现为 `lua -`。

When called without the option -E, the interpreter checks for an environment variable LUA_INIT_5_4 (or LUA_INIT if the versioned name is not defined) before running any argument. If the variable content has the format @filename, then lua executes the file. Otherwise, lua executes the string itself.
不带选项 `-E` 调用时，解释器在运行任何参数之前检查环境变量 `LUA_INIT_5_4`（如果未定义版本化名称，则为 `LUA_INIT`）。如果变量内容具有格式 `@filename`，则 `lua` 执行文件。否则，`lua` 执行字符串本身。

When called with the option -E, Lua does not consult any environment variables. In particular, the values of package.path and package.cpath are set with the default paths defined in luaconf.h. To signal to the libraries that this option is on, the stand-alone interpreter sets the field "LUA_NOENV" in the registry to a true value. Other libraries may consult this field for the same purpose.
使用选项 `-E` 调用时，Lua 不查询任何环境变量。特别地，`package.path` 和 `package.cpath` 的值使用 `luaconf.h` 中定义的默认路径设置。为了向库发出此选项已打开的信号，独立解释器将注册表中的字段 `"LUA_NOENV"` 设置为 `true`。其他库可以出于相同目的查询此字段。

The options -e, -l, and -W are handled in the order they appear. For instance, an invocation like
选项 `-e`、`-l` 和 `-W` 按照它们出现的顺序处理。例如，调用如

```
$ lua -e 'a=1' -llib1 script.lua
```

will first set a to 1, then require the library lib1, and finally run the file script.lua with no arguments. (Here $ is the shell prompt. Your prompt may be different.)
将首先将 `a` 设置为 1，然后请求库 `lib1`，最后不带参数运行文件 `script.lua`。（这里 `$` 是 shell 提示符。你的提示符可能不同。）

Before running any code, lua collects all command-line arguments in a global table called arg. The script name goes to index 0, the first argument after the script name goes to index 1, and so on. Any arguments before the script name (that is, the interpreter name plus its options) go to negative indices.
在运行任何代码之前，`lua` 将所有命令行参数收集到一个名为 `arg` 的全局表中。脚本名称进入索引 0，脚本名称后的第一个参数进入索引 1，依此类推。脚本名称之前的任何参数（即解释器名称及其选项）进入负索引。

For instance, in the call
例如，在调用中

```
$ lua -la b.lua t1 t2
```

the table is like this:
表如下所示：

```lua
arg = { [-2] = "lua", [-1] = "-la",
        [0] = "b.lua",
        [1] = "t1", [2] = "t2" }
```

If there is no script in the call, the interpreter name goes to index 0, followed by the other arguments. For instance, the call
如果调用中没有脚本，则解释器名称进入索引 0，后跟其他参数。例如，调用

```
$ lua -e "print(arg[1])"
```

will print -e.
将打印 `-e`。

If there is a script, the script is called with arguments arg[1], ..., arg[#arg]. Like all chunks in Lua, the script is compiled as a variadic function.
如果有脚本，则使用参数 `arg[1]`、...、`arg[#arg]` 调用脚本。与 Lua 中的所有代码块一样，脚本被编译为变长函数。

In interactive mode, Lua repeatedly prompts and waits for a line. After reading a line, Lua first try to interpret the line as an expression. If it succeeds, it prints its value. Otherwise, it interprets the line as a statement. If you write an incomplete statement, the interpreter waits for its completion by issuing a different prompt.
在交互模式下，Lua 反复提示并等待一行。读取一行后，Lua 首先尝试将该行解释为表达式。如果成功，它会打印其值。否则，它将该行解释为语句。如果你写了一个不完整的语句，解释器会通过发出不同的提示来等待其完成。

If the global variable _PROMPT contains a string, then its value is used as the prompt. Similarly, if the global variable _PROMPT2 contains a string, its value is used as the secondary prompt (issued during incomplete statements).
如果全局变量 `_PROMPT` 包含字符串，则其值用作提示符。类似地，如果全局变量 `_PROMPT2` 包含字符串，则其值用作辅助提示符（在不完整语句期间发出）。

In case of unprotected errors in the script, the interpreter reports the error to the standard error stream. If the error object is not a string but has a metamethod __tostring, the interpreter calls this metamethod to produce the final message. Otherwise, the interpreter converts the error object to a string and adds a stack traceback to it. When warnings are on, they are simply printed in the standard error output.
在脚本中出现未受保护的错误时，解释器将错误报告给标准错误流。如果错误对象不是字符串但具有元方法 `__tostring`，则解释器调用此元方法来生成最终消息。否则，解释器将错误对象转换为字符串并向其添加栈回溯。当警告打开时，它们只是在标准错误输出中打印。

When finishing normally, the interpreter closes its main Lua state (see lua_close). The script can avoid this step by calling os.exit to terminate.
正常完成时，解释器关闭其主 Lua 状态（见 `lua_close`）。脚本可以通过调用 `os.exit` 终止来避免此步骤。

To allow the use of Lua as a script interpreter in Unix systems, Lua skips the first line of a file chunk if it starts with #. Therefore, Lua scripts can be made into executable programs by using chmod +x and the #! form, as in
为了允许在 Unix 系统中将 Lua 用作脚本解释器，如果文件代码块的第一行以 `#` 开头，Lua 会跳过它。因此，Lua 脚本可以通过使用 `chmod +x` 和 `#!` 形式制成可执行程序，例如

```
#!/usr/local/bin/lua
```

Of course, the location of the Lua interpreter may be different in your machine. If lua is in your PATH, then
当然，Lua 解释器的位置在你的机器上可能不同。如果 `lua` 在你的 `PATH` 中，那么

```
#!/usr/bin/env lua
```

is a more portable solution.
是更便携的解决方案。

---

# 8 – Incompatibilities with the Previous Version（与先前版本的不兼容性）

Here we list the incompatibilities that you may find when moving a program from Lua 5.3 to Lua 5.4.
这里我们列出将程序从 Lua 5.3 移动到 Lua 5.4 时可能发现的不兼容性。

You can avoid some incompatibilities by compiling Lua with appropriate options (see file luaconf.h). However, all these compatibility options will be removed in the future. More often than not, compatibility issues arise when these compatibility options are removed. So, whenever you have the chance, you should try to test your code with a version of Lua compiled with all compatibility options turned off. That will ease transitions to newer versions of Lua.
你可以通过使用适当的选项编译 Lua 来避免一些不兼容性（请参阅文件 `luaconf.h`）。但是，所有这些兼容性选项将来都会被删除。通常情况下，当这些兼容性选项被删除时会出现兼容性问题。因此，只要有机会，你应该尝试使用关闭了所有兼容性选项编译的 Lua 版本测试你的代码。这将有助于过渡到更新版本的 Lua。

Lua versions can always change the C API in ways that do not imply source-code changes in a program, such as the numeric values for constants or the implementation of functions as macros. Therefore, you should never assume that binaries are compatible between different Lua versions. Always recompile clients of the Lua API when using a new version.
Lua 版本总是可以以不需要程序源代码更改的方式更改 **C API**，例如常量的数值或函数作为宏的实现。因此，你绝不应假设不同 Lua 版本之间的二进制文件是兼容的。使用新版本时，始终重新编译 Lua API 的客户端。

Similarly, Lua versions can always change the internal representation of precompiled chunks; precompiled chunks are not compatible between different Lua versions.
类似地，Lua 版本总是可以更改预编译代码块的内部表示；预编译代码块在不同 Lua 版本之间不兼容。

The standard paths in the official distribution may change between versions.
官方发行版中的标准路径可能在版本之间更改。

## 8.1 – Incompatibilities in the Language（语言中的不兼容性）

- The coercion of strings to numbers in arithmetic and bitwise operations has been removed from the core language. The string library does a similar job for arithmetic (but not for bitwise) operations using the string metamethods. However, unlike in previous versions, the new implementation preserves the implicit type of the numeral in the string. For instance, the result of "1" + "2" now is an integer, not a float.
- 算术和按位运算中字符串到数字的强制转换已从核心语言中删除。字符串库使用字符串元方法为算术（但不为按位）运算执行类似的工作。然而，与以前的版本不同，新实现保留了字符串中数字的隐式类型。例如，`"1" + "2"` 的结果现在是整数，而不是浮点数。

- Literal decimal integer constants that overflow are read as floats, instead of wrapping around. You can use hexadecimal notation for such constants if you want the old behavior (reading them as integers with wrap around).
- 溢出的十进制整数字面常量被读取为浮点数，而不是回绕。如果你想要旧的行为（将它们读取为带回绕的整数），你可以使用十六进制表示法来表示此类常量。

- The use of the __lt metamethod to emulate __le has been removed. When needed, this metamethod must be explicitly defined.
- 使用 `__lt` 元方法来模拟 `__le` 已被删除。需要时，必须显式定义此元方法。

- The semantics of the numerical for loop over integers changed in some details. In particular, the control variable never wraps around.
- 整数数字 `for` 循环的语义在某些细节上发生了变化。特别地，控制变量永远不会回绕。

- A label for a goto cannot be declared where a label with the same name is visible, even if this other label is declared in an enclosing block.
- `goto` 的标签不能在具有相同名称的标签可见的地方声明，即使此其他标签在封闭块中声明。

- When finalizing an object, Lua does not ignore __gc metamethods that are not functions. Any value will be called, if present. (Non-callable values will generate a warning, like any other error when calling a finalizer.)
- 在终结对象时，Lua 不会忽略不是函数的 `__gc` 元方法。如果存在，任何值都会被调用。（不可调用的值会产生警告，就像调用终结器时的任何其他错误一样。）

## 8.2 – Incompatibilities in the Libraries（库中的不兼容性）

- The function print does not call tostring to format its arguments; instead, it has this functionality hardwired. You should use __tostring to modify how values are printed.
- 函数 `print` 不调用 `tostring` 来格式化其参数；相反，它将此功能硬连线。你应该使用 `__tostring` 来修改值的打印方式。

- The pseudo-random number generator used by the function math.random now starts with a somewhat random seed. Moreover, it uses a different algorithm.
- 函数 `math.random` 使用的伪随机数生成器现在以某种随机种子开始。此外，它使用不同的算法。

- By default, the decoding functions in the utf8 library do not accept surrogates as valid code points. An extra parameter in these functions makes them more permissive.
- 默认情况下，`utf8` 库中的解码函数不接受代理对作为有效的代码点。这些函数中的额外参数使它们更宽松。

- The options "setpause" and "setstepmul" of the function collectgarbage are deprecated. You should use the new option "incremental" to set them.
- 函数 `collectgarbage` 的选项 `"setpause"` 和 `"setstepmul"` 已弃用。你应该使用新选项 `"incremental"` 来设置它们。

- The function io.lines now returns four values, instead of just one. That can be a problem when it is used as the sole argument to another function that has optional parameters, such as in load(io.lines(filename, "L")). To fix that issue, you can wrap the call into parentheses, to adjust its number of results to one.
- 函数 `io.lines` 现在返回四个值，而不是只有一个。当它是具有可选参数的另一个函数的唯一参数时，这可能是一个问题，例如在 `load(io.lines(filename, "L"))` 中。要解决此问题，你可以将调用包装在括号中，以将其结果数量调整为一个。

## 8.3 – Incompatibilities in the API（API 中的不兼容性）

- Full userdata now has an arbitrary number of associated user values. Therefore, the functions lua_newuserdata, lua_setuservalue, and lua_getuservalue were replaced by lua_newuserdatauv, lua_setiuservalue, and lua_getiuservalue, which have an extra argument. For compatibility, the old names still work as macros assuming one single user value. Note, however, that userdata with zero user values are more efficient memory-wise.
- 完整 userdata 现在具有任意数量的关联用户值。因此，函数 `lua_newuserdata`、`lua_setuservalue` 和 `lua_getuservalue` 被 `lua_newuserdatauv`、`lua_setiuservalue` 和 `lua_getiuservalue` 替换，它们有一个额外的参数。为了兼容性，旧名称仍然作为假设单个用户值的宏工作。但是请注意，具有零个用户值的 userdata 在内存方面更有效。

- The function lua_resume has an extra parameter. This out parameter returns the number of values on the top of the stack that were yielded or returned by the coroutine. (In previous versions, those values were the entire stack.)
- 函数 `lua_resume` 有一个额外的参数。此输出参数返回协程让出或返回的栈顶值的数量。（在以前的版本中，这些值是整个栈。）

- The function lua_version returns the version number, instead of an address of the version number. The Lua core should work correctly with libraries using their own static copies of the same core, so there is no need to check whether they are using the same address space.
- 函数 `lua_version` 返回版本号，而不是版本号的地址。Lua 核心应该与使用相同核心的自己的静态副本的库正确工作，因此无需检查它们是否使用相同的地址空间。

- The constant LUA_ERRGCMM was removed. Errors in finalizers are never propagated; instead, they generate a warning.
- 常量 `LUA_ERRGCMM` 已被删除。终结器中的错误永远不会传播；相反，它们会生成警告。

- The options LUA_GCSETPAUSE and LUA_GCSETSTEPMUL of the function lua_gc are deprecated. You should use the new option LUA_GCINC to set them.
- 函数 `lua_gc` 的选项 `LUA_GCSETPAUSE` 和 `LUA_GCSETSTEPMUL` 已弃用。你应该使用新选项 `LUA_GCINC` 来设置它们。

---

# 9 – The Complete Syntax of Lua（Lua 的完整语法）

Here is the complete syntax of Lua in extended BNF. As usual in extended BNF, {A} means 0 or more As, and [A] means an optional A. (For operator precedences, see §3.4.8; for a description of the terminals Name, Numeral, and LiteralString, see §3.1.)
以下是扩展 BNF 中 Lua 的完整语法。与扩展 BNF 中通常一样，`{A}` 表示 0 个或多个 `A`，`[A]` 表示可选的 `A`。（有关运算符优先级，请参见 §3.4.8；有关终结符 `Name`、`Numeral` 和 `LiteralString` 的描述，请参见 §3.1。）

```bnf
chunk        ::= block

block        ::= {stat} [retstat]

stat         ::= ';' |
                 varlist '=' explist |
                 functioncall |
                 label |
                 break |
                 goto Name |
                 do block end |
                 while exp do block end |
                 repeat block until exp |
                 if exp then block {elseif exp then block} [else block] end |
                 for Name '=' exp ',' exp [',' exp] do block end |
                 for namelist in explist do block end |
                 function funcname funcbody |
                 local function Name funcbody |
                 local attnamelist ['=' explist]

attnamelist  ::= Name attrib {',' Name attrib}

attrib       ::= ['<' Name '>']

retstat      ::= return [explist] [';']

label        ::= '::' Name '::'

funcname     ::= Name {'.' Name} [':' Name]

varlist      ::= var {',' var}

var          ::= Name | prefixexp '[' exp ']' | prefixexp '.' Name

namelist     ::= Name {',' Name}

explist      ::= exp {',' exp}

exp          ::= nil | false | true | Numeral | LiteralString | '...' |
                 functiondef | prefixexp | tableconstructor |
                 exp binop exp | unop exp

prefixexp    ::= var | functioncall | '(' exp ')'

functioncall ::= prefixexp args | prefixexp ':' Name args

args         ::= '(' [explist] ')' | tableconstructor | LiteralString

functiondef  ::= function funcbody

funcbody     ::= '(' [parlist] ')' block end

parlist      ::= namelist [',' '...'] | '...'

tableconstructor ::= '{' [fieldlist] '}'

fieldlist    ::= field {fieldsep field} [fieldsep]

field        ::= '[' exp ']' '=' exp | Name '=' exp | exp

fieldsep     ::= ',' | ';'

binop        ::= '+' | '-' | '*' | '/' | '//' | '^' | '%' |
                 '&' | '~' | '|' | '>>' | '<<' | '..' |
                 '<' | '<=' | '>' | '>=' | '==' | '~=' |
                 and | or

unop         ::= '-' | not | '#' | '~'
```


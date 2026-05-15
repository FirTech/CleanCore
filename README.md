# CleanCore

CleanCore 是一个基于 NT API 的 Windows 内存碎片整理示例程序。

在系统运行过程中，内存中不可避免地会产生一些碎片，这会影响系统的执行效率，因此适时整理内存碎片是有必要的。

市面上很多内存碎片整理工具的做法，是尽可能向系统申请大量内存，再释放这些内存，从而迫使系统把尽可能多的页面挤入交换文件。这个思路可以工作，但并不优雅：

- 它依赖进程自身能够申请到足够多的地址空间和内存。
- 如果工具运行在 WoW 兼容层中，例如 32 位程序，能申请的空间天然受限。
- 它本质上是在“逼迫”系统完成页面迁移，因此效率通常不高。
- 这种方式会对系统调度和内存管理行为带来更明显的干扰。

CleanCore 采用了另一种方式：直接调用 NT API，通知内核主动处理特定内存列表，从而以更直接的方式完成内存碎片整理。相比“疯狂申请内存再释放”的方案，这种做法更简洁，也更贴近系统底层机制。

## 灵感来源

灵感来自 Sysinternals Suite 中的 RAMMap 工具。

在使用 RAMMap 的 `Empty` 菜单相关功能时，可以观察到：按照合适的顺序操作若干内存列表，能够比较高效地整理内存碎片。基于这个思路，对相关行为进行了分析和整理，并实现了这个简化的示例程序。

## 工作原理

程序启动后会执行以下步骤：

1. 启用当前进程的 `SeProfileSingleProcessPrivilege` 特权。
2. 调用 `NtSetSystemInformation`，并向 `SystemMemoryListInformation` 依次传入以下命令：
   - `MemoryEmptyWorkingSets`
   - `MemoryFlushModifiedList`
   - `MemoryPurgeStandbyList`

对应源码可以参考：

- [CleanCore.cpp](./CleanCore/CleanCore.cpp)

简而言之，程序按如下顺序整理内存列表：

- Working Sets
- Modified Page List
- Standby List

## 运行要求

- 需要在 Windows 上运行。
- 需要以管理员身份运行。
- 程序清单文件已声明 `requireAdministrator`，启动时会请求提权。

## 编译

### 方法一：快速编译

在已安装 Visual Studio C++ 构建环境的前提下，直接运行：

```bat
BuildAllTargets.cmd
```

该脚本会：

- 清理 `Output` 目录
- 初始化 Visual Studio 编译环境
- 编译 `Debug/Release` 的 `x64` 和 `ARM64` 目标

### 方法二：使用 Visual Studio 或 MSBuild

也可以直接打开解决方案进行构建：

- [CleanCore.sln](./CleanCore.sln)

### 编译依赖

除 Visual Studio C++ 工具链外，项目还依赖以下内容：

- 项目目录中的 [Mile.Project.Windows](./Mile.Project.Windows)
- 首次构建时可访问 NuGet，用于自动还原 `Mile.Project.Configurations`、`VC-LTL`、`Mile.Windows.Helpers`、`YY-Thunks` 等依赖

首次成功还原后，后续构建通常可以直接复用本地缓存。

### 输出目录

默认构建输出位于：

- `Output/Binaries/Debug/x64`
- `Output/Binaries/Release/x64`
- `Output/Binaries/Debug/ARM64`
- `Output/Binaries/Release/ARM64`

## 注意事项

- 本项目是一个偏底层的系统工具示例，会直接触发系统内存列表操作。
- 它的目标是展示一种基于 NT API 的整理方式，而不是提供复杂的图形界面或附加策略。
- 由于行为依赖系统内部内存管理机制，请在理解用途的前提下使用。

## 许可证

本项目采用 MIT License，详见：

- [LICENSE](./LICENSE.md)

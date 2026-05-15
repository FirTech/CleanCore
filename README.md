# CleanCore

[简体中文](README.zh.md) | English

CleanCore is a Windows memory defragmentation sample program based on NT APIs.

During system runtime, some memory fragmentation is unavoidable. This can affect overall execution efficiency, so defragmenting memory at the right time can be beneficial.

Many memory defragmentation tools on the market work by allocating as much memory as possible and then releasing it, forcing the system to push as many memory pages as possible into the paging file. This approach can work, but it is not very elegant:

- It depends on the process being able to request enough address space and memory.
- If the tool runs under the WoW compatibility layer, for example as a 32-bit process, the available address space is naturally limited.
- It essentially "forces" the system to move pages, so the efficiency is usually not very high.
- This approach can also have a more noticeable impact on system scheduling and memory management behavior.

CleanCore uses another approach: it directly calls NT APIs to notify the kernel to actively process specific memory lists, so memory defragmentation can be completed in a more direct way. Compared with the "allocate a lot and then free it" approach, this method is cleaner and closer to the underlying system mechanism.

## Inspiration

The inspiration comes from the RAMMap tool in Sysinternals Suite.

When using the related functions in RAMMap's `Empty` menu, it can be observed that operating on several memory lists in an appropriate order can defragment memory quite efficiently. Based on this idea, the related behavior was analyzed and summarized, and this simplified sample program was implemented.

## How It Works

After startup, the program performs the following steps:

1. Enable the `SeProfileSingleProcessPrivilege` privilege for the current process.
2. Call `NtSetSystemInformation`, and pass the following commands to `SystemMemoryListInformation` in order:
   - `MemoryEmptyWorkingSets`
   - `MemoryFlushModifiedList`
   - `MemoryPurgeStandbyList`

Relevant source code:

- [CleanCore.cpp](./CleanCore/CleanCore.cpp)

In short, the program processes memory lists in the following order:

- Working Sets
- Modified Page List
- Standby List

## Runtime Requirements

- Windows is required.
- The program must be run as administrator.
- The application manifest already declares `requireAdministrator`, so elevation will be requested on launch.

## Build

### Method 1: Quick Build

If a Visual Studio C++ build environment is installed, simply run:

```bat
BuildAllTargets.cmd
```

This script will:

- Clean the `Output` directory
- Initialize the Visual Studio build environment
- Build `Debug/Release` targets for `x86`, `x64`, and `ARM64`

### Method 2: Use Visual Studio or MSBuild

You can also open the solution directly and build it:

- [CleanCore.sln](./CleanCore.sln)

### Build Dependencies

In addition to the Visual Studio C++ toolchain, the project also depends on:

- [Mile.Project.Windows](./Mile.Project.Windows) in the project directory
- Access to NuGet during the first build, so dependencies such as `Mile.Project.Configurations`, `VC-LTL`, `Mile.Windows.Helpers`, and `YY-Thunks` can be restored automatically

After the first successful restore, subsequent builds can usually reuse the local cache directly.

### Output Directories

By default, build outputs are placed in:

- `Output/Binaries/Debug/x86`
- `Output/Binaries/Release/x86`
- `Output/Binaries/Debug/x64`
- `Output/Binaries/Release/x64`
- `Output/Binaries/Debug/ARM64`
- `Output/Binaries/Release/ARM64`

## Notes

- This project is a low-level system utility sample that directly triggers Windows memory list operations.
- Its purpose is to demonstrate a defragmentation approach based on NT APIs, rather than to provide a complex graphical interface or additional policies.
- Because its behavior depends on internal Windows memory management mechanisms, please use it with a proper understanding of its purpose.

## Acknowledgements

- Code source: [Mile](https://mouri.moe/zh/2021/11/14/Defrag-memory-with-NT-API/)

## License

This project is licensed under the MIT License. See:

- [LICENSE](./LICENSE)

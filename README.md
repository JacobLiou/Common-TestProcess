# TestProcess

`TestProcess` is a legacy Windows test-process DLL for the FSTP test platform. It hosts a socket-based XML command loop, runs requested test initialization and test items, and reports task, process, graph, and log information back to the platform.

The project is an MFC/Win32 dynamic library, not a standalone executable. Its primary output is `bin/TestProcess.dll`.

## What It Does

The DLL exports a small lifecycle API:

| Export | Purpose |
| --- | --- |
| `StartTP` | Starts the socket server and the message-processing worker threads. |
| `StopTP` | Stops processing and closes the socket server. |
| `ShowDialog` | Reserved calibration/back-door entry point; the current implementation returns success without showing a dialog. |
| `GetError` | Returns the most recent error text maintained by the main controller. |

At startup, `CMainTestPro` configures `CNetCommand`, `CTPInfoManager`, and `CTaskPro`. Incoming XML messages are classified by message type. Task initialization and task-run commands are delegated to `CTaskPro` and then `CTestTaskManager`; information queries are served by `CTPInfoManager`.

The current socket setup uses the hard-coded address `172.0.0.1` in `CMainTestPro::StartSocket`. Existing configuration files may describe other devices or paths, but this address is not currently read from them.

## Repository Layout

| Path | Contents |
| --- | --- |
| `src/` | MFC DLL source, Visual Studio projects, resources, and generated COM type-library wrappers. |
| `include/` | Platform-facing data structures and interfaces, including XML/network and task-menu contracts. |
| `lib/` | Local library/type-library dependencies. |
| `bin/` | Runtime DLL output, platform configuration, task data, logs, and release material. |
| `data/` | Sample task bundles and scan-temporary data. |
| `FSTPLog/`, `Log/` | Captured runtime logs. |
| `UUI_Debug/` | Debugger-related configuration and logs. |

## Build Prerequisites

Build from `src/TestProcess.sln` with Visual Studio C++ support for the `v140_xp` toolset, MFC, and the Win32 platform. The project defines `Debug|Win32` and `Release|Win32` configurations.

The checked-in project retains machine-specific external dependencies:

- MFC and WinSock headers/libraries.
- `lib/msxml4.dll`, imported through `#import`.
- `UDL2_Server.dll`, currently imported from `C:\Public-T\Software\UDL\UDL2_Server.dll`.
- Additional include/library directories stored as absolute paths in `src/TestProcess.vcxproj`.

Before building on a new machine, update or replace these external paths using local property sheets or project settings. Avoid committing local absolute paths unless the project intentionally standardizes them.

## Build

Open `src/TestProcess.sln`, select `Debug|Win32` or `Release|Win32`, then build the `TestProcess` project. A successful build writes the DLL to `bin/TestProcess.dll`.

From a Visual Studio Developer Command Prompt, the equivalent command is:

```powershell
msbuild .\src\TestProcess.sln /p:Configuration=Debug /p:Platform=Win32
```

The project has no automated test project configured. Validation is therefore integration-oriented: load the built DLL in the host platform, call `StartTP`, send the relevant XML command sequence, and inspect the task-completion and log responses.

## Runtime Configuration and Data

The deployment layout expects configuration beneath `bin/Config/`. For example, `FSTPConfig.xml` contains FSTP device communication settings and `SystemConfig.ini` contains server/configuration paths. These files include environment-specific addresses and paths; review them before deployment.

Task bundles and execution data are retained in `data/` and `bin/data/`. Runtime and historical logs can contain station, product, or operational data and should not be altered as part of a source-only change.

## Key Code Paths

- `src/TestProcess.cpp`: MFC initialization and exported lifecycle functions.
- `src/MainTestPro.cpp`: server startup, message loop, command dispatch, and worker-thread lifecycle.
- `src/NetCommand.*`: XML transport command implementation.
- `src/TaskPro.cpp` and `src/TestTaskManager.*`: test initialization and individual task execution.
- `src/TPInfoManager.*`: task metadata, base information, and XML-backed information reporting.
- `src/GraphPro.*`: graph-data reporting through the network command layer.

For implementation guidance and AI-specific working rules, see `AGENTS.md`.
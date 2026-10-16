# AGENTS.md

## Purpose

This repository contains a legacy MFC/Win32 DLL that participates in an FSTP test platform. Treat its XML socket protocol, exported DLL functions, configuration layout, and external interface headers as compatibility boundaries.

## Start Here

1. Read `README.md` for the project and deployment overview.
2. Trace DLL lifecycle changes from `src/TestProcess.cpp` into `CMainTestPro`.
3. Trace inbound protocol changes through `src/MainTestPro.cpp`, `src/NetCommand.*`, and the data types in `include/`.
4. Trace test-execution changes through `src/TaskPro.*` and `src/TestTaskManager.*`.

## Architecture

- `CTestProcessApp` initializes MFC socket support.
- Global `CMainTestPro g_tp` implements the public DLL lifecycle.
- `StartTP` starts the server plus message-monitor and worker threads.
- `CNetCommand` receives and sends XML messages using the contracts in `include/NetCommInterface.h` and related data headers.
- `CMainTestPro::MessagePro` acknowledges selected request/response messages immediately and queues command messages for `EventPro`.
- `CTaskPro` runs initialization or test tasks and reports `TASK_DONE` or `TASK_FAIL` through `CNetCommand`.
- `CTPInfoManager` publishes base info, task info, and task descriptions.

## Compatibility Rules

- Preserve the exported names and calling patterns in `src/TestProcess.cpp`: `StartTP`, `StopTP`, `ShowDialog`, and `GetError`.
- Preserve the XML protocol method signatures and data layouts in `include/` unless the host platform is updated in lockstep.
- Do not change numeric message IDs, task IDs, result enums, buffer sizes, or struct layouts without locating all producer and consumer updates.
- Keep MFC state requirements in mind for new exported functions. Existing exports do not invoke `AFX_MANAGE_STATE`; assess that boundary before adding MFC calls to an export.
- `StartSocket` currently uses `172.0.0.1` directly. Do not claim a configuration-file change affects it until the implementation is changed and tested.
- `ShowDialog` is presently a no-op success path. Do not rely on it to launch a calibration program.

## Build and External Dependencies

- Build `src/TestProcess.sln` as `Debug|Win32` or `Release|Win32` with the `v140_xp` toolset and MFC installed.
- The project imports `lib/msxml4.dll` and an externally installed `UDL2_Server.dll`; it also contains absolute include/library paths. A build failure on a new machine is likely an environment/dependency issue before it is a source defect.
- The output DLL path is `bin/TestProcess.dll`.
- Do not modernize the toolset, character set, project format, or COM imports as incidental cleanup. Make those separate, explicitly scoped changes.

## Source and Data Handling

- Many C++ source comments and string literals are in a legacy Chinese code page. Preserve a file's existing encoding; do not bulk-convert source, INI, XML, logs, or generated files to UTF-8.
- Treat `src/msxml4.tlh`, `src/msxml4.tli`, `src/UDL2_Server.tlh`, and `src/UDL2_Server.tli` as generated COM-import artifacts. Regenerate them from the matching dependency rather than editing them by hand.
- Treat `bin/Config/`, `bin/data/`, `data/`, `FSTPLog/`, and `Log/` as deployment or operational data. Do not modify them for a code-only request.
- Avoid committing machine-specific paths, station addresses, credentials, serial numbers, production data, or log changes.

## Change Workflow

1. Identify the controlling entry point or message type before editing.
2. Keep the edit limited to the owning module and preserve MFC/C++ style used nearby.
3. For command handling, verify both the inbound action and its success/failure XML/log response.
4. For thread or socket changes, verify startup, shutdown, event signaling, and error propagation.
5. Build the affected `Win32` configuration when the required dependencies are available. Report missing proprietary dependencies instead of creating placeholders.
6. No automated tests are configured in the solution. For behavior changes, provide a focused host-platform integration check using the relevant XML command sequence.

## Code Standards

- Follow the local MFC naming convention: classes use the `C` prefix, member fields use `m_`, and Boolean variables use a `b` prefix. Match nearby naming for Windows handles, strings, structures, and pointers.
- Keep declarations in headers and behavior in `.cpp` files. Add a public method only when a caller outside the class needs it; prefer private helpers for a single command-processing step.
- Preserve the existing C++ and MFC style in the touched file, including indentation, braces, `CString`, `BOOL`, and Windows API usage. Do not mix modern C++ rewrites into an unrelated fix.
- Initialize every pointer, handle, buffer, and state flag. Check pointer arguments before dereferencing them and validate message fields before converting or indexing with them.
- Give resources one clear owner. Close every `HANDLE`, stop every socket, and release COM/MFC resources on each success, failure, and shutdown path. Do not close a handle from multiple owners.
- Do not introduce raw ownership transfer, `new`/`delete` pairs, unchecked C string copies, or unbounded formatting into new code. When the existing ABI requires raw buffers, validate the destination length before writing.
- Use comments only for protocol, lifetime, threading, or hardware behavior that is not clear from the code. Keep comments in the existing file encoding.
- Keep changes narrowly scoped. Do not reformat unrelated source, regenerate type-library wrappers, or edit deployment data while changing behavior.

## Best Practices for This Codebase

- Handle an incoming command as a complete transaction: validate it, perform the requested action, send a matching success/failure result where the protocol requires one, and log actionable context without sensitive values.
- Keep protocol behavior symmetric. A new inbound XML message type needs an explicit dispatch path, error path, response/log behavior, and a host-side integration scenario.
- Treat `m_bStartPro`, `m_bSockState`, events, and socket shutdown as one lifecycle. Changes to worker threads must demonstrate that `StopTP` can unblock and finish without using invalid handles or accessing a destroyed owner.
- Do not silently discard failures. Preserve or set the error message, emit the appropriate `XmlSendLog`/result response, and return a failure value to the caller when recovery is not possible.
- Avoid blocking work on the message-monitor loop. If work is asynchronous, ensure its input remains valid for the lifetime of the worker and define how concurrent commands are serialized or rejected.
- Favor constants and existing protocol enums over literal command IDs, task offsets, timeouts, and buffer lengths. A literal is acceptable only when the surrounding code already defines it as a local implementation detail.
- Keep configuration behavior explicit. Use a configuration value only after defining its fallback, validation, deployment location, and error behavior; do not add another machine-specific path.
- Maintain backward compatibility unless a task explicitly includes a host-platform rollout plan. For breaking protocol or ABI changes, document the consumer impact and migration order before implementation.

## Code Review Checklist

- Does the change identify the actual controlling entry point, message type, or task implementation?
- Are exported functions, public interfaces, protocol enums, structure layouts, and buffer contracts unchanged, or intentionally versioned with all consumers accounted for?
- Are all failure paths observable through a return value, stored error, protocol response, or useful log entry?
- Does task execution send the correct completion state and preserve the initiating user/task identity?
- Are pointer checks, buffer lengths, conversions, and return values handled safely on malformed or incomplete XML data?
- Are handles, sockets, event signaling, worker lifetimes, and shared state safe during startup, command processing, and shutdown?
- Are logs free of credentials and unnecessary production identifiers?
- Is the changed source still in its original encoding, with no generated files, logs, or environment-specific configuration changed incidentally?
- Has the relevant `Win32` build or a documented dependency-blocked equivalent been run?
- Does the change include a focused manual integration check for its XML command, test task, graph response, or lifecycle behavior?

## Useful Paths

- `src/TestProcess.cpp`: DLL exports and MFC initialization.
- `src/MainTestPro.cpp`: socket lifecycle, command dispatch, worker threads.
- `src/TaskPro.cpp`: initialization/task completion reporting.
- `src/TestTaskManager.*`: concrete task execution.
- `src/TPInfoManager.*`: task and base-info reporting.
- `src/TestProcess.vcxproj`: configurations, toolset, and dependency paths.
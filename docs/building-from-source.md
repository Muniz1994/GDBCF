# Building from Source

This page explains how to compile GDBCF for Windows, Linux, and macOS using SCons.

---

## Prerequisites

| Tool | Version | Notes |
|---|---|---|
| [Python](https://www.python.org/) | 3.8+ | Required by SCons |
| [SCons](https://scons.org/) | 4.x | `pip install scons` |
| C++ compiler | — | MSVC 2022 on Windows; GCC 11+ or Clang 14+ on Linux/macOS |
| Git | any | To clone submodules |

---

## Clone the Repository

```bash
git clone https://github.com/your-org/GDBCF.git
cd GDBCF
git submodule update --init --recursive
```

This pulls:
- `thirdparty/godot-cpp` — Godot 4.4 C++ bindings (pinned to `godot-4.4-stable`)
- `thirdparty/tinyxml2` — XML parser

The `thirdparty/miniz/` folder is included directly in the repository (not a submodule).

---

## Build

### Windows (MSVC 2022)

Open a **Developer Command Prompt for VS 2022** or a terminal with the VS environment loaded, then:

```powershell
cd GDBCF
scons platform=windows arch=x86_64 target=template_debug
```

For a release build:

```powershell
scons platform=windows arch=x86_64 target=template_release
```

### Linux

```bash
scons platform=linux arch=x86_64 target=template_debug
```

### macOS

```bash
scons platform=macos arch=arm64 target=template_debug
# Universal binary (Intel + Apple Silicon):
scons platform=macos arch=universal target=template_debug
```

---

## Output

The compiled library lands in:

```
addons/GDBCF/bin/libgdbcf.<platform>.<target>.<arch>.dll   # Windows
addons/GDBCF/bin/libgdbcf.<platform>.<target>.<arch>.so    # Linux
addons/GDBCF/bin/libgdbcf.<platform>.<target>.<arch>.dylib # macOS
```

Copy the entire `addons/GDBCF/` folder into your Godot project.

---

## Build Options

| Option | Default | Description |
|---|---|---|
| `platform` | auto-detected | `windows`, `linux`, `macos` |
| `arch` | `x86_64` | `x86_64`, `arm64`, `universal` |
| `target` | `template_debug` | `template_debug` or `template_release` |
| `-j N` | 11 (auto) | Parallel jobs |

Example — fast debug build with 4 cores:

```bash
scons platform=linux arch=x86_64 target=template_debug -j4
```

---

## Generating godot-cpp Bindings

The bindings are generated automatically during the first build. If you ever need to regenerate them manually:

```bash
cd thirdparty/godot-cpp
scons platform=<platform> target=template_debug
```

---

## Clean Build

```bash
scons -c
```

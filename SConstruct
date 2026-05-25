#!/usr/bin/env python
import os
import sys

# ── Environment ──────────────────────────────────────────────────────────────
env = SConscript("thirdparty/godot-cpp/SConstruct")

# ── Compiler settings ────────────────────────────────────────────────────────
env.Append(CPPFLAGS=["-std=c++17"] if env["platform"] != "windows" else [])
if env["platform"] == "windows":
    env.Append(CXXFLAGS=["/std:c++17", "/EHsc"])

# ── Include paths ─────────────────────────────────────────────────────────────
env.Append(CPPPATH=[
    "src/",
    "thirdparty/tinyxml2/",
    "thirdparty/miniz/",
])

# ── Source files ──────────────────────────────────────────────────────────────
sources = Glob("src/*.cpp")
sources += Glob("src/resources/*.cpp")
sources += Glob("src/io/*.cpp")
sources += Glob("src/ifc_bridge/*.cpp")

# tinyxml2 and miniz compiled alongside the extension
sources += ["thirdparty/tinyxml2/tinyxml2.cpp"]
sources += [
    "thirdparty/miniz/miniz.c",
    "thirdparty/miniz/miniz_tdef.c",
    "thirdparty/miniz/miniz_tinfl.c",
    "thirdparty/miniz/miniz_zip.c",
]

# ── Output library name ───────────────────────────────────────────────────────
lib_name = "libgdbcf"
lib_path = "addons/GDBCF/bin/{}{}{}".format(
    lib_name,
    env["suffix"],
    env["SHLIBSUFFIX"],
)

# ── Build shared library ──────────────────────────────────────────────────────
library = env.SharedLibrary(lib_path, source=sources)
Default(library)

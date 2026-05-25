# GDBCF Documentation

**GDBCF** is a Godot 4 GDExtension for reading and writing **BCF-XML 3.0** files — the buildingSMART open standard for BIM coordination issues.

## Contents

| Page | Description |
|---|---|
| [Getting Started](getting-started.md) | Installation, scene setup, and first load |
| [BCF Format Overview](bcf-format.md) | What a BCF file contains and how it maps to Godot classes |
| [API Reference](api-reference.md) | Every class, property, method and signal |
| [GDIFC Integration](gdifc-integration.md) | Combining GDBCF with GDIFC to navigate viewpoints in an IFC scene |
| [Building from Source](building-from-source.md) | SCons build instructions for Windows, Linux and macOS |

## At a Glance

```
GDBCFManager            ← Node — entry point for load / save / factory
└─ BCFProject           ← Resource — root of the in-memory BCF tree
   ├─ BCFProjectInfo    ← project ID and name
   ├─ BCFExtensions     ← allowed types, statuses, priorities …
   ├─ BCFTopic[]        ← one per issue
   │  ├─ BCFComment[]
   │  ├─ BCFViewpointRef[]
   │  │  └─ BCFVisualizationInfo
   │  │     ├─ BCFComponents
   │  │     ├─ BCFPerspectiveCamera / BCFOrthogonalCamera
   │  │     ├─ BCFLine[]
   │  │     ├─ BCFClippingPlane[]
   │  │     └─ BCFBitmap[]
   │  ├─ BCFBimSnippet
   │  ├─ BCFDocumentReference[]
   │  └─ BCFHeaderFile[]
   └─ BCFDocument[]
```

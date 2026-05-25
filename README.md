# GDBCF — BCF-XML for Godot 4

**GDBCF** is a [GDExtension](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/index.html) that lets Godot 4 projects **read and write BCF-XML 3.0 files** — the buildingSMART open standard for exchanging coordination issues on top of BIM models.

Use it standalone or together with [GDIFC](https://github.com/your-org/GDIFC) to open a BCF file, jump to a saved camera viewpoint, and highlight or hide the IFC elements referenced by each topic — all from GDScript.

---

## Features

- **Full BCF-XML 3.0 read/write** — topics, comments, viewpoints, extensions, documents, header files
- **Godot Resource tree** — every BCF concept maps to a typed `Resource` subclass (editor-inspectable, serialisable)
- **Async + sync API** — threaded `load_bcf` / `save_bcf` with signals, plus blocking `_sync` variants for editor tooling
- **Factory helpers** — `create_project`, `create_topic`, `generate_guid` (UUID v4)
- **GDIFC bridge** — `apply_viewpoint` walks an IFC scene and applies BCF component visibility, selection and colour overrides; works without GDIFC present (duck-typed)
- **No runtime dependencies** — tinyxml2 and miniz are compiled in; nothing to install

---

## Installation

### Option A — Prebuilt binaries (recommended)

1. Copy the `addons/GDBCF/` folder into your project at `res://addons/GDBCF/`.
2. In Godot, open **Project → Project Settings → Plugins** and enable **GDBCF**.

### Option B — Build from source

See [docs/building-from-source.md](docs/building-from-source.md).

---

## Quick Start

### Load a BCF file

```gdscript
extends Node

@onready var bcf: GDBCFManager = $GDBCFManager

func _ready() -> void:
    bcf.bcf_loaded.connect(_on_bcf_loaded)
    bcf.error_occurred.connect(func(msg): push_error(msg))
    bcf.load_bcf("res://issues/coordination.bcf")

func _on_bcf_loaded(project: BCFProject) -> void:
    print("Loaded BCF v", project.version)
    for topic in project.topics:
        print(" - [%s] %s" % [topic.topic_status, topic.title])
```

### Create and save a BCF file

```gdscript
var bcf := GDBCFManager.new()

# Create a project with sensible default extensions
var project: BCFProject = bcf.create_project("My Coordination Model")

# Add a topic
var topic: BCFTopic = bcf.create_topic("CLASH", "OPEN", "Pipe clashes with beam", "alice@example.com")
project.topics.append(topic)

# Add a comment
var comment := BCFComment.new()
comment.guid        = bcf.generate_guid()
comment.date        = "2026-05-25T12:00:00Z"
comment.author      = "alice@example.com"
comment.comment     = "The 150mm pipe in grid C-4 intersects the primary steel beam."
topic.comments.append(comment)

# Save (async — emits bcf_saved on completion)
bcf.save_bcf(project, "res://issues/coordination.bcf")
```

### Apply a viewpoint to an IFC scene (GDIFC integration)

```gdscript
# Assumes GDIFCManager has already loaded the model and its children are IFCNodes.
var ifc_root: Node = $GDIFCManager

func _on_bcf_loaded(project: BCFProject) -> void:
    var topic: BCFTopic = project.topics[0]
    if topic.viewpoints.is_empty():
        return

    var vp_ref: BCFViewpointRef = topic.viewpoints[0]
    var vis: BCFVisualizationInfo = vp_ref.visualization_info

    # Move camera to the saved viewpoint
    if vis.camera_type == BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE:
        var cam := vis.perspective_camera
        $Camera3D.position  = cam.view_point
        $Camera3D.look_at(cam.view_point + cam.direction)

    # Show/hide IFC elements and apply colour overrides
    $GDBCFManager.apply_viewpoint(vis, ifc_root)
```

---

## BCF Concepts → Godot Classes

| BCF concept | Godot class |
|---|---|
| Container (ZIP) | `BCFProject` (root resource) |
| `project.bcfp` | `BCFProjectInfo` |
| `extensions.xml` | `BCFExtensions` |
| `{guid}/markup.bcf` | `BCFTopic` |
| Comment | `BCFComment` |
| Viewpoint reference | `BCFViewpointRef` |
| `{guid}/*.bcfv` | `BCFVisualizationInfo` |
| Component list | `BCFComponents` / `BCFComponent` |
| Visibility rules | `BCFComponentVisibility` |
| Colour override | `BCFComponentColor` |
| Perspective camera | `BCFPerspectiveCamera` |
| Orthogonal camera | `BCFOrthogonalCamera` |
| Line annotation | `BCFLine` |
| Clipping plane | `BCFClippingPlane` |
| Bitmap overlay | `BCFBitmap` |
| BIM snippet | `BCFBimSnippet` |
| Document reference | `BCFDocumentReference` |
| Document file | `BCFDocument` |
| Header file entry | `BCFHeaderFile` |

---

## Documentation

| Page | Description |
|---|---|
| [Getting Started](docs/getting-started.md) | Full setup and first scene guide |
| [API Reference](docs/api-reference.md) | All classes and methods |
| [BCF Format Overview](docs/bcf-format.md) | BCF-XML 3.0 container layout |
| [GDIFC Integration](docs/gdifc-integration.md) | Viewpoint bridge with GDIFC |
| [Building from Source](docs/building-from-source.md) | SCons build instructions |

---

## License

MIT. See [LICENSE](LICENSE).


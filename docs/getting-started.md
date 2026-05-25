# Getting Started

This guide walks you through installing GDBCF, adding it to a scene, and performing your first BCF load and save.

## Requirements

- Godot **4.4** or later
- A Godot project (2D or 3D)

---

## Installation

### Option A — Prebuilt Binaries

1. Download the latest release from the [GitHub Releases page](https://github.com/your-org/GDBCF/releases).
2. Extract and copy the `addons/GDBCF/` folder into your project so it sits at `res://addons/GDBCF/`.
3. Open **Project → Project Settings → Plugins** and enable **GDBCF**.

### Option B — Build from Source

See [Building from Source](building-from-source.md).

---

## Scene Setup

1. Add a **`GDBCFManager`** node to your scene tree (it extends `Node`, so it can live anywhere).
2. Attach a script to the same node or to a parent node.

```
MyScene (Node3D)
├─ GDBCFManager
└─ ...
```

---

## Loading a BCF File

### Async (recommended for runtime)

`load_bcf` runs on a background thread and emits signals when done, keeping the main thread free.

```gdscript
extends Node

@onready var bcf: GDBCFManager = $GDBCFManager

func _ready() -> void:
    bcf.bcf_loaded.connect(_on_bcf_loaded)
    bcf.error_occurred.connect(_on_error)
    bcf.load_bcf("res://issues/review.bcf")

func _on_bcf_loaded(project: BCFProject) -> void:
    print("BCF version: ", project.version)
    print("Topics: ", project.topics.size())
    for topic: BCFTopic in project.topics:
        print("  [%s] %s — %s" % [topic.topic_status, topic.guid, topic.title])

func _on_error(message: String) -> void:
    push_error("BCF error: " + message)
```

### Sync (editor tools / offline processing)

```gdscript
var bcf := GDBCFManager.new()
var project: BCFProject = bcf.load_bcf_sync("res://issues/review.bcf")
if project == null:
    push_error("Failed to load BCF")
    return
print("Loaded %d topics" % project.topics.size())
```

---

## Creating and Saving a BCF File

```gdscript
var bcf := GDBCFManager.new()

# 1. Create a project shell with default extensions
var project: BCFProject = bcf.create_project("Site Coordination - Block A")

# 2. Create a topic
var topic: BCFTopic = bcf.create_topic(
    "CLASH",          # type (must be in extensions.topic_types)
    "OPEN",           # status
    "Pipe clashes with primary beam at grid C-4",
    "alice@office.com"
)
project.topics.append(topic)

# 3. Add a comment
var comment := BCFComment.new()
comment.guid    = bcf.generate_guid()
comment.date    = "2026-05-25T09:00:00Z"
comment.author  = "alice@office.com"
comment.comment = "Confirmed in site visit. Pipe needs to be re-routed 200 mm to the east."
topic.comments.append(comment)

# 4. Save async (emits bcf_saved or error_occurred)
bcf.bcf_saved.connect(func(): print("Saved!"))
bcf.error_occurred.connect(func(m): push_error(m))
bcf.save_bcf(project, "user://exports/review.bcf")
```

---

## Iterating Topics

```gdscript
for topic: BCFTopic in project.topics:
    print(topic.title)
    for comment: BCFComment in topic.comments:
        print("  ", comment.author, ": ", comment.comment)
    for vp_ref: BCFViewpointRef in topic.viewpoints:
        var vis: BCFVisualizationInfo = vp_ref.visualization_info
        if vis and vis.camera_type == BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE:
            var cam: BCFPerspectiveCamera = vis.perspective_camera
            print("  Camera at ", cam.view_point, " fov=", cam.fov)
```

---

## Next Steps

- [BCF Format Overview](bcf-format.md) — understand the ZIP container layout
- [API Reference](api-reference.md) — full property and method listing
- [GDIFC Integration](gdifc-integration.md) — apply viewpoints to a live IFC scene

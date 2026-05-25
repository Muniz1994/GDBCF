# GDIFC Integration

GDBCF can work standalone, but its full power comes when combined with **GDIFC** — the Godot IFC loader. Together they let you:

1. Open a BCF file that was created in tools like Solibri, BIMcollab, or Revit.
2. Jump the camera to a saved viewpoint.
3. Automatically show, hide, and colour the IFC elements referenced by each BCF topic.

---

## How the Bridge Works

`GDBCFManager.apply_viewpoint(vis, ifc_root)` performs a **duck-typed** walk of the scene tree rooted at `ifc_root`. It does not import GDIFC headers at compile time. Instead it checks at runtime whether each `Node` exposes an `ifc_guid` property (or `get_ifc_guid()` method). Any node that does is treated as an IFC element.

This means:
- The bridge compiles and ships **without GDIFC present**.
- If GDIFC is loaded, `IFCNode` instances (which expose `ifc_guid`) are found automatically.
- You can also use it with any custom node that has an `ifc_guid` property.

---

## Scene Setup

```
MyScene (Node3D)
├─ GDIFCManager          ← loads the IFC model; creates IFCNode children
├─ GDBCFManager          ← loads BCF issues
└─ Camera3D
```

---

## Complete Example

```gdscript
extends Node3D

@onready var ifc_manager: GDIFCManager  = $GDIFCManager
@onready var bcf_manager: GDBCFManager  = $GDBCFManager
@onready var camera: Camera3D           = $Camera3D

var bcf_project: BCFProject
var current_topic_index := 0

func _ready() -> void:
    # Load the IFC model first
    ifc_manager.read_ifc("res://models/building.ifc", false, [])

    # Connect BCF signals
    bcf_manager.bcf_loaded.connect(_on_bcf_loaded)
    bcf_manager.error_occurred.connect(func(m): push_error(m))
    bcf_manager.load_bcf("res://issues/coordination.bcf")

func _on_bcf_loaded(project: BCFProject) -> void:
    bcf_project = project
    print("Loaded %d topics" % project.topics.size())
    # Jump straight to the first topic
    if project.topics.size() > 0:
        show_topic(0)

func show_topic(index: int) -> void:
    if bcf_project == null or index >= bcf_project.topics.size():
        return

    current_topic_index = index
    var topic: BCFTopic = bcf_project.topics[index]
    print("[%d/%d] %s" % [index + 1, bcf_project.topics.size(), topic.title])

    if topic.viewpoints.is_empty():
        return

    var vp_ref: BCFViewpointRef  = topic.viewpoints[0]
    var vis: BCFVisualizationInfo = vp_ref.visualization_info
    if vis == null:
        return

    # ── Move camera ──────────────────────────────────────────────────────────
    if vis.camera_type == BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE:
        var cam: BCFPerspectiveCamera = vis.perspective_camera
        camera.position = cam.view_point
        # BCF direction is a unit vector; look_at needs a target point
        camera.look_at(cam.view_point + cam.direction, cam.up_vector)
        camera.fov = cam.fov

    elif vis.camera_type == BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL:
        var cam: BCFOrthogonalCamera = vis.orthogonal_camera
        camera.position = cam.view_point
        camera.look_at(cam.view_point + cam.direction, cam.up_vector)

    # ── Apply component visibility, selection and colour overrides ───────────
    bcf_manager.apply_viewpoint(vis, ifc_manager)

func _input(event: InputEvent) -> void:
    # Navigate topics with arrow keys
    if event.is_action_pressed("ui_right"):
        show_topic(current_topic_index + 1)
    elif event.is_action_pressed("ui_left"):
        show_topic(current_topic_index - 1)
```

---

## What apply_viewpoint Does, Step by Step

### 1. Build a GUID map

Walks every node under `ifc_root`. For each node it calls `get_ifc_guid()` (method check first, then property scan). Builds a `HashMap<ifc_guid → Node*>`.

### 2. Apply visibility

Reads `BCFComponentVisibility.default_visibility`:
- If `true` — all IFC nodes are made visible; nodes in `exceptions` are hidden.
- If `false` — all IFC nodes are hidden; nodes in `exceptions` are shown.

Visibility is applied by calling `Node3D.set_visible()`.

### 3. Apply selection highlight

For each `BCFComponent` in `BCFComponents.selection`:
- Calls `set_highlight(true)` if the node has that method (GDIFC pattern).
- Otherwise sets `modulate` to a warm yellow (`Color(1.5, 1.5, 0.5, 1.0)`) on `MeshInstance3D` nodes.

### 4. Apply colour overrides

For each `BCFComponentColor` in `BCFComponents.coloring`:
- Parses the hex string (RRGGBB or AARRGGBB, BCF ARGB order).
- Calls `set_highlight_color(color)` if available, otherwise sets `modulate` on `MeshInstance3D`.

---

## Coordinate System Note

BCF stores positions in the IFC coordinate system. If GDIFC's `coordinate_to_origin` setting was enabled when loading the model, all `IFCNode` positions were shifted. You must apply the same offset to the BCF camera positions before passing them to `Camera3D`:

```gdscript
# Example: if you know the IFC origin offset
var ifc_offset := Vector3(500_000.0, 0.0, -200_000.0)
var cam: BCFPerspectiveCamera = vis.perspective_camera
camera.position = cam.view_point - ifc_offset
camera.look_at(cam.view_point - ifc_offset + cam.direction, cam.up_vector)
```

GDIFC exposes the applied offset via `GDIFCManager.origin_offset` (see GDIFC docs).

---

## Using Without GDIFC

If you have custom nodes with an `ifc_guid` property, `apply_viewpoint` will still work:

```gdscript
# Any node with an "ifc_guid" exported property works
class_name MyIfcProxy extends Node3D
@export var ifc_guid: String
```

No changes to GDBCF are needed.

# API Reference

Complete listing of all GDBCF classes, properties, methods and signals.

---

## GDBCFManager (`Node`)

The main entry point. Add one to your scene.

### Signals

| Signal | Arguments | Description |
|---|---|---|
| `bcf_loaded` | `project: BCFProject` | Emitted when `load_bcf` completes successfully |
| `bcf_saved` | — | Emitted when `save_bcf` completes successfully |
| `error_occurred` | `message: String` | Emitted on any async error |

### Methods — Async

```gdscript
func load_bcf(path: String) -> void
```
Loads a `.bcf` file on a background thread. Emits `bcf_loaded` or `error_occurred`.

```gdscript
func save_bcf(project: BCFProject, path: String) -> void
```
Writes `project` to `path` on a background thread. Emits `bcf_saved` or `error_occurred`.

> Only one async operation can run at a time. Calling a second while one is running emits `error_occurred` immediately.

### Methods — Sync

```gdscript
func load_bcf_sync(path: String) -> BCFProject
```
Blocking load. Returns `null` on failure. Suitable for editor scripts and `@tool` code.

```gdscript
func save_bcf_sync(project: BCFProject, path: String) -> int
```
Blocking save. Returns `OK` (`0`) on success or a non-zero error code.

### Methods — Factory

```gdscript
func create_project(name: String) -> BCFProject
```
Returns a new `BCFProject` with `version = "3.0"`, a generated project ID, and a sensible default `BCFExtensions` (types: ERROR, WARNING, INFORMATION, CLASH, OTHER; statuses: OPEN, IN_PROGRESS, SOLVED, CLOSED; priorities: LOW, MEDIUM, HIGH, CRITICAL).

```gdscript
func create_topic(type: String, status: String, title: String, author: String) -> BCFTopic
```
Returns a new `BCFTopic` with a generated UUID GUID, current UTC timestamp, and the supplied fields.

```gdscript
func generate_guid() -> String
```
Returns a random UUID v4 string in the form `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx`.

### Methods — GDIFC Bridge

```gdscript
func apply_viewpoint(viewpoint_info: BCFVisualizationInfo, ifc_root: Node) -> void
```
Walks the `ifc_root` subtree, finds nodes that expose an `ifc_guid` property (e.g. GDIFC `IFCNode` instances), and applies:
- **Visibility** from `BCFComponentVisibility` (shows/hides `Node3D` nodes)
- **Selection highlights** (`set_highlight(true)` if available, otherwise modulate)
- **Colour overrides** from `BCFComponentColor` (`set_highlight_color` if available, otherwise modulate)

No hard dependency on GDIFC — works with any node that exposes `ifc_guid`.

---

## BCFProject (`Resource`)

Root of the in-memory BCF tree, one per loaded file.

| Property | Type | Description |
|---|---|---|
| `version` | `String` | BCF version string, always `"3.0"` |
| `project_info` | `BCFProjectInfo` | Project ID and name |
| `extensions` | `BCFExtensions` | Allowed enum values |
| `topics` | `Array[BCFTopic]` | All topics in the file |
| `documents` | `Array[BCFDocument]` | Embedded document files |

---

## BCFProjectInfo (`Resource`)

| Property | Type | Description |
|---|---|---|
| `project_id` | `String` | UUID identifying the project |
| `name` | `String` | Human-readable project name |

---

## BCFExtensions (`Resource`)

Defines which string values are valid in topic fields.

| Property | Type | Description |
|---|---|---|
| `topic_types` | `Array[String]` | e.g. `["CLASH", "WARNING"]` |
| `topic_statuses` | `Array[String]` | e.g. `["OPEN", "CLOSED"]` |
| `priorities` | `Array[String]` | e.g. `["HIGH", "LOW"]` |
| `labels` | `Array[String]` | Free-form tag labels |
| `users` | `Array[String]` | Known user identifiers |
| `snippet_types` | `Array[String]` | BIM snippet MIME types |
| `stages` | `Array[String]` | Project stage identifiers |

---

## BCFTopic (`Resource`)

One coordination issue.

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID |
| `server_assigned_id` | `String` | Server-side ID (optional) |
| `topic_type` | `String` | Must match `extensions.topic_types` |
| `topic_status` | `String` | Must match `extensions.topic_statuses` |
| `title` | `String` | Short summary |
| `description` | `String` | Detailed text |
| `creation_date` | `String` | ISO 8601 UTC |
| `creation_author` | `String` | |
| `modified_date` | `String` | ISO 8601 UTC |
| `modified_author` | `String` | |
| `due_date` | `String` | ISO 8601 UTC |
| `assigned_to` | `String` | User identifier |
| `priority` | `String` | Must match `extensions.priorities` |
| `labels` | `Array[String]` | |
| `stage` | `String` | |
| `reference_links` | `Array[String]` | External URLs |
| `bim_snippet` | `BCFBimSnippet` | |
| `document_references` | `Array[BCFDocumentReference]` | |
| `related_topics` | `Array[String]` | GUIDs of related topics |
| `header_files` | `Array[BCFHeaderFile]` | IFC file references |
| `comments` | `Array[BCFComment]` | |
| `viewpoints` | `Array[BCFViewpointRef]` | |

---

## BCFComment (`Resource`)

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID |
| `date` | `String` | ISO 8601 UTC |
| `author` | `String` | |
| `comment` | `String` | Text body |
| `viewpoint_guid` | `String` | Links to a `BCFViewpointRef.guid` (optional) |
| `modified_date` | `String` | |
| `modified_author` | `String` | |

---

## BCFViewpointRef (`Resource`)

A reference to a viewpoint file (`.bcfv`) stored inside the topic folder.

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID |
| `viewpoint_filename` | `String` | Filename of the `.bcfv` file |
| `snapshot_filename` | `String` | Filename of the snapshot image |
| `snapshot_data` | `PackedByteArray` | Raw image bytes (PNG or JPEG) |
| `index` | `int` | Display order hint (`-1` = unset) |
| `visualization_info` | `BCFVisualizationInfo` | Parsed viewpoint data |

---

## BCFVisualizationInfo (`Resource`)

The parsed contents of a `.bcfv` file.

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID |
| `components` | `BCFComponents` | Component selection / visibility |
| `camera_type` | `int` | `BCF_CAMERA_NONE`, `BCF_CAMERA_PERSPECTIVE`, `BCF_CAMERA_ORTHOGONAL` |
| `perspective_camera` | `BCFPerspectiveCamera` | Valid when `camera_type == BCF_CAMERA_PERSPECTIVE` |
| `orthogonal_camera` | `BCFOrthogonalCamera` | Valid when `camera_type == BCF_CAMERA_ORTHOGONAL` |
| `lines` | `Array[BCFLine]` | Annotation lines |
| `clipping_planes` | `Array[BCFClippingPlane]` | |
| `bitmaps` | `Array[BCFBitmap]` | Image overlays |

**Constants** (bind to GDScript as `BCFVisualizationInfo.BCF_CAMERA_*`):

| Constant | Value |
|---|---|
| `BCF_CAMERA_NONE` | `0` |
| `BCF_CAMERA_PERSPECTIVE` | `1` |
| `BCF_CAMERA_ORTHOGONAL` | `2` |

---

## BCFComponents (`Resource`)

| Property | Type | Description |
|---|---|---|
| `selection` | `Array[BCFComponent]` | Selected (highlighted) components |
| `visibility` | `BCFComponentVisibility` | Default show/hide rule + exceptions |
| `coloring` | `Array[BCFComponentColor]` | Per-group colour overrides |

---

## BCFComponent (`Resource`)

Identifies a single IFC element.

| Property | Type | Description |
|---|---|---|
| `ifc_guid` | `String` | IFC compressed GUID |
| `originating_system` | `String` | Tool that created the reference |
| `authoring_tool_id` | `String` | Application-specific ID |

---

## BCFComponentVisibility (`Resource`)

| Property | Type | Description |
|---|---|---|
| `default_visibility` | `bool` | `true` = all visible by default |
| `exceptions` | `Array[BCFComponent]` | Components that differ from the default |
| `spaces_visible` | `bool` | |
| `space_boundaries_visible` | `bool` | |
| `openings_visible` | `bool` | |

---

## BCFComponentColor (`Resource`)

| Property | Type | Description |
|---|---|---|
| `color` | `String` | RRGGBB or AARRGGBB hex (BCF ARGB order) |
| `components` | `Array[BCFComponent]` | Components to colour |

---

## BCFPerspectiveCamera (`Resource`)

| Property | Type | Description |
|---|---|---|
| `view_point` | `Vector3` | Camera position |
| `direction` | `Vector3` | Unit look direction |
| `up_vector` | `Vector3` | Up axis |
| `fov` | `float` | Vertical field of view in degrees (default `60`) |
| `aspect_ratio` | `float` | Width / height (default `1`) |

---

## BCFOrthogonalCamera (`Resource`)

| Property | Type | Description |
|---|---|---|
| `view_point` | `Vector3` | Camera position |
| `direction` | `Vector3` | Unit look direction |
| `up_vector` | `Vector3` | Up axis |
| `view_to_world_scale` | `float` | Metres per pixel height (default `1`) |
| `aspect_ratio` | `float` | Width / height (default `1`) |

---

## BCFLine (`Resource`)

| Property | Type | Description |
|---|---|---|
| `start_point` | `Vector3` | |
| `end_point` | `Vector3` | |

---

## BCFClippingPlane (`Resource`)

| Property | Type | Description |
|---|---|---|
| `location` | `Vector3` | A point on the plane |
| `direction` | `Vector3` | Outward normal |

---

## BCFBitmap (`Resource`)

An image overlay placed in 3D space.

| Property | Type | Description |
|---|---|---|
| `format` | `String` | `"png"` or `"jpg"` |
| `reference` | `String` | Filename inside the topic folder |
| `location` | `Vector3` | Centre position |
| `normal` | `Vector3` | Plane normal |
| `up` | `Vector3` | Up axis within the plane |
| `height` | `float` | Metres (default `1`) |

---

## BCFBimSnippet (`Resource`)

| Property | Type | Description |
|---|---|---|
| `snippet_type` | `String` | MIME type or label |
| `is_external` | `bool` | `false` = embedded, `true` = URL reference |
| `reference` | `String` | File path or URL |
| `reference_schema` | `String` | Schema URL |

---

## BCFDocumentReference (`Resource`)

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID |
| `document_guid` | `String` | References a `BCFDocument.guid` (internal) |
| `url` | `String` | External URL (used when `document_guid` is empty) |
| `description` | `String` | |

---

## BCFDocument (`Resource`)

An embedded file stored in the BCF ZIP.

| Property | Type | Description |
|---|---|---|
| `guid` | `String` | UUID — also the path inside the ZIP |
| `filename` | `String` | Original file name |
| `description` | `String` | |
| `data` | `PackedByteArray` | File contents |

---

## BCFHeaderFile (`Resource`)

An IFC file referenced in the topic header.

| Property | Type | Description |
|---|---|---|
| `ifc_project` | `String` | `IfcProject` GUID |
| `ifc_spatial_structure_element` | `String` | GUID of the relevant spatial element |
| `is_external` | `bool` | `true` = file is not embedded |
| `filename` | `String` | Display name |
| `date` | `String` | ISO 8601 date |
| `reference` | `String` | File path or URL |

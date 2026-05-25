## BCFDock — editor panel for reading and creating BCF-XML 3.0 files.
##
## Sits in the right dock panel (same slot as Inspector and Signals).
## Requires the GDBCF GDExtension to be loaded (provides BCFProject, BCFTopic, etc.)
@tool
extends Control

# ── Constants ──────────────────────────────────────────────────────────────────
const PLUGIN_NAME := "BCF"

# Status colours (match common BCF conventions)
const STATUS_COLORS := {
	"OPEN":        Color(0.20, 0.55, 1.00),   # blue
	"IN_PROGRESS": Color(1.00, 0.75, 0.10),   # amber
	"SOLVED":      Color(0.20, 0.80, 0.40),   # green
	"CLOSED":      Color(0.55, 0.55, 0.55),   # grey
}
const TYPE_COLORS := {
	"CLASH":       Color(1.00, 0.30, 0.30),
	"WARNING":     Color(1.00, 0.65, 0.10),
	"ERROR":       Color(0.90, 0.10, 0.10),
	"INFORMATION": Color(0.30, 0.75, 1.00),
}

# ── State ──────────────────────────────────────────────────────────────────────
var _manager: GDBCFManager
var _project: BCFProject
var _current_path := ""
var _selected_topic: BCFTopic
var _dirty := false

# ── UI references ──────────────────────────────────────────────────────────────
var _lbl_file: Label
var _btn_save: Button
var _topic_tree: Tree
var _tab_container: TabContainer

# Details tab
var _fld_guid: LineEdit
var _fld_type: OptionButton
var _fld_status: OptionButton
var _fld_priority: OptionButton
var _fld_title: LineEdit
var _fld_assigned: LineEdit
var _fld_due: LineEdit
var _fld_desc: TextEdit
var _btn_apply_topic: Button

# Comments tab
var _comment_list: VBoxContainer
var _fld_comment_text: TextEdit
var _btn_add_comment: Button

# Viewpoints tab
var _selected_viewpoint: BCFViewpointRef
var _vp_entries_box: VBoxContainer
var _vp_detail_panel: PanelContainer
var _vp_camera_opt: OptionButton
var _vp_components_box: VBoxContainer
var _btn_delete_vp: Button

# Debug panel
var _debug_panel: PanelContainer
var _debug_log: RichTextLabel

# ── Lifecycle ──────────────────────────────────────────────────────────────────

func _init() -> void:
	name = PLUGIN_NAME

func _ready() -> void:
	_manager = GDBCFManager.new()
	add_child(_manager)
	_manager.bcf_loaded.connect(_on_bcf_loaded)
	_manager.bcf_saved.connect(_on_bcf_saved)
	_manager.error_occurred.connect(_on_error)
	_build_ui()

# ── UI Construction ────────────────────────────────────────────────────────────

func _build_ui() -> void:
	# Root layout
	var root := VBoxContainer.new()
	root.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	root.add_theme_constant_override("separation", 4)
	add_child(root)

	# ── Toolbar ───────────────────────────────────────────────────────────────
	var toolbar := HBoxContainer.new()
	toolbar.add_theme_constant_override("separation", 4)
	root.add_child(toolbar)

	var btn_open := _make_button("Open…", _on_open_pressed)
	btn_open.tooltip_text = "Open an existing .bcf file"
	toolbar.add_child(btn_open)

	var btn_new := _make_button("New", _on_new_pressed)
	btn_new.tooltip_text = "Create a new empty BCF project"
	toolbar.add_child(btn_new)

	_btn_save = _make_button("Save", _on_save_pressed)
	_btn_save.tooltip_text = "Save to the current file"
	_btn_save.disabled = true
	toolbar.add_child(_btn_save)

	var btn_save_as := _make_button("Save As…", _on_save_as_pressed)
	btn_save_as.tooltip_text = "Save to a new file"
	toolbar.add_child(btn_save_as)

	toolbar.add_child(VSeparator.new())

	_lbl_file = Label.new()
	_lbl_file.text = "(no file)"
	_lbl_file.clip_text = true
	_lbl_file.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_lbl_file.add_theme_color_override("font_color", Color(0.7, 0.7, 0.7))
	toolbar.add_child(_lbl_file)

	toolbar.add_child(VSeparator.new())
	var btn_dbg := Button.new()
	btn_dbg.text = "DBG"
	btn_dbg.tooltip_text = "Toggle debug log"
	btn_dbg.toggle_mode = true
	btn_dbg.toggled.connect(_on_debug_toggled)
	toolbar.add_child(btn_dbg)

	# ── Topics section ────────────────────────────────────────────────────────
	var topics_header := HBoxContainer.new()
	root.add_child(topics_header)

	var lbl_topics := Label.new()
	lbl_topics.text = "Topics"
	lbl_topics.add_theme_font_size_override("font_size", 12)
	lbl_topics.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	topics_header.add_child(lbl_topics)

	var btn_add_topic := _make_button("+ Add", _on_add_topic_pressed)
	btn_add_topic.tooltip_text = "Create a new topic"
	topics_header.add_child(btn_add_topic)

	var btn_del_topic := _make_button("Delete", _on_delete_topic_pressed)
	btn_del_topic.tooltip_text = "Remove the selected topic"
	topics_header.add_child(btn_del_topic)

	_topic_tree = Tree.new()
	_topic_tree.custom_minimum_size = Vector2(0, 160)
	_topic_tree.hide_root = true
	_topic_tree.columns = 3
	_topic_tree.set_column_title(0, "Title")
	_topic_tree.set_column_title(1, "Type")
	_topic_tree.set_column_title(2, "Status")
	_topic_tree.set_column_titles_visible(true)
	_topic_tree.set_column_expand(0, true)
	_topic_tree.set_column_expand(1, false)
	_topic_tree.set_column_custom_minimum_width(1, 80)
	_topic_tree.set_column_expand(2, false)
	_topic_tree.set_column_custom_minimum_width(2, 80)
	_topic_tree.size_flags_vertical = Control.SIZE_EXPAND_FILL
	_topic_tree.item_selected.connect(_on_topic_selected)
	root.add_child(_topic_tree)

	# ── Details tabs ──────────────────────────────────────────────────────────
	_tab_container = TabContainer.new()
	_tab_container.custom_minimum_size = Vector2(0, 280)
	_tab_container.size_flags_vertical = Control.SIZE_EXPAND_FILL
	root.add_child(_tab_container)

	_build_details_tab()
	_build_comments_tab()
	_build_viewpoints_tab()

	_set_details_enabled(false)
	_build_debug_panel(root)

# ── Details tab ────────────────────────────────────────────────────────────────

func _build_details_tab() -> void:
	var scroll := ScrollContainer.new()
	scroll.name = "Topic"
	scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	_tab_container.add_child(scroll)

	var vbox := VBoxContainer.new()
	vbox.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	vbox.add_theme_constant_override("separation", 6)
	scroll.add_child(vbox)

	_fld_guid = _add_field(vbox, "GUID", LineEdit.new())
	_fld_guid.editable = false
	_fld_guid.add_theme_color_override("font_color", Color(0.6, 0.6, 0.6))

	_fld_title     = _add_field(vbox, "Title",     LineEdit.new())
	_fld_assigned  = _add_field(vbox, "Assigned",  LineEdit.new())
	_fld_due       = _add_field(vbox, "Due Date",  LineEdit.new())
	_fld_due.placeholder_text = "YYYY-MM-DDTHH:MM:SSZ"

	_fld_type = _add_option(vbox, "Type")
	_fld_status = _add_option(vbox, "Status")
	_fld_priority = _add_option(vbox, "Priority")

	var lbl_desc := Label.new()
	lbl_desc.text = "Description"
	vbox.add_child(lbl_desc)

	_fld_desc = TextEdit.new()
	_fld_desc.custom_minimum_size = Vector2(0, 70)
	_fld_desc.wrap_mode = TextEdit.LINE_WRAPPING_BOUNDARY
	vbox.add_child(_fld_desc)

	_btn_apply_topic = Button.new()
	_btn_apply_topic.text = "Apply Changes"
	_btn_apply_topic.pressed.connect(_on_apply_topic_pressed)
	vbox.add_child(_btn_apply_topic)

# ── Comments tab ───────────────────────────────────────────────────────────────

func _build_comments_tab() -> void:
	var vbox := VBoxContainer.new()
	vbox.name = "Comments"
	vbox.add_theme_constant_override("separation", 6)
	_tab_container.add_child(vbox)

	var scroll := ScrollContainer.new()
	scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	scroll.custom_minimum_size = Vector2(0, 140)
	vbox.add_child(scroll)

	_comment_list = VBoxContainer.new()
	_comment_list.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_comment_list.add_theme_constant_override("separation", 4)
	scroll.add_child(_comment_list)

	var sep := HSeparator.new()
	vbox.add_child(sep)

	var lbl_new := Label.new()
	lbl_new.text = "Add comment"
	vbox.add_child(lbl_new)

	_fld_comment_text = TextEdit.new()
	_fld_comment_text.custom_minimum_size = Vector2(0, 55)
	_fld_comment_text.placeholder_text = "Write a comment…"
	_fld_comment_text.wrap_mode = TextEdit.LINE_WRAPPING_BOUNDARY
	vbox.add_child(_fld_comment_text)

	_btn_add_comment = Button.new()
	_btn_add_comment.text = "Add Comment"
	_btn_add_comment.pressed.connect(_on_add_comment_pressed)
	vbox.add_child(_btn_add_comment)

# ── Viewpoints tab ─────────────────────────────────────────────────────────────

func _build_viewpoints_tab() -> void:
	var outer := VBoxContainer.new()
	outer.name = "Viewpoints"
	outer.size_flags_vertical = Control.SIZE_EXPAND_FILL
	outer.add_theme_constant_override("separation", 4)
	_tab_container.add_child(outer)

	# Toolbar
	var vp_toolbar := HBoxContainer.new()
	outer.add_child(vp_toolbar)

	var btn_add_vp := _make_button("+ Add Viewpoint", _on_add_viewpoint_pressed)
	btn_add_vp.tooltip_text = "Create a new empty viewpoint for this topic"
	vp_toolbar.add_child(btn_add_vp)

	_btn_delete_vp = _make_button("Delete", _on_delete_viewpoint_pressed)
	_btn_delete_vp.tooltip_text = "Remove the selected viewpoint"
	_btn_delete_vp.disabled = true
	vp_toolbar.add_child(_btn_delete_vp)

	# Viewpoint entry list
	var list_scroll := ScrollContainer.new()
	list_scroll.custom_minimum_size = Vector2(0, 90)
	list_scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	outer.add_child(list_scroll)

	_vp_entries_box = VBoxContainer.new()
	_vp_entries_box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_vp_entries_box.add_theme_constant_override("separation", 2)
	list_scroll.add_child(_vp_entries_box)

	outer.add_child(HSeparator.new())

	# ── Detail panel (hidden until a viewpoint is selected) ────────────────────
	_vp_detail_panel = PanelContainer.new()
	_vp_detail_panel.size_flags_vertical = Control.SIZE_EXPAND_FILL
	_vp_detail_panel.visible = false
	outer.add_child(_vp_detail_panel)

	var detail_vb := VBoxContainer.new()
	detail_vb.add_theme_constant_override("separation", 6)
	_vp_detail_panel.add_child(detail_vb)

	# Camera row
	var cam_hdr := Label.new()
	cam_hdr.text = "Camera"
	cam_hdr.add_theme_font_size_override("font_size", 11)
	detail_vb.add_child(cam_hdr)

	var cam_row := HBoxContainer.new()
	detail_vb.add_child(cam_row)

	_vp_camera_opt = OptionButton.new()
	_vp_camera_opt.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_vp_camera_opt.add_item("None",        BCFVisualizationInfo.BCF_CAMERA_NONE)
	_vp_camera_opt.add_item("Perspective",  BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE)
	_vp_camera_opt.add_item("Orthogonal",   BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL)
	_vp_camera_opt.item_selected.connect(_on_vp_camera_type_changed)
	cam_row.add_child(_vp_camera_opt)

	var btn_capture := _make_button("Capture Editor Cam", _on_capture_camera_pressed)
	btn_capture.tooltip_text = "Read the active 3D editor viewport camera into this viewpoint"
	cam_row.add_child(btn_capture)

	detail_vb.add_child(HSeparator.new())

	# IFC Component selection
	var comp_hdr := Label.new()
	comp_hdr.text = "IFC Component Selection"
	comp_hdr.add_theme_font_size_override("font_size", 11)
	detail_vb.add_child(comp_hdr)

	var comp_scroll := ScrollContainer.new()
	comp_scroll.custom_minimum_size = Vector2(0, 70)
	comp_scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	detail_vb.add_child(comp_scroll)

	_vp_components_box = VBoxContainer.new()
	_vp_components_box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_vp_components_box.add_theme_constant_override("separation", 2)
	comp_scroll.add_child(_vp_components_box)

	var comp_btns := HBoxContainer.new()
	detail_vb.add_child(comp_btns)

	var btn_add_sel := _make_button("Add Scene Selection", _on_add_scene_selection_pressed)
	btn_add_sel.tooltip_text = "Add the IFC GUID of every selected scene node to this viewpoint's component selection"
	btn_add_sel.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	comp_btns.add_child(btn_add_sel)

	var btn_clr_sel := _make_button("Clear", _on_clear_vp_selection_pressed)
	btn_clr_sel.tooltip_text = "Remove all component selections from this viewpoint"
	comp_btns.add_child(btn_clr_sel)

# ── Helper widgets ─────────────────────────────────────────────────────────────

func _make_button(label: String, cb: Callable) -> Button:
	var btn := Button.new()
	btn.text = label
	btn.pressed.connect(cb)
	return btn

func _add_field(parent: VBoxContainer, label_text: String, field: Control) -> Control:
	var lbl := Label.new()
	lbl.text = label_text
	parent.add_child(lbl)
	field.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	parent.add_child(field)
	return field

func _add_option(parent: VBoxContainer, label_text: String) -> OptionButton:
	var lbl := Label.new()
	lbl.text = label_text
	parent.add_child(lbl)
	var opt := OptionButton.new()
	opt.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	parent.add_child(opt)
	return opt

func _set_details_enabled(enabled: bool) -> void:
	_tab_container.modulate.a = 1.0 if enabled else 0.4
	_fld_title.editable   = enabled
	_fld_assigned.editable = enabled
	_fld_due.editable      = enabled
	_fld_desc.editable     = enabled
	_fld_type.disabled     = !enabled
	_fld_status.disabled   = !enabled
	_fld_priority.disabled = !enabled
	_btn_apply_topic.disabled = !enabled
	_btn_add_comment.disabled = !enabled

# ── Topic tree population ──────────────────────────────────────────────────────

func _populate_topics() -> void:
	_topic_tree.clear()
	var root_item := _topic_tree.create_item()

	if _project == null:
		return

	for topic: BCFTopic in _project.topics:
		var item := _topic_tree.create_item(root_item)
		item.set_text(0, topic.title if topic.title else "(untitled)")
		item.set_text(1, topic.topic_type)
		item.set_text(2, topic.topic_status)

		var type_col := TYPE_COLORS.get(topic.topic_type.to_upper(), Color(0.8, 0.8, 0.8))
		item.set_custom_color(1, type_col)

		var status_col := STATUS_COLORS.get(topic.topic_status.to_upper(), Color(0.8, 0.8, 0.8))
		item.set_custom_color(2, status_col)

		item.set_metadata(0, topic)

func _populate_extension_options() -> void:
	_fld_type.clear()
	_fld_status.clear()
	_fld_priority.clear()

	if _project == null or _project.extensions == null:
		return

	for t in _project.extensions.topic_types:
		_fld_type.add_item(t)
	for s in _project.extensions.topic_statuses:
		_fld_status.add_item(s)
	for p in _project.extensions.priorities:
		_fld_priority.add_item(p)

# ── Topic detail population ────────────────────────────────────────────────────

func _load_topic_into_form(topic: BCFTopic) -> void:
	_selected_topic = topic
	_selected_viewpoint = null
	_set_details_enabled(true)

	_fld_guid.text     = topic.guid
	_fld_title.text    = topic.title
	_fld_assigned.text = topic.assigned_to
	_fld_due.text      = topic.due_date
	_fld_desc.text     = topic.description

	_select_option(_fld_type,     topic.topic_type)
	_select_option(_fld_status,   topic.topic_status)
	_select_option(_fld_priority, topic.priority)

	_populate_comments(topic)
	_populate_viewpoints(topic)

func _select_option(opt: OptionButton, value: String) -> void:
	for i in opt.item_count:
		if opt.get_item_text(i) == value:
			opt.selected = i
			return

func _populate_comments(topic: BCFTopic) -> void:
	for child in _comment_list.get_children():
		child.queue_free()

	_fld_comment_text.text = ""

	for comment: BCFComment in topic.comments:
		var panel := PanelContainer.new()
		var vb := VBoxContainer.new()
		panel.add_child(vb)

		var header := HBoxContainer.new()
		var lbl_author := Label.new()
		lbl_author.text = comment.author
		lbl_author.add_theme_font_size_override("font_size", 10)
		lbl_author.add_theme_color_override("font_color", Color(0.7, 0.85, 1.0))
		header.add_child(lbl_author)

		var lbl_date := Label.new()
		lbl_date.text = " · " + comment.date.left(10)  # show date only
		lbl_date.add_theme_font_size_override("font_size", 10)
		lbl_date.add_theme_color_override("font_color", Color(0.6, 0.6, 0.6))
		header.add_child(lbl_date)

		vb.add_child(header)

		var lbl_body := Label.new()
		lbl_body.text = comment.comment
		lbl_body.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		lbl_body.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		vb.add_child(lbl_body)

		_comment_list.add_child(panel)

func _populate_viewpoints(topic: BCFTopic) -> void:
	for child in _vp_entries_box.get_children():
		child.queue_free()

	if topic.viewpoints.is_empty():
		var lbl := Label.new()
		lbl.text = "(no viewpoints)"
		lbl.add_theme_color_override("font_color", Color(0.5, 0.5, 0.5))
		_vp_entries_box.add_child(lbl)
		_vp_detail_panel.visible = false
		_selected_viewpoint = null
		_btn_delete_vp.disabled = true
		return

	for vp: BCFViewpointRef in topic.viewpoints:
		var btn := Button.new()
		btn.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		btn.alignment = HORIZONTAL_ALIGNMENT_LEFT

		var label_text := vp.viewpoint_filename if vp.viewpoint_filename else vp.guid.left(8) + "…"
		var vis: BCFVisualizationInfo = vp.visualization_info
		if vis:
			match vis.camera_type:
				BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE:
					label_text += "  [Persp]"
				BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL:
					label_text += "  [Ortho]"
			if vis.components:
				var n: int = vis.components.selection.size()
				if n > 0:
					label_text += "  %d IFC" % n
		btn.text = label_text

		if vp == _selected_viewpoint:
			btn.add_theme_color_override("font_color", Color(0.3, 0.8, 1.0))

		var captured_vp := vp  # capture for lambda
		btn.pressed.connect(func(): _select_viewpoint(captured_vp))
		_vp_entries_box.add_child(btn)


func _select_viewpoint(vp: BCFViewpointRef) -> void:
	_selected_viewpoint = vp
	_btn_delete_vp.disabled = false
	_vp_detail_panel.visible = true
	_load_viewpoint_into_panel(vp)
	# Refresh list so the highlight updates
	if _selected_topic:
		_populate_viewpoints(_selected_topic)


func _load_viewpoint_into_panel(vp: BCFViewpointRef) -> void:
	var vis: BCFVisualizationInfo = vp.visualization_info
	var cam_type := BCFVisualizationInfo.BCF_CAMERA_NONE
	if vis:
		cam_type = vis.camera_type
	# Select matching item by id
	for i in _vp_camera_opt.item_count:
		if _vp_camera_opt.get_item_id(i) == cam_type:
			_vp_camera_opt.selected = i
			break
	_refresh_vp_components_list()


func _refresh_vp_components_list() -> void:
	for child in _vp_components_box.get_children():
		child.queue_free()

	if _selected_viewpoint == null:
		return
	var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
	if vis == null or vis.components == null or vis.components.selection.is_empty():
		var lbl := Label.new()
		lbl.text = "(none)"
		lbl.add_theme_color_override("font_color", Color(0.5, 0.5, 0.5))
		lbl.add_theme_font_size_override("font_size", 10)
		_vp_components_box.add_child(lbl)
		return

	for comp: BCFComponent in vis.components.selection:
		var row := HBoxContainer.new()
		var lbl := Label.new()
		lbl.text = comp.ifc_guid
		lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		lbl.add_theme_font_size_override("font_size", 10)
		row.add_child(lbl)
		var captured_comp := comp
		var btn_rm := Button.new()
		btn_rm.text = "×"
		btn_rm.tooltip_text = "Remove this component"
		btn_rm.pressed.connect(func():
			var _sel: Array = vis.components.selection
			_sel.erase(captured_comp)
			vis.components.selection = _sel
			_dirty = true
			_refresh_vp_components_list()
			_populate_viewpoints(_selected_topic)
		)
		row.add_child(btn_rm)
		_vp_components_box.add_child(row)

# ── Toolbar callbacks ──────────────────────────────────────────────────────────

func _on_open_pressed() -> void:
	var dlg := EditorFileDialog.new()
	dlg.title = "Open BCF File"
	dlg.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	dlg.access = EditorFileDialog.ACCESS_FILESYSTEM
	dlg.add_filter("*.bcf", "BCF-XML files")
	dlg.file_selected.connect(_on_open_file_selected)
	add_child(dlg)
	dlg.popup_centered(Vector2i(800, 600))

func _on_open_file_selected(path: String) -> void:
	var os_path := _to_os_path(path)
	_current_path = os_path
	_lbl_file.text = os_path.get_file()
	_lbl_file.tooltip_text = os_path
	_dbg("Opening: " + os_path)
	_manager.load_bcf(os_path)

func _on_new_pressed() -> void:
	var dlg := AcceptDialog.new()
	dlg.title = "New BCF Project"

	var vb := VBoxContainer.new()
	var lbl := Label.new()
	lbl.text = "Project name:"
	vb.add_child(lbl)
	var le := LineEdit.new()
	le.name = "ProjectName"
	le.placeholder_text = "My Coordination Project"
	vb.add_child(le)

	dlg.add_child(vb)
	dlg.get_ok_button().pressed.connect(func():
		var project_name: String = le.text.strip_edges()
		if project_name.is_empty():
			project_name = "New BCF Project"
		_project = _manager.create_project(project_name)
		_current_path = ""
		_lbl_file.text = "(unsaved)"
		_btn_save.disabled = true
		_dirty = true
		_dbg("Created project: \"" + project_name + "\"", "ok")
		_populate_extension_options()
		_populate_topics()
		_set_details_enabled(false)
		_selected_topic = null
		dlg.queue_free()
	)
	add_child(dlg)
	dlg.popup_centered()

func _on_save_pressed() -> void:
	if _current_path.is_empty() or _project == null:
		_on_save_as_pressed()
		return
	var os_path := _to_os_path(_current_path)
	_dbg("Saving to: " + os_path)
	_manager.save_bcf(_project, os_path)

func _on_save_as_pressed() -> void:
	if _project == null:
		return
	var dlg := EditorFileDialog.new()
	dlg.title = "Save BCF File"
	dlg.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	dlg.access = EditorFileDialog.ACCESS_FILESYSTEM
	dlg.add_filter("*.bcf", "BCF-XML files")
	dlg.file_selected.connect(func(path: String):
		var os_path := _to_os_path(path)
		_current_path = os_path
		_lbl_file.text = os_path.get_file()
		_lbl_file.tooltip_text = os_path
		_btn_save.disabled = false
		_dbg("Saving to: " + os_path)
		_manager.save_bcf(_project, os_path)
	)
	add_child(dlg)
	dlg.popup_centered(Vector2i(800, 600))

# ── Topic callbacks ────────────────────────────────────────────────────────────

func _on_topic_selected() -> void:
	var item := _topic_tree.get_selected()
	if item == null:
		return
	var topic: BCFTopic = item.get_metadata(0)
	if topic:
		_load_topic_into_form(topic)

func _on_add_topic_pressed() -> void:
	if _project == null:
		_show_error("Open or create a BCF project first.")
		return

	var type: String   = _project.extensions.topic_types[0]   if _project.extensions.topic_types.size()   > 0 else "OTHER"
	var status: String = _project.extensions.topic_statuses[0] if _project.extensions.topic_statuses.size() > 0 else "OPEN"

	var _author := OS.get_environment("USERNAME")
	if _author.is_empty():
		_author = OS.get_environment("USER")
	if _author.is_empty():
		_author = "editor"
	var topic: BCFTopic = _manager.create_topic(type, status, "New Topic", _author)
	var _topics := _project.topics
	_topics.append(topic)
	_project.topics = _topics
	_dbg("Added topic \"%s\" [%s/%s]" % [topic.title, type, status], "ok")
	_dirty = true
	_populate_topics()
	# Select the new item in the tree
	var root_item := _topic_tree.get_root()
	if root_item:
		var last := root_item.get_child(root_item.get_child_count() - 1)
		if last:
			last.select(0)
			_load_topic_into_form(topic)

func _on_delete_topic_pressed() -> void:
	if _selected_topic == null or _project == null:
		return
	var idx := _project.topics.find(_selected_topic)
	if idx >= 0:
		_dbg("Deleted topic \"%s\"" % _selected_topic.title, "warn")
		var _topics := _project.topics
		_topics.remove_at(idx)
		_project.topics = _topics
		_selected_topic = null
		_dirty = true
		_populate_topics()
		_set_details_enabled(false)

func _on_apply_topic_pressed() -> void:
	if _selected_topic == null:
		return

	_selected_topic.title       = _fld_title.text.strip_edges()
	_selected_topic.assigned_to = _fld_assigned.text.strip_edges()
	_selected_topic.due_date    = _fld_due.text.strip_edges()
	_selected_topic.description = _fld_desc.text

	if _fld_type.selected >= 0:
		_selected_topic.topic_type = _fld_type.get_item_text(_fld_type.selected)
	if _fld_status.selected >= 0:
		_selected_topic.topic_status = _fld_status.get_item_text(_fld_status.selected)
	if _fld_priority.selected >= 0:
		_selected_topic.priority = _fld_priority.get_item_text(_fld_priority.selected)

	_dbg("Applied topic \"%s\" [%s/%s]" % [_selected_topic.title, _selected_topic.topic_type, _selected_topic.topic_status], "ok")
	_dirty = true
	_populate_topics()

func _on_add_comment_pressed() -> void:
	if _selected_topic == null:
		return
	var text := _fld_comment_text.text.strip_edges()
	if text.is_empty():
		return

	var comment := BCFComment.new()
	comment.guid    = _manager.generate_guid()
	comment.author  = OS.get_environment("USERNAME") if OS.get_environment("USERNAME") else "editor"
	comment.date    = Time.get_datetime_string_from_system(true) + "Z"
	comment.comment = text

	var _comments := _selected_topic.comments
	_comments.append(comment)
	_selected_topic.comments = _comments
	_fld_comment_text.text = ""
	_dbg("Added comment to \"%s\" by %s" % [_selected_topic.title, comment.author], "ok")
	_dirty = true
	_populate_comments(_selected_topic)

# ── Viewpoint callbacks ───────────────────────────────────────────────────────

func _on_add_viewpoint_pressed() -> void:
	if _selected_topic == null:
		_show_error("Select a topic first.")
		return
	var vp := BCFViewpointRef.new()
	vp.guid = _manager.generate_guid()
	vp.viewpoint_filename = vp.guid + ".bcfv"
	var vis := BCFVisualizationInfo.new()
	vis.guid = vp.guid
	vis.camera_type = BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE
	var default_cam := BCFPerspectiveCamera.new()
	default_cam.view_point   = Vector3(0, 0, 0)
	default_cam.direction    = Vector3(0, 0, -1)
	default_cam.up_vector    = Vector3(0, 1, 0)
	default_cam.fov          = 60.0
	default_cam.aspect_ratio = 1.0
	vis.perspective_camera = default_cam
	var comps := BCFComponents.new()
	vis.components = comps
	vp.visualization_info = vis
	var _vps := _selected_topic.viewpoints
	_vps.append(vp)
	_selected_topic.viewpoints = _vps
	_dbg("Added viewpoint %s to topic \"%s\"" % [vp.guid.left(8), _selected_topic.title], "ok")
	_dirty = true
	_populate_viewpoints(_selected_topic)
	_select_viewpoint(vp)


func _on_delete_viewpoint_pressed() -> void:
	if _selected_viewpoint == null or _selected_topic == null:
		return
	_dbg("Deleted viewpoint %s" % _selected_viewpoint.guid.left(8), "warn")
	var _vps := _selected_topic.viewpoints
	_vps.erase(_selected_viewpoint)
	_selected_topic.viewpoints = _vps
	_selected_viewpoint = null
	_btn_delete_vp.disabled = true
	_vp_detail_panel.visible = false
	_dirty = true
	_populate_viewpoints(_selected_topic)


func _on_vp_camera_type_changed(index: int) -> void:
	if _selected_viewpoint == null:
		return
	var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
	if vis == null:
		return
	vis.camera_type = _vp_camera_opt.get_item_id(index)
	_dbg("Viewpoint camera type → %s" % _vp_camera_opt.get_item_text(index))
	_dirty = true
	_populate_viewpoints(_selected_topic)


func _on_capture_camera_pressed() -> void:
	if _selected_viewpoint == null:
		_dbg("Capture camera: no viewpoint selected", "warn")
		return
	var subvp: SubViewport = EditorInterface.get_editor_viewport_3d(0)
	if subvp == null:
		_dbg("Capture camera: get_editor_viewport_3d(0) returned null", "err")
		_show_error("No 3D editor viewport found.")
		return
	var cam: Camera3D = subvp.get_camera_3d()
	if cam == null:
		_dbg("Capture camera: get_camera_3d() returned null (open a 3D scene first)", "err")
		_show_error("No active Camera3D in the 3D editor viewport.")
		return

	var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
	if vis == null:
		vis = BCFVisualizationInfo.new()
		vis.guid = _selected_viewpoint.guid
		_selected_viewpoint.visualization_info = vis

	var fwd := -cam.global_transform.basis.z
	var up  :=  cam.global_transform.basis.y
	var pos :=  cam.global_position
	var aspect := (float(subvp.size.x) / float(subvp.size.y)) if subvp.size.y > 0 else 1.0

	if cam.projection == Camera3D.PROJECTION_PERSPECTIVE:
		vis.camera_type = BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE
		if vis.perspective_camera == null:
			vis.perspective_camera = BCFPerspectiveCamera.new()
		vis.perspective_camera.view_point   = pos
		vis.perspective_camera.direction    = fwd
		vis.perspective_camera.up_vector    = up
		vis.perspective_camera.fov          = cam.fov
		vis.perspective_camera.aspect_ratio = aspect
	else:
		vis.camera_type = BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL
		if vis.orthogonal_camera == null:
			vis.orthogonal_camera = BCFOrthogonalCamera.new()
		vis.orthogonal_camera.view_point          = pos
		vis.orthogonal_camera.direction           = fwd
		vis.orthogonal_camera.up_vector           = up
		vis.orthogonal_camera.view_to_world_scale = cam.size
		vis.orthogonal_camera.aspect_ratio        = aspect

	_dirty = true
	var cam_type_name := "orthogonal" if cam.projection != Camera3D.PROJECTION_PERSPECTIVE else "perspective"
	_dbg("Captured %s camera pos=%s fov=%.1f aspect=%.2f" % [cam_type_name, str(pos), cam.fov, aspect], "ok")
	_populate_viewpoints(_selected_topic)
	_load_viewpoint_into_panel(_selected_viewpoint)


func _on_add_scene_selection_pressed() -> void:
	if _selected_viewpoint == null:
		_dbg("Add scene selection: no viewpoint selected", "warn")
		return
	var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
	if vis == null:
		vis = BCFVisualizationInfo.new()
		vis.guid = _selected_viewpoint.guid
		_selected_viewpoint.visualization_info = vis
	if vis.components == null:
		vis.components = BCFComponents.new()

	var nodes := EditorInterface.get_selection().get_selected_nodes()
	_dbg("Scene selection: %d node(s) selected" % nodes.size())
	var sel: Array = vis.components.selection
	var added := 0
	var skipped_no_guid := 0
	var skipped_duplicate := 0
	for node: Node in nodes:
		var guid_str := _resolve_ifc_guid(node)
		if guid_str.is_empty():
			skipped_no_guid += 1
			continue
		# Skip duplicates
		var skip := false
		for existing: BCFComponent in sel:
			if existing.ifc_guid == guid_str:
				skip = true
				break
		if skip:
			skipped_duplicate += 1
			continue
		var comp := BCFComponent.new()
		comp.ifc_guid = guid_str
		comp.originating_system = "GDBCF Editor"
		sel.append(comp)
		added += 1

	vis.components.selection = sel  # commit: COW requires re-assignment
	_dbg("Scene selection result: +%d added, %d no-guid, %d duplicate" % [added, skipped_no_guid, skipped_duplicate],
			"ok" if added > 0 else "warn")
	if added > 0:
		_dirty = true
		_refresh_vp_components_list()
		_populate_viewpoints(_selected_topic)


# Try every known GDIFC path to extract the IFC GlobalId from a node.
# Returns an empty string when nothing is found.
func _resolve_ifc_guid(node: Node) -> String:
	# ── Path 1: explicit getter (custom bridge or GDIFC future API)
	if node.has_method("get_ifc_guid"):
		var v: Variant = node.get_ifc_guid()
		if v != null and str(v) != "":
			_dbg("  %s → guid via get_ifc_guid(): %s" % [node.name, str(v)])
			return str(v)

	# ── Path 2: direct ifc_guid property
	if "ifc_guid" in node:
		var v: Variant = node.get("ifc_guid")
		if v != null and str(v) != "":
			_dbg("  %s → guid via .ifc_guid: %s" % [node.name, str(v)])
			return str(v)

	# ── Path 3: IFCNode → ifc_object.get_attribute("GlobalId")  (main GDIFC path)
	if "ifc_object" in node:
		var obj: Variant = node.get("ifc_object")
		if obj != null:
			if obj.has_method("get_attribute"):
				var v: Variant = obj.get_attribute("GlobalId")
				if v != null and str(v) != "":
					_dbg("  %s → guid via ifc_object.get_attribute(GlobalId): %s" % [node.name, str(v)])
					return str(v)
				_dbg("  %s: ifc_object.get_attribute(\"GlobalId\") returned null/empty" % node.name, "warn")
			else:
				_dbg("  %s: ifc_object has no get_attribute method (type=%s)" % [node.name, typeof(obj)], "warn")
		else:
			_dbg("  %s: ifc_object property is null" % node.name, "warn")

	# ── Path 4: IFCNode → attributes Dictionary
	if "attributes" in node:
		var attrs: Variant = node.get("attributes")
		if attrs is Dictionary:
			var v: Variant = attrs.get("GlobalId", null)
			if v != null and str(v) != "":
				_dbg("  %s → guid via .attributes[GlobalId]: %s" % [node.name, str(v)])
				return str(v)
			_dbg("  %s: attributes dict has no GlobalId key. Keys: %s" % [node.name, str(attrs.keys().slice(0, 8))], "warn")

	# ── Nothing found — log what properties/methods were actually present
	var present: Array[String] = []
	for prop in ["ifc_guid", "ifc_object", "attributes", "ifc_class", "properties"]:
		if prop in node:
			present.append(prop)
	for meth in ["get_ifc_guid", "get_attribute", "get_all_attributes"]:
		if node.has_method(meth):
			present.append(meth + "()")
	_dbg("  %s (%s): no IFC GUID found. Present IFC keys: [%s]" % [
			node.name, node.get_class(), ", ".join(present)], "warn")
	return ""


func _on_clear_vp_selection_pressed() -> void:
	if _selected_viewpoint == null:
		return
	var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
	if vis == null or vis.components == null:
		return
	var n: int = vis.components.selection.size()
	var _sel: Array = vis.components.selection
	_sel.clear()
	vis.components.selection = _sel
	_dbg("Cleared %d components from viewpoint %s" % [n, _selected_viewpoint.guid.left(8)], "warn")
	_dirty = true
	_refresh_vp_components_list()
	_populate_viewpoints(_selected_topic)


# ── GDBCFManager signal handlers ──────────────────────────────────────────────

func _on_bcf_loaded(project: BCFProject) -> void:
	_project = project
	_dirty = false
	_selected_topic = null
	_selected_viewpoint = null
	_btn_save.disabled = false
	var pi_name := ""
	if project.project_info:
		pi_name = project.project_info.name
	_dbg("Loaded \"%s\"  topics:%d  docs:%d  path:%s" % [
			pi_name, project.topics.size(), project.documents.size(), _current_path.get_file()], "ok")
	_populate_extension_options()
	_populate_topics()
	_set_details_enabled(false)

func _on_bcf_saved() -> void:
	_dirty = false
	_dbg("Saved → " + _current_path.get_file(), "ok")

func _on_error(message: String) -> void:
	_dbg("Error: " + message, "err")
	_show_error(message)

# ── Utilities ─────────────────────────────────────────────────────────────────

# Convert a Godot virtual path (res:// / user://) to a real OS path so the
# C++ BCF writer can open it with standard file I/O.
func _to_os_path(path: String) -> String:
	if path.begins_with("res://") or path.begins_with("user://"):
		var globalized := ProjectSettings.globalize_path(path)
		_dbg("Globalized path: %s → %s" % [path, globalized])
		return globalized
	return path

func _show_error(message: String) -> void:
	var dlg := AcceptDialog.new()
	dlg.title = "GDBCF Error"
	dlg.dialog_text = message
	dlg.confirmed.connect(dlg.queue_free)
	add_child(dlg)
	dlg.popup_centered()

# ── Debug panel ───────────────────────────────────────────────────────────────

func _build_debug_panel(root: VBoxContainer) -> void:
	_debug_panel = PanelContainer.new()
	_debug_panel.visible = false
	_debug_panel.custom_minimum_size = Vector2(0, 160)
	_debug_panel.size_flags_vertical = Control.SIZE_EXPAND_FILL
	root.add_child(_debug_panel)

	var vb := VBoxContainer.new()
	vb.add_theme_constant_override("separation", 4)
	_debug_panel.add_child(vb)

	var header := HBoxContainer.new()
	var hdr_lbl := Label.new()
	hdr_lbl.text = "Debug Log"
	hdr_lbl.add_theme_font_size_override("font_size", 11)
	hdr_lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	header.add_child(hdr_lbl)

	var btn_dump := _make_button("Dump", _on_dump_state_pressed)
	btn_dump.tooltip_text = "Print current project / topic / viewpoint state"
	header.add_child(btn_dump)

	var btn_copy := _make_button("Copy", _on_copy_log_pressed)
	btn_copy.tooltip_text = "Copy log text to clipboard"
	header.add_child(btn_copy)

	var btn_clear := _make_button("Clear", func(): _debug_log.clear())
	btn_clear.tooltip_text = "Clear log"
	header.add_child(btn_clear)
	vb.add_child(header)

	_debug_log = RichTextLabel.new()
	_debug_log.bbcode_enabled = true
	_debug_log.scroll_following = true
	_debug_log.selection_enabled = true
	_debug_log.size_flags_vertical = Control.SIZE_EXPAND_FILL
	vb.add_child(_debug_log)


func _on_debug_toggled(pressed: bool) -> void:
	if _debug_panel:
		_debug_panel.visible = pressed


func _on_copy_log_pressed() -> void:
	if _debug_log:
		DisplayServer.clipboard_set(_debug_log.get_parsed_text())


func _on_dump_state_pressed() -> void:
	_dbg("── State Dump ──")
	if _project == null:
		_dbg("  project: null", "warn")
	else:
		var pname := _project.project_info.name if _project.project_info else "?"
		_dbg("  project: \"%s\"  id:%s  topics:%d  docs:%d" % [
				pname, _project.project_info.project_id if _project.project_info else "?",
				_project.topics.size(), _project.documents.size()], "ok")
		if _project.extensions:
			_dbg("  extensions: %d types, %d statuses, %d priorities" % [
					_project.extensions.topic_types.size(),
					_project.extensions.topic_statuses.size(),
					_project.extensions.priorities.size()])
	if _selected_topic == null:
		_dbg("  topic: none selected")
	else:
		_dbg("  topic: \"%s\" guid:%s [%s/%s]  comments:%d  vps:%d" % [
				_selected_topic.title, _selected_topic.guid.left(8),
				_selected_topic.topic_type, _selected_topic.topic_status,
				_selected_topic.comments.size(), _selected_topic.viewpoints.size()])
	if _selected_viewpoint == null:
		_dbg("  viewpoint: none selected")
	else:
		var vis: BCFVisualizationInfo = _selected_viewpoint.visualization_info
		var cam_name := "none"
		if vis:
			match vis.camera_type:
				BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE: cam_name = "perspective"
				BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL:  cam_name = "orthogonal"
		var comp_n := 0
		if vis and vis.components:
			comp_n = vis.components.selection.size()
			for c: BCFComponent in vis.components.selection:
				_dbg("    ifc_guid: " + c.ifc_guid)
		_dbg("  viewpoint: %s  cam:%s  components:%d" % [
				_selected_viewpoint.guid.left(8), cam_name, comp_n])
	_dbg("── End Dump ──")


# level: "info" | "ok" | "warn" | "err"
func _dbg(msg: String, level: String = "info") -> void:
	if _debug_log == null:
		return
	var color := "#cccccc"
	var prefix := "[INFO]"
	match level:
		"ok":   color = "#55dd88"; prefix = "[ OK ]"
		"warn": color = "#ffcc44"; prefix = "[WARN]"
		"err":  color = "#ff5555"; prefix = "[ERR ]"
	var ts := Time.get_time_string_from_system()
	var safe := msg.replace("[", "\\[")
	_debug_log.append_text("[color=#555555]%s[/color] [color=%s]%s %s[/color]\n" % [ts, color, prefix, safe])


func _get_minimum_size() -> Vector2:
	return Vector2(260, 400)

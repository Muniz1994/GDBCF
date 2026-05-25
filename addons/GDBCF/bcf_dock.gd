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
var _viewpoint_list: VBoxContainer

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
	var scroll := ScrollContainer.new()
	scroll.name = "Viewpoints"
	scroll.size_flags_vertical = Control.SIZE_EXPAND_FILL
	_tab_container.add_child(scroll)

	_viewpoint_list = VBoxContainer.new()
	_viewpoint_list.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_viewpoint_list.add_theme_constant_override("separation", 6)
	scroll.add_child(_viewpoint_list)

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
	for child in _viewpoint_list.get_children():
		child.queue_free()

	for vp: BCFViewpointRef in topic.viewpoints:
		var hb := HBoxContainer.new()

		var lbl := Label.new()
		lbl.text = vp.viewpoint_filename if vp.viewpoint_filename else vp.guid
		lbl.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		lbl.add_theme_font_size_override("font_size", 11)
		hb.add_child(lbl)

		var vis: BCFVisualizationInfo = vp.visualization_info
		if vis:
			var cam_lbl := Label.new()
			match vis.camera_type:
				BCFVisualizationInfo.BCF_CAMERA_PERSPECTIVE:
					cam_lbl.text = "Perspective"
					cam_lbl.add_theme_color_override("font_color", Color(0.4, 0.8, 1.0))
				BCFVisualizationInfo.BCF_CAMERA_ORTHOGONAL:
					cam_lbl.text = "Orthogonal"
					cam_lbl.add_theme_color_override("font_color", Color(0.8, 1.0, 0.4))
				_:
					cam_lbl.text = "No camera"
			cam_lbl.add_theme_font_size_override("font_size", 10)
			hb.add_child(cam_lbl)

			var comp_lbl := Label.new()
			if vis.components:
				var sel_count: int = vis.components.selection.size()
				comp_lbl.text = "  %d selected" % sel_count
				comp_lbl.add_theme_font_size_override("font_size", 10)
				comp_lbl.add_theme_color_override("font_color", Color(0.7, 0.7, 0.7))
				hb.add_child(comp_lbl)

		_viewpoint_list.add_child(hb)
		_viewpoint_list.add_child(HSeparator.new())

	if topic.viewpoints.is_empty():
		var lbl := Label.new()
		lbl.text = "(no viewpoints)"
		lbl.add_theme_color_override("font_color", Color(0.5, 0.5, 0.5))
		_viewpoint_list.add_child(lbl)

# ── Toolbar callbacks ──────────────────────────────────────────────────────────

func _on_open_pressed() -> void:
	var dlg := EditorFileDialog.new()
	dlg.title = "Open BCF File"
	dlg.file_mode = EditorFileDialog.FILE_MODE_OPEN_FILE
	dlg.add_filter("*.bcf", "BCF-XML files")
	dlg.file_selected.connect(_on_open_file_selected)
	add_child(dlg)
	dlg.popup_centered(Vector2i(800, 600))

func _on_open_file_selected(path: String) -> void:
	_current_path = path
	_lbl_file.text = path.get_file()
	_lbl_file.tooltip_text = path
	_manager.load_bcf(path)

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
	_manager.save_bcf(_project, _current_path)

func _on_save_as_pressed() -> void:
	if _project == null:
		return
	var dlg := EditorFileDialog.new()
	dlg.title = "Save BCF File"
	dlg.file_mode = EditorFileDialog.FILE_MODE_SAVE_FILE
	dlg.add_filter("*.bcf", "BCF-XML files")
	dlg.file_selected.connect(func(path: String):
		_current_path = path
		_lbl_file.text = path.get_file()
		_lbl_file.tooltip_text = path
		_btn_save.disabled = false
		_manager.save_bcf(_project, path)
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

	var topic: BCFTopic = _manager.create_topic(type, status, "New Topic", "")
	_project.topics.append(topic)
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
		_project.topics.remove_at(idx)
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

	_selected_topic.comments.append(comment)
	_fld_comment_text.text = ""
	_dirty = true
	_populate_comments(_selected_topic)

# ── GDBCFManager signal handlers ──────────────────────────────────────────────

func _on_bcf_loaded(project: BCFProject) -> void:
	_project = project
	_dirty = false
	_selected_topic = null
	_btn_save.disabled = false
	_populate_extension_options()
	_populate_topics()
	_set_details_enabled(false)

func _on_bcf_saved() -> void:
	_dirty = false

func _on_error(message: String) -> void:
	_show_error(message)

# ── Utilities ─────────────────────────────────────────────────────────────────

func _show_error(message: String) -> void:
	var dlg := AcceptDialog.new()
	dlg.title = "GDBCF Error"
	dlg.dialog_text = message
	dlg.confirmed.connect(dlg.queue_free)
	add_child(dlg)
	dlg.popup_centered()

func _get_minimum_size() -> Vector2:
	return Vector2(260, 400)

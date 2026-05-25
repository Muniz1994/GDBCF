@tool
extends EditorPlugin

const BCFDock := preload("res://addons/GDBCF/bcf_dock.gd")

var _dock: Control

func _enter_tree() -> void:
	_dock = BCFDock.new()
	# DOCK_SLOT_RIGHT_BL places the tab on the right panel (same side as Inspector/Signals)
	add_control_to_dock(DOCK_SLOT_RIGHT_BL, _dock)

func _exit_tree() -> void:
	if _dock:
		remove_control_from_docks(_dock)
		_dock.queue_free()
		_dock = null

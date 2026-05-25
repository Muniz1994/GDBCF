#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFComponent : public Resource {
    GDCLASS(BCFComponent, Resource)

    String ifc_guid;
    String originating_system;
    String authoring_tool_id;

protected:
    static void _bind_methods();

public:
    void set_ifc_guid(const String &v)          { ifc_guid = v; }
    String get_ifc_guid() const                 { return ifc_guid; }

    void set_originating_system(const String &v){ originating_system = v; }
    String get_originating_system() const       { return originating_system; }

    void set_authoring_tool_id(const String &v) { authoring_tool_id = v; }
    String get_authoring_tool_id() const        { return authoring_tool_id; }
};

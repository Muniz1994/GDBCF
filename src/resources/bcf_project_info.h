#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFProjectInfo : public Resource {
    GDCLASS(BCFProjectInfo, Resource)

    String project_id;
    String name;

protected:
    static void _bind_methods();

public:
    void set_project_id(const String &v) { project_id = v; }
    String get_project_id() const        { return project_id; }

    void set_name(const String &v)       { name = v; }
    String get_name() const              { return name; }
};

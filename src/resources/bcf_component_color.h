#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// Color attribute is a 6 or 8 char hex string (RGB or ARGB) as per BCF spec.
class BCFComponentColor : public Resource {
    GDCLASS(BCFComponentColor, Resource)

    String color;       // e.g. "FF0000" or "80FF0000"
    Array  components;  // Array[BCFComponent]

protected:
    static void _bind_methods();

public:
    void set_color(const String &v)     { color = v; }
    String get_color() const            { return color; }

    void set_components(const Array &v) { components = v; }
    Array get_components() const        { return components; }
};

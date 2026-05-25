#pragma once
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BCFComponents : public Resource {
    GDCLASS(BCFComponents, Resource)

    Array selection;   // Array[BCFComponent]
    Ref<Resource> visibility; // BCFComponentVisibility (typed at runtime)
    Array coloring;    // Array[BCFComponentColor]

protected:
    static void _bind_methods();

public:
    void set_selection(const Array &v)          { selection = v; }
    Array get_selection() const                 { return selection; }

    void set_visibility(const Ref<Resource> &v) { visibility = v; }
    Ref<Resource> get_visibility() const        { return visibility; }

    void set_coloring(const Array &v)           { coloring = v; }
    Array get_coloring() const                  { return coloring; }
};
